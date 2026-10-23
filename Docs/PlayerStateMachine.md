# プレイヤーのステートマシーン（BP 登録・ステート側で設定）

## 処理の分担

- **PlayerCharacter**：入力の取得と転送、コンポーネントの生成・保持、共通カメラ、衝突・接触情報、HP などの共有データ、通知の受け渡し。
- **PlayerStateMachine**：BP からのステート登録、遷移、入力要求、共有スタミナ残量と回復、非アクティブ時も必要なクールダウン・無敵時間の更新。
- **各ステート**：状態別のパラメータと、開始・更新・終了処理。プレイヤーの `UpdateShadow` などを呼ぶラッパーではなく、動作そのものを各 `.cpp` で実装しています。

各クラスは `Public/State/Player/<クラス名>.h` と `Private/State/Player/<クラス名>.cpp` に分かれています。

## 登録手順

1. ソースを取得して C++ をビルドします。
2. `PlayerStateMachine` を親に `BP_PlayerStateMachine` を作成します。
3. 下表の各 C++ ステートを親に、それぞれステート BP を作成します。
4. マシーン BP の **Class Defaults → State Machine → State Classes** に、11個のステート BP クラスを重複なく登録します。順序は自由です。
5. プレイヤー BP の **Class Defaults → State Machine → Player State Machine Class** にマシーン BP を指定します。
6. マシーン BP の **Initial State Class** を通常は `PlayerIdleState` にします。
7. 各 BP をコンパイルして保存します。

ステート BP は表のネイティブ親クラスから派生してください。状態の識別には親クラスの型を使います。必要なステートの不足や重複はログにエラーを出し、不完全な設定では操作を無効にします。

## 各ステート BP のパラメータ

すべて **ステート BP の Class Defaults** で設定します。共通項目の **Movement → Move Speed** はそのステートへの入場時に適用されます。通常の移動速度は cm/s、時間は秒です。

| 親クラス | 主な設定と初期値 |
| --- | --- |
| `PlayerIdleState` | Move Speed `300`、Rotation Interp Speed `10`、Footstep Noise Level `1` |
| `PlayerWalkState` | Move Speed `300`、Rotation Interp Speed `10`、Footstep Noise Level `1` |
| `PlayerDashState` | Move Speed `600`、Stamina Consumption Rate `1` |
| `PlayerCrouchState` | Move Speed `100`、Capsule Height Ratio `0.8`、Footstep Noise Level `0` |
| `PlayerJumpState` | Move Speed `300`、Jump Impulse `1500`、Gravity Curve Base `10000`、Maximum Fall Speed `1800`、Jump Noise Level `3` |
| `PlayerAttackState` | Move Speed `0`、Attack Range `150`、Attack Radius `70`、Sneak Kill Damage `10`、Attack Duration `1`、Attack Cooldown `0.3`、Minimum Facing Dot `0.4`、Attack Gravity Scale `0.1` |
| `PlayerDamageState` | Move Speed `0`、Recovery Duration `1`、Invincibility Duration `5`、Knockback Multiplier `1` |
| `PlayerDeadState` | Move Speed `0`、Open Game Over Level `true`、Game Over Level `GameOver` |
| `PlayerShadowState` | Move Speed `300`、Stamina Consumption Rate `1`、Shadow Collision Profile `PlayerInShadow`、Shadow Effect Class |
| `PlayerInteractState` | Move Speed `300`、Acceptance Radius `50`、Approach Input Scale `0.5`、Rotation Interp Speed `10` |
| `PlayerInteractAnimationState` | Move Speed `0`、Interaction Duration `2.5`、Wait For Animation Notify `false` |

- **Rotation Interp Speed**：移動・接近時の旋回補間速度。角速度そのものではありません。
- **Camera → Override Camera / Camera Settings**：状態別のカメラ距離・画角・ソケットオフセット（ofSet）・補間速度。Dash / Crouch / Shadow は初期状態で有効です。カメラの更新処理はプレイヤー共通ですが、状態別の設定はステートに保持します。`Set Event Camera Change` による演出指定は一時的に優先します。
- **Stamina Consumption Rate**：共有の消費時間が1秒あたり増える量。`1` なら5秒分のスタミナを5秒で消費します。状態から退出すると消費が止まります。
- **Allow Manual Control / Allow Movement Input**：ネイティブ親が設定済みです。攻撃・被ダメージ・死亡・インタラクト中は手動操作を止めるため、通常は変更不要です。
- 被ダメージ時の無敵時間は復帰時間以上に保ちます。`Invincibility Duration` を短くしても、反応中に同一 Damage へ再入場することを防ぎます。
- **Wait For Animation Notify** を有効にした場合、時間では終了しません。アニメーション通知からプレイヤーの既存関数 **On Interact Animation End** を呼んでください。
- 影状態ではプレイヤーのメッシュを非表示にし、解除時は入る前の表示状態に戻します。メッシュ差し替え用の Shadow Mesh は廃止しました。**Shadow Effect Class** が未設定の場合はエフェクトを生成しません。

共有スタミナの **Maximum Stamina（初期値5） / Stamina Recovery Rate（初期値1.5）** は、マシーン BP の Class Defaults → Stamina にあります。

### 既存プレイヤー BP から移す設定

**旧プレイヤー BP の設定値は自動移行されません。** 以前に変更していた値は、対応するステート BP へ設定し直してください。

| 旧プレイヤー設定 | 移動先 |
| --- | --- |
| Walk Speed | Idle / Walk の Move Speed。必要に応じて Jump / Shadow / Interact にも個別設定 |
| Dash Speed、ダッシュの消費倍率 | Dash の Move Speed / Stamina Consumption Rate |
| Crouch Speed、Cranch Collision Ratio | Crouch の Move Speed / Capsule Height Ratio |
| Jump Vector | Jump の Jump Impulse |
| 攻撃範囲・半径・ダメージ・クールダウン | Attack の各設定 |
| ダメージ復帰時間・無敵時間 | Damage の Recovery Duration / Invincibility Duration |
| 影エフェクトクラス | Shadow の Shadow Effect Class |
| 最大スタミナ・回復倍率 | マシーンの Maximum Stamina / Stamina Recovery Rate |
| Initial Player Status | マシーンの Initial State Class |
| 旧 Camera Init Pos の Dash / Crouch / InShadow | 各ステートの Camera Settings |
| 旧 Camera Init Pos の ThirdPerson / TopDownView | プレイヤーの Third Person Camera / Top Down Camera |

通常メッシュ、通常カプセル寸法、通常・俯瞰の共通カメラ設定、入力アセットは引き続きプレイヤー BP に設定します。クラウド側では `.uasset` を編集していません。

## BP で処理を追加する場合

既存動作を使うだけならイベントグラフは空で使えます。拡張時は `On Enter / On Update / On Exit` をオーバーライドし、既存動作を残すため **親の関数を呼び出す** を接続してください。`On Move Input` と `On Stamina Depleted` も拡張できます。

`Get Player` でプレイヤーを、`Get Player Machine` でマシーンを取得できます。`Request Player State` の戻り値は実際に遷移できたかを表します。外部 BP からはプレイヤーの `Get Player State Machine → Get Player State` で登録済みのステートを取得できます。

`Can Enter State / Can Exit State / Can Transition To State` は通常の遷移条件です。Damage / Dead は終了条件を無視して割り込み、Dead から通常遷移では復帰しません。入場・退出・遷移条件の評価中の再帰的な遷移要求は拒否されるので、次の遷移は `On Update` または後続イベントから要求してください。

## 動作上の扱いと検証

- 移動系の状態選択はマシーン側で行います。ジャンプ・着地、しゃがみ、ダッシュ、歩き／待機を入力と移動状態に応じて切り替えます。
- 被ダメージによる割り込みではジャンプの独自重力を終了し、以後は CharacterMovement の重力に戻します。しゃがみの要求は保持するため、復帰後は必要に応じて Crouch に戻ります。ダッシュは攻撃・被ダメージ・インタラクト・影への入場で解除します。
- 無敵時間と攻撃クールダウンは、対応するステートが非アクティブでも更新します。
- 剣コンポーネントの終了通知と、Attack の時間上限の双方で攻撃終了を扱います。割り込み後の終了通知は現在の状態を確認して無視します。
- インタラクト対象は接触情報から状態開始時に保持します。途中で接触が外れても有効な対象は維持し、破棄されたら処理を中断します。実行は入場ごとに一度だけです。

UE 5.7 による C++ / UHT / BP のビルドとゲーム実行は、この環境では未実施です。エディタで BP の値を変更した後、速度の切り替え、ジャンプ・着地、割り込み、影解除時の衝突復元、通知待ちインタラクト、スタミナ切れを確認してください。

### 影状態の障害物判定

影状態ではカプセルのサイズ・位置を変更せず、Shadow ステートBPの **Shadow Collision Profile**（既定 `PlayerInShadow`）に切り替えます。退出時は入場前のプリセットへ戻します。旧 Capsule Height Ratio は使用しません。

通過させたい障害物のコリジョン設定で、Object Response の **PlayerInShadow** を **Ignore** にしてください。床と通過不可の壁は **Block** のままにします。同じコリジョンで床と障害物をまとめている場合は、別コンポーネントに分けて設定してください。この方式は指定した障害物全体を通過するため、隙間の寸法による判定は行いません。既存の影解除時の障害物チェックと退避位置への復帰処理は継続します。

### クラスでのステート管理

ステート遷移用のEnum対応表は削除しました。State Classesの登録済みインスタンスをクラスで検索し、現在のステート自身の型で判定します。ネイティブクラスを指定しても登録したBP派生クラスを取得するため、BPの設定値は継続して使用します。同じ役割の派生クラスは1つずつ登録してください。

開始状態はステートマシンBPの Initial State Class で指定します（既定はPlayerIdleState）。Change Player State / Request Player State / Get Player State はEnumの代わりにステートクラスを指定します。Can Transition To Status は Can Transition To State に変更しました。プレイヤーの Get Player State はステート本体を返します。Get Player Status はAnimBP用のEnumを返します。旧Enumでステートを変更するBPはクラス指定のノードへ差し替えて再コンパイルしてください。Is Shadow / Is Dash / Is Crouch はそのまま使用できます。

### 共通BaseのBP処理

On Enter / On Update / On Exit と Can Enter State / Can Exit State / Can Transition To State は UStateBase にまとめました。UPlayerStateBase では再宣言せず、同じイベントを継承して使います。遷移条件のクラス引数はプレイヤー専用ではなく StateBase の派生クラスを受け取ります。既存BPでこの引数を使っている場合はノードを更新して再コンパイルしてください。

BPでイベントを上書きしたときは、イベントノードの右クリックから「親関数を呼び出す」でC++の処理を実行できます。C++では Super::OnEnter_Implementation() などを呼びます。呼ばなければ、そのイベントの親処理は実行しません。タイマー更新は共通の Enter / Update、プレイヤーの移動速度反映とカメラ切り替えは UPlayerStateBase の Enter / Exit に残すため、BPで親イベントを呼ばなくても実行します。

ステートマシンは旧ステートの Exit、現在ステートの差し替え、新ステートの Enter の順で切り替えます。通常の Update は現在のステートだけに呼びます。プレイヤー用の BackgroundUpdate は攻撃の待ち時間と無敵時間を別ステート中も進めるための処理です。

### AnimBP用の状態Enum

EPlayerStatus はアニメーション用の値として使用します。各ステートが Animation → Player Status を持ち、ネイティブの各ステートには対応する既定値を設定しています。プレイヤーの Get Player Status は現在のステートから値を取得し、ステートが無い場合は Idle を返します。AnimBPではプレイヤーから Get Player Status を取得し、Blend Poses by Enum や遷移条件に使用できます。

ステートの変更・検索・現在状態の判定は従来通りクラスで行います。Enumを別途更新する処理や、Enumからステートを選ぶ対応表はありません。

### 影に潜る演出

Shadow BP の Shadow → Appearance で Sink Duration（既定0.4秒）、Sink Distance（既定100cm）、Opacity Parameter Name（既定Opacity）を設定します。移動を止めずにメッシュだけ下げ、不透明度を下げます。潜り終わったらメッシュを非表示にします。途中の解除や被ダメージによる割り込みでも、元の相対位置・マテリアル・表示・衝突設定へ戻します。退出時は即座に復元します。

**マテリアル設定が必要です。** プレイヤーの全マテリアルに Scalar Parameter「Opacity」（既定1）を用意し、Maskedなら DitherTemporalAA の Alpha Threshold に接続し、その出力を Opacity Mask へ接続します（既存のマスクがあれば掛け合わせます）。Translucentなら Opacity に接続します。Opaqueのまま、またはパラメータを追加しただけでは透明になりません。パラメータが無いマテリアルは警告を出し、沈む動作と最後の非表示のみになります。この環境ではマテリアルアセット自体は変更していません。

演出には一時的な動的マテリアルを使い、元のマテリアルインスタンスや他のキャラクターの値を変更しません。BPでOn Enter / On Update / On Exitを上書きする場合は「親関数を呼び出す」を接続してください。
