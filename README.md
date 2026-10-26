# Sneak｜ステルスアクション

影に潜り、敵の視界を避けながら宝物を盗み、脱出を目指すチーム制作作品です。
「緊張を開放感に」をコンセプトに制作しました。

## 制作情報

| 項目 | 内容 |
| --- | --- |
| 制作人数 | 5人（プログラム4人、デザイン1人） |
| 主な制作期間 | 2025年11月20日～2026年2月25日 |
| 継続開発 | 制作終了後も機能の整理・拡張と不具合修正を継続 |
| 開発環境 | Unreal Engine 5（現在のプロジェクトは5.7） |
| 使用言語 | C++、Blueprint |
| 動作環境 | Windows 11 |
| 日髙大智の役割 | プログラムリーダー |

## 担当箇所

- ライトの管理、照射範囲と遮蔽物を用いた影判定
- ゲーム内の音情報の発生・通知基盤
- プレイヤー処理の引き継ぎ・拡張と、ステートクラスへの処理の分離
- ステートマシンの共通基盤、プレイヤーおよび新しい敵向けの各ステート
- カメラ制御、アニメーションと連携するカメラ演出
- チェックポイントとプレイ情報の管理
- ステージギミックの実装、ステージ設計
- 共用の近接攻撃コンポーネント、ダメージインターフェース

ソース内の「担当日髙」が担当範囲の目印です。他メンバーの担当表記はそのまま残しています。
プレイヤー処理は開発途中から引き継いだもので、作品全体を個人制作したものではありません。

## 技術的な工夫

### 1. 照射範囲とレイキャストを組み合わせた影判定

当初は、影を表す専用のメッシュとコリジョンを配置して判定していました。
この方式では、光源の移動・回転に対して、描画上の影と判定範囲にずれが生じました。
そこで、光源とプレイヤーの足元、遮蔽物の位置関係から判定する方式へ変更しました。

1. 光源から足元までの距離が、ライトの減衰半径内かを確認します。
2. ライトの向きと足元への方向の内積から、照射角度内かを確認します。
3. 範囲内の場合、光源から足元へ`LineTraceSingleByObjectType`でレイキャストし、遮蔽物を調べます。
4. 複数のライトの判定結果を、影潜りの開始・継続条件に使用します。

影の描画にはUEのライティング機能を利用し、ゲームの判定には距離・角度の計算とレイキャストを組み合わせています。
影用コリジョンを個別に配置する手間を減らし、光源の移動や回転にも判定を追従させました。
ただし、遮蔽物の判定は描画メッシュそのものではなく設定されたコリジョンを使用するため、描画上の影との完全な一致を保証する方式ではありません。

主な実装：

- [ExtendedSpotLight.cpp](https://github.com/dai278/StealthAction_Git/blob/main/Source/StealthAction/Private/Light/ExtendedSpotLight.cpp)
- [ExtendedSpotLightManager.cpp](https://github.com/dai278/StealthAction_Git/blob/main/Source/StealthAction/Private/Light/ExtendedSpotLightManager.cpp)

### 2. プレイヤーの共通処理とステート固有処理の分離

プレイヤークラスへ移動・攻撃・影潜りなどの処理が集中しないよう、状態ごとの動作を個別のステートクラスへ分離しました。
プレイヤーには入力やコンポーネント操作などの共通処理を残し、状態の切り替えはステートマシンが管理します。

- 状態を`UObject`で管理し、開始・更新・終了の処理を共通の基底クラスに定義。
- 使用するステートのBPクラスを、ステートマシンのBPから登録。
- 移動速度・スタミナ消費量・カメラ設定などを、各ステートのBPで調整。
- 各状態の固有処理を分け、既存の状態への影響を確認しながら拡張できる構成。
- AnimBP向けの状態Enumは各ステートから返し、遷移先はステートクラスで扱う。

影ステートは、カプセルの縮小・復元、メッシュを沈める演出、マテリアルの不透明度、Niagaraエフェクトを管理します。
影への出入りはプレイヤーのデリゲートから通知し、他クラスが演出を連携できる形にしています。

主な実装：

- [StateBase.h](https://github.com/dai278/StealthAction_Git/blob/main/Source/StealthAction/Public/State/StateBase.h)
- [StateMachineComponent.cpp](https://github.com/dai278/StealthAction_Git/blob/main/Source/StealthAction/Private/State/StateMachineComponent.cpp)（`UStateMachine`）
- [PlayerStateMachine.cpp](https://github.com/dai278/StealthAction_Git/blob/main/Source/StealthAction/Private/State/Player/PlayerStateMachine.cpp)
- [PlayerStateBase.h](https://github.com/dai278/StealthAction_Git/blob/main/Source/StealthAction/Public/State/Player/PlayerStateBase.h)
- [PlayerShadowState.cpp](https://github.com/dai278/StealthAction_Git/blob/main/Source/StealthAction/Private/State/Player/PlayerShadowState.cpp)

新しい敵向けのステートは、旧敵クラスとは独立した構成で実装しています。既存の敵すべてを置き換えたものではありません。

### 3. 状態ごとのカメラ設定とアニメーションの連携

移動速度を上げすぎずに疾走感を出すため、カメラの距離・視野角・オフセットや追従の設定を調整しました。
カメラに必要な値は構造体にまとめ、各ステートの設定から適用します。

また、AnimNotifyなどのアニメーション側のタイミングとカメラ演出を連携し、攻撃やギミックに合わせて演出用カメラへ切り替えられる構成にしました。
カメラ情報を提供する側と、その情報を使って切り替える側を分けています。

主な実装：

- [CameraInfo.h](https://github.com/dai278/StealthAction_Git/blob/main/Source/StealthAction/Public/Camera/CameraInfo.h)
- [CameraFocusProvider.h](https://github.com/dai278/StealthAction_Git/blob/main/Source/StealthAction/Public/Camera/CameraFocusProvider.h)
- [CameraFocusDirectorComponent.cpp](https://github.com/dai278/StealthAction_Git/blob/main/Source/StealthAction/Private/Camera/CameraFocusDirectorComponent.cpp)

### 4. 音の発生と敵の反応を分ける通知基盤

ダッシュやギミックによる大きな行動に対して、敵に気付かれるリスクを持たせるため、音量と位置をゲーム内の情報として通知する仕組みを実装しました。
これは音声の再生そのものとは別の、敵の認知に使う情報です。

```text
プレイヤー・ギミック
  └ 音量・位置を渡す
      └ NoiseManager
          └ 登録された有効なNoiseListenerComponentへ通知
              └ 登録したコールバックを呼び出す
                  └ 敵側で反応を決定
```

`UNoiseManager`が音情報を受け付け、登録されている有効かつアクティブなリスナーの`OnNoiseHeard`を呼び出します。
リスナーは登録されたコールバックへ音量と位置を渡します。
音を発生させる側に敵の反応処理を持たせず、通知する側と反応する側の役割を分けました。

主な実装：

- [NoiseManager.cpp](https://github.com/dai278/StealthAction_Git/blob/main/Source/StealthAction/Private/Noise/NoiseManager.cpp)
- [NoiseListenerComponent.cpp](https://github.com/dai278/StealthAction_Git/blob/main/Source/StealthAction/Private/Noise/NoiseListenerComponent.cpp)

## 操作方法

| 操作 | キーボード・マウス | XInputコントローラー |
| --- | --- | --- |
| 移動 | W / A / S / D | 左スティック |
| カメラ操作 | マウス、矢印キー（マウス推奨） | 右スティック |
| 影潜り・解除 | F | LT |
| ダッシュ | Shift | RT |
| しゃがみ | Ctrl | X |
| 攻撃・スニークキル | マウス左クリック | RB |
| インタラクト | E | A |

攻撃は状況に応じてスニークキルと通常攻撃を選択します。通常攻撃を使用する構成では、`PlayerNormalAttackState`のBP登録とモンタージュなどの設定が必要です。

## ソースコードの読み方

影判定、ステート管理、音情報の通知の順に読むと、担当した主要な仕組みを確認できます。
各機能のhでインターフェースと設定項目を確認し、同名のcppで処理の流れを確認してください。

設定や構成の詳細：

- [プレイヤーステートとBPの設定](https://github.com/dai278/StealthAction_Git/blob/main/Docs/PlayerStateMachine.md)
- [新しい敵クラスの構成とBP・BTの設定](https://github.com/dai278/StealthAction_Git/blob/main/Docs/ModularEnemySetup.md)
- [ゲームオーバー・クリア時のフェード設定](https://github.com/dai278/StealthAction_Git/blob/main/Docs/LevelFadeTransition.md)

`Tests`には、実ソースから関数を抜き出し、UE型の代用品で処理を確認する簡易C++テストを収録しています。
これらはUEの描画・物理・UHTや、パッケージ版の動作確認を代替するものではありません。

## プロジェクトを開く場合

1. Unreal Engine 5.7と、対応するVisual StudioのC++開発環境・Windows SDKを用意します。
2. Git LFSを使用して、Contentを含むプロジェクトを取得します。
3. `StealthAction.uproject`からプロジェクトファイルを生成し、エディタ用ターゲットをビルドします。
4. UEで各BP・マップを開き、必要なアセット設定を確認します。

企業提出用の「担当ソースのみ」「全ソース」の閲覧資料は、読みやすいように機能別へ再配置したものです。
閲覧資料だけではUEプロジェクトをビルドできないため、実行する場合は元のプロジェクト構成とアセットが必要です。

---

最終更新：2026年10月27日
