# 新しい敵の設定手順

既存のEnemyBase・Enemy1・Enemy2と既存BPは変更していません。別のCharacterとAIControllerを使って、探索／戦闘／被ダメージ／死亡を試す構成です。既存敵を置き換えず、まず専用のテストレベルに1体配置してください。

## 仕組み

```text
ModularEnemyCharacter
  └ ModularEnemyAIController
      ├ ModularEnemyStateMachine
      │   ├ Explore：巡回・物音の調査 → BT_EnemyExplore
      │   ├ Combat：発見した相手の追跡・攻撃 → BT_EnemyCombat
      │   ├ Damage：行動を中断し、一定時間ひるむ
      │   └ Dead：BT・移動・攻撃を停止。復帰しない
      ├ Blackboard：2つのBTで同じアセットを使用
      └ BehaviorTreeComponent：同時に動かすBTは1つだけ
```

今回は「プレイヤーを発見したらCombatへ移る」としています。CombatのBTが相手への接近も担当します。見失った直後は最後に見た場所へ向かい、Lose Sight Delay（既定5秒）を過ぎたらExploreへ戻ってその場所を調査します。被ダメージ後は、被弾前の状態を丸ごと再開せず、現在の対象情報からExplore／Combatを選び直します。

BT切り替えは前のBTへSafe停止を要求し、IsAbortPendingとTreeHasBeenStartedで停止完了を確認してから次を開始します。中断待ち中に別の要求が来た場合は最新のものを使い、被ダメージ・死亡では開始予約も破棄します。終了・UnPossess時のみ強制停止します。

## 1. ビルドとアセットの作成

エディタを閉じてC++をビルドしてください。次のBPを新規作成します。既存の敵BPを複製して親を無理に差し替える必要はありません。

| 新しいアセット名の例 | 親のC++クラス |
|---|---|
| BP_ModularEnemy | ModularEnemyCharacter |
| BP_ModularEnemyAI | ModularEnemyAIController |
| BP_ModularEnemyMachine | ModularEnemyStateMachine |
| BP_ModularEnemyExplore | ModularEnemyExploreState |
| BP_ModularEnemyCombat | ModularEnemyCombatState |
| BP_ModularEnemyDamage | ModularEnemyDamageState |
| BP_ModularEnemyDead | ModularEnemyDeadState |

作成画面で表示されなければ「すべてのクラス」を展開して検索します。さらに、コンテンツブラウザで「人工知能」から次の3アセットを作ります。

- Blackboard：BB_ModularEnemy
- Behavior Tree：BT_ModularEnemyExplore
- Behavior Tree：BT_ModularEnemyCombat

## 2. Blackboard

BB_ModularEnemyに次を**同じ名前・型**で追加します。TargetActorのBase ClassはActorにします。

| キー名 | Key Type | 用途 |
|---|---|---|
| TargetActor | Object（Base Class: Actor） | 現在見えているプレイヤー |
| CanSeeTarget | Bool | 視認中か |
| LastKnownLocation | Vector | 最後にプレイヤーを見た位置 |
| HomeLocation | Vector | 敵の開始位置 |
| PatrolLocation | Vector | 次の巡回先 |
| HasInvestigation | Bool | 調べる場所があるか |
| InvestigationLocation | Vector | 物音や見失った場所の調査先 |

**両方のBTのBlackboard Assetに、この同じBB_ModularEnemyを指定します。** キーを同じにした別アセットではなく、アセット自体を共有してください。初期化時にキー名・型を確認し、間違っている場合はLogTempにエラーを出します。

## 3. 探索BT

BT_ModularEnemyExploreを開き、次の順番で配置します。Selectorの子は左が優先です。

```text
Root
└ Selector
   ├ Sequence：調査
   │  ├ Decorator：Blackboard「HasInvestigation」Is Set
   │  ├ Move To：InvestigationLocation
   │  │  └ Decorator：Force Success
   │  ├ Wait：1.0秒
   │  └ Modular Enemy Finish Investigation
   └ Sequence：巡回
      ├ Modular Enemy Patrol Point
      ├ Move To：PatrolLocation
      │  └ Decorator：Force Success
      └ Wait：1.0秒
```

DecoratorはそのSequenceやTaskを選んで右クリック→Add Decoratorから追加します。調査SequenceのBlackboard DecoratorはObserver AbortsをBothにします。Move ToのBlackboard Keyをそれぞれ選択してください。Force Successは到達できない地点でも次の待ち・調査完了へ進め、同じ失敗を繰り返し続けないためです。

Modular Enemy Patrol Pointは敵に設定したPatrol Pointsを順番に選びます。地点が未設定なら開始位置を使います。キーを書き込むだけなので、移動自体はMove Toが担当します。

## 4. 戦闘BT

```text
Root
└ Selector
   ├ Sequence：見えている相手へ接近して攻撃
   │  ├ Decorator：Blackboard「CanSeeTarget」Is Set（Observer Aborts: Both）
   │  ├ Move To：TargetActor
   │  ├ Modular Enemy Attack
   │  └ Wait：0.5秒
   ├ Sequence：最後に見た位置へ移動
   │  ├ Decorator：Blackboard「CanSeeTarget」Is Not Set（Observer Aborts: Both）
   │  ├ Move To：LastKnownLocation
   │  │  └ Decorator：Force Success
   │  └ Wait：0.5秒
   └ Wait：0.25秒
```

戦闘側のMove To（TargetActor）はAcceptance Radiusを100cmにし、Reach Test Includes Agent RadiusとReach Test Includes Goal RadiusをOFFにします。攻撃距離の既定150cm以内まで接近させるためです。障害物などで接近に失敗した場合は最後のWaitへ進み、次に再試行します。

Modular Enemy Attackは新CharacterのBeginAttackを呼び、攻撃が終わるまで待ちます。中断時はCancelAttackを呼んでその場でAbortedを返します。タスクは敵ごとのインスタンスなので、複数体の攻撃対象や待ち状態が混ざりません。攻撃は既定0.3秒後に1回だけ判定し、既定0.8秒で終了します。判定時にも距離と視線を確認します。

## 5. 各ステートBP

クラスのデフォルトで次を指定します。

| BP | 項目 | 値 |
|---|---|---|
| BP_ModularEnemyExplore | Enemy State → Behavior Tree | BT_ModularEnemyExplore |
| BP_ModularEnemyCombat | Enemy State → Behavior Tree | BT_ModularEnemyCombat |
| BP_ModularEnemyDamage | Enemy State → Recovery Time | ひるみ時間。既定0.4秒 |
| BP_ModularEnemyDead | 必須設定なし | OnDeathの演出はCharacter側 |

単に値を変えるだけならOn Enter等の上書きは不要です。上書きする場合は「親関数を呼び出す」をつなげます。大きな状態をBT側で独自に書き換える処理は追加しないでください。

BP_ModularEnemyMachineのクラスのデフォルト→State Machine→State Classesには、上の**4つのステートBPを1つずつ**登録します。StateBaseやModularEnemyStateは登録しません。初期ステートはコードが探索／戦闘から選択します。

State Classesが空ならC++の4ステートを自動登録しますが、C++既定ステートにはBTが未指定なので、巡回させるには上記BP設定を行ってください。親のC++クラスとそのBPを両方登録するなど、同じ種類の重複はエラーになります。

## 6. AIController BP

BP_ModularEnemyAIのクラスのデフォルトで指定します。

| 項目 | 設定 |
|---|---|
| Enemy → AI → Shared Blackboard | BB_ModularEnemy |
| Enemy → AI → State Machine Class | BP_ModularEnemyMachine |
| Lose Sight Delay | 見失ってから探索へ戻る時間。既定5秒 |
| Abort Warning Delay | BTの中断が完了しない場合の警告時間。既定2秒 |

Sight Configの既定は視認距離1500cm、見失い距離1800cm、視野の半角70度です。Hearing Configの既定は1500cmです。中立も検出対象に含めています。必要ならPerceptionの設定をBP側で調整してください。

このAIControllerがBTを開始するので、BPのBeginPlayなどにRun Behavior Treeを追加しないでください。もう1つBehaviorTreeComponentを追加する必要もありません。

## 7. Character BP・レベル

BP_ModularEnemyのクラスのデフォルトで指定します。

| 項目 | 設定 |
|---|---|
| Pawn → AI Controller Class | BP_ModularEnemyAI |
| Pawn → Auto Possess AI | Placed in World or Spawned |
| Mesh → Skeletal Mesh Asset | 敵に使うメッシュ |
| Mesh → Animation Mode / Anim Class | 使用するAnimBP |
| Enemy → Movement → Explore Speed | 探索速度。既定180 |
| Enemy → Movement → Combat Speed | 戦闘速度。既定350 |
| Enemy → Damage → Maximum Health | HP上限。既定40 |
| Enemy → Attack → Attack Range | 既定150cm |
| Attack Damage / Attack Windup / Attack Duration / Attack Cooldown | 既定10 / 0.3秒 / 0.8秒 / 0.5秒 |
| Attack Montage | 任意。未設定でも攻撃判定は動く |

既存敵専用AnimBPはEnemyBaseへCastしている可能性があるため、そのまま流用すると失敗します。最初は新しいAnimBPでTry Get Pawn Owner→Get Velocity→Vector Lengthから速度を取得するなど、新Characterに対応したものを使ってください。攻撃モンタージュを使う場合は対応するSlotをOutput Poseまでの経路へつなぎます。判定時間はモンタージュの長さから自動取得しないため、Windup／Durationを演出と合わせてください。

CapsuleのプリセットはコードでEnemyに設定しています。カプセルやメッシュの高さ・相対位置は使用するモデルに合わせてください。レベルにNav Mesh Bounds Volumeを置いて床と巡回経路を囲み、Pキーで移動可能な緑色の範囲を確認します。

レベルにTargetPointを複数配置し、**配置した敵インスタンス**のEnemy→Patrol→Patrol Pointsへ順番に登録してください。既存の巡回ルートManagerは使いません。

## 8. 物音・被ダメージの接続

UE標準のReport Noise Event（AI Perception）を使えばHearingで調査先を受け取ります。World Contextは現在のレベル、Noise Locationは発生位置、Instigatorは発生元Actorを指定してください。既存のUNoiseManagerは独自実装なので、自動ではこのAIへ届きません。そちらを使う場合は新AIControllerのInvestigate Locationを呼んで調査先を渡します。戦闘対象がいる間は物音による調査へ割り込みません。

被ダメージは既存のIDamageable::OnDamageと、UEのApply Damageに対応します。検証だけなら、新敵BPでダメージ用のCustom Eventを作りApply DamageのDamaged ActorへSelf、Base Damageへ10などを渡し、レベルBPから呼んで確認できます。BPでEvent AnyDamageを使ってもう一度OnDamageを呼ぶと二重適用になるため不要です。

**既存プレイヤーのステルスキル対象検索は旧EnemyManager／EnemyBase専用のままです。新しい敵はそのManagerに登録していません。** プレイヤー側の検索・ステルスキル演出、チェックポイント保存、既存の警戒ゲージ・BGM・照明／影による発見補正は今回の新クラスに自動移植していません。既存クラスを変更せず試すための独立した実装です。新敵がプレイヤーへ与える攻撃は既存IDamageableへ届きます。

## 9. UEでの確認順

1. プレイヤーが見えない向きに1体配置し、巡回先を2つ設定。巡回と待機を確認。
2. プレイヤーを視野へ入れる。探索BTが止まり、戦闘BTだけが動くことをBTエディタのDebug Objectで確認。
3. プレイヤーに接近・攻撃すること、1回の攻撃でダメージが1回だけ発生することを確認。
4. 壁の裏へ隠れる。LastKnownLocationへ移動し、5秒後に探索へ戻ることを確認。
5. 巡回中／接近中／攻撃の構え中に10ダメージを与え、移動・攻撃が中断し、ひるみ後に現在の状況へ復帰することを確認。
6. ひるみ中にも追加ダメージを与え、ひるみ時間がリセットされることを確認。
7. HPを0にし、BT・移動・攻撃が止まり、探索や戦闘へ戻らないことを確認。
8. 複数体を配置し、片方の被ダメージ／死亡が他方へ影響しないことを確認。
9. 最後にBPタスクを追加する。中断で処理待ちを始めるタスクは完了時にFinish Abortを呼び、独自タイマー・デリゲート・予約済み攻撃も解除する。

「Waiting for BT abort」が出た場合は、待ち中のタスクの中断処理を確認します。安全のため一定時間後に次のBTを無理に開始する処理は入れていません。中断が終わるまで次のBTは動きません。

## この環境での検証

Python 3とC++17対応のg++がある環境で `python Tests/ModularEnemy/run_checks.py` を実行します（別コンパイラはCXX環境変数で指定）。実際の切り替え・攻撃関数を抽出し、UE型の代用品でC++コンパイル・実行します。中断待ち、要求の置き換え、同じBTの再起動防止、Blackboard不一致、被ダメージによる割り込み、死亡後の遷移拒否などを確認します。

このテストはUHT・UE本体のBT実行・Perception・NavMesh・BP／BTアセットの動作を検証しません。このクラウド環境にはUEがないため、UE 5.7でのビルドと上のプレイ確認はエディタ側で必要です。BP／BT／Blackboardアセットはこの手順で作成する必要があり、C++追加だけでは敵は巡回しません。
