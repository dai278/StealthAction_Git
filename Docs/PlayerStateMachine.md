# プレイヤーのステートマシーン（BP 登録方式）

## 構成

`APlayerCharacter` は BP で指定された `UPlayerStateMachine` の派生クラスを生成します。
マシーンは自身の BP に設定された `State Classes` からステートを生成し、現在のステートの `Enter / Update / Exit` を実行します。C++ でステートを自動登録する処理はありません。

`GetPlayerStatus()` は現在のステートを参照します。既存の `EPlayerStatus` の名前・順序と、プレイヤーの BP 向け取得関数は維持しています。以前の `m_status` は **Initial Player Status** として初期状態の指定にだけ使います。

## エディタで必要な設定

新しい C++ クラスをビルドしてから、次を設定してください。このクラウド環境には UE がないため、既存の `.uasset` の編集・BP 作成・コンパイルは行っていません。**ソース変更だけでは移行は完了せず、この BP 設定が必要です。**

1. `PlayerStateMachine` を親に `BP_PlayerStateMachine` を作成します。
2. 下表の C++ クラスを親に、それぞれステート BP を作成します。
3. `BP_PlayerStateMachine` の **Class Defaults → State Machine → State Classes** に、作成した 11 個の BP クラスを登録します。オブジェクトのインスタンスではなくクラスを指定します。配列順序は問いません。
4. 使用中のプレイヤー BP の **Class Defaults → State Machine → Player State Machine Class** に `BP_PlayerStateMachine` を指定します。
5. プレイヤー BP の **Initial Player Status** は通常 `Idle` にします。

| ステート BP の親クラス | Player Status |
| --- | --- |
| `PlayerIdleState` | Idle |
| `PlayerWalkState` | Walk |
| `PlayerDashState` | Dash |
| `PlayerJumpState` | Jump |
| `PlayerCrouchState` | Crouch |
| `PlayerAttackState` | Attack |
| `PlayerDamageState` | Damage |
| `PlayerDeadState` | Dead |
| `PlayerShadowState` | InShadow |
| `PlayerInteractState` | Interact |
| `PlayerInteractAnimationState` | InteractAnimation |

親クラスが設定する `Player Status` は変更せず使ってください。動作を独自実装する場合は `PlayerStateBase` を親に BP を作り、対応する `Player Status` を指定して登録したクラスを置き換えることもできます。

未設定・ステータス重複・必要なステートの不足はログにエラーを出します。不完全な状態で操作処理だけが進まないよう、初期設定が失敗した場合はプレイヤーの操作を無効にします。

## BP から処理を拡張する

各ステート BP で次をオーバーライドできます。

- `On Enter`：開始処理。
- `On Update`：現在のステートだけが毎フレーム実行する処理。
- `On Exit`：終了・割り込み時の後始末。
- `Can Enter State` / `Can Exit State` / `Can Transition To Status`：通常遷移の条件。

既存動作を使いながら拡張する場合は **親の関数を呼び出す** ノードを接続してください。例えば Damage の `On Update` は経過時間による復帰を、Attack の `On Exit` は攻撃フラグ・カメラ・重力の復元を行います。親を呼ばない場合は、それらも BP で実装します。

`Get Player` でプレイヤーを取得でき、`Request Player State` で登録済みの状態への遷移を要求できます。外部 BP からはプレイヤーの `Get Player State Machine → Change Player State` を使います。戻り値は実際に遷移できたかを表します。同一ステートへの再遷移はしません。

遷移の最中に再帰的な切り替えを起こさないよう、`On Enter` / `On Exit` / 遷移条件の評価中の遷移要求は拒否します。次の状態の要求は `On Update` や後続イベントから行ってください。

Damage / Dead は通常の終了条件に関係なく割り込みます。Dead から通常遷移では復帰しません。復帰は従来どおりレベルの再開などで行います。

## 既存処理との分担

- 移動は従来の Enhanced Input による処理を使います。移動系ステートは入力、しゃがみ、ダッシュ、ジャンプのフラグを参照し、`Jump → Crouch → Dash → Walk → Idle` の優先順で状態を選びます。これらのフラグは姿勢や入力の情報で、実行中ステートの別コピーではありません。移動キーを離した場合も入力値をクリアします。
- 移動系ステートを BP から直接要求するだけでは、ジャンプの打ち上げやダッシュの入力処理を実行することにはなりません。従来の入力処理を使うか、独自 BP ステートに開始動作を実装してください。
- 無敵時間、スタミナ、ジャンプの物理更新、カメラは共通処理としてプレイヤー側で更新します。被ダメージ中なども必要な更新を止めないためです。
- 攻撃終了の通知は Attack 中だけ受け付けます。割り込み後に遅れて届いた通知で Damage / Dead の操作ロックを解除しません。
- 影の見た目・カプセル設定は Shadow の開始・終了で切り替えるため、BP からの遷移やダメージ割り込みでも後始末が実行されます。
- インタラクトの待機時間はアニメーションステートへの入場時にリセットします。終了遷移を BP が拒否した場合も `Interact` の実行は入場ごとに一度だけです。

## 検証範囲

クラウド上ではソース差分と参照関係の静的確認のみです。UE 5.7 による C++ ビルド、UHT、BP コンパイル、ゲーム実行は未実施です。

UE が使える環境で確認する場合は、上記 BP 設定後に次を確認してください。

- Idle / Walk、ダッシュ開始・解除、しゃがみ、ジャンプ・着地で状態が切り替わる。
- 攻撃完了で復帰し、攻撃を Damage / Dead で割り込んだ後の遅い通知で操作が再開しない。
- 影の開始・解除・光・スタミナ切れ・被ダメージで見た目とカプセルが戻る。
- インタラクトへの移動、アニメーション、完了、割り込みと再開始でタイマーと操作が復元する。
- BP の遷移条件が通常遷移を止め、Damage / Dead の割り込みは止めない。
- 未登録や二重登録がログで分かり、操作が無効になる。
