# ゲームオーバー・クリア時のフェード

ゲームオーバーとクリアの両方で、画面を暗くする→暗転したままレベル移動→移動先で明るく戻す処理を使用します。UMGのメニューも含めて画面を覆うため、CameraManagerのフェードではなくViewport最前面のSlateウィジェットを使用しています。

## ゲームオーバー

BP_PlayerDeadStateのクラスデフォルトで、Death → Fadeを調整します。

| 設定 | 既定値 | 内容 |
|---|---|---|
| Fade Out Duration | 0.5秒 | 死亡後に暗くなる時間 |
| Black Hold Duration | 0.1秒 | 完全に暗くなってから移動するまで |
| Fade In Duration | 0.5秒 | GameOverのロード完了後に明るく戻す時間 |
| Fade Color | 黒 | 画面を覆う色 |

移動先は従来のGame Over Level、移動するかはOpen Game Over Levelで指定します。On EnterをBPで上書きしている場合は「親関数を呼び出す」を接続してください。

## ゲームクリア

GoalActorを親とする使用中のゴールBP、またはレベルに配置したゴールのClear → Fadeで設定します。

- Clear Level：既定GameClear。
- Fade Out Duration / Black Hold Duration / Fade In Duration / Fade Color：ゲームオーバーと同じ意味・既定値です。

プレイヤーがゴールへ接触すると、従来の即時OpenLevelの代わりに共通フェードを開始します。ゲームオーバー／クリア用に新たなウィジェットBPを作る必要はありません。

## 共通処理

LevelTransitionSubsystemはGameInstanceSubsystemなので自動生成されます。プロジェクト設定への登録は不要です。レベルをまたいで暗転用ウィジェットを保持し、ロード完了後にフェードインします。フェード終了時とSubsystem終了時はウィジェットと通知を片付けます。ロード失敗時も暗いまま残さないよう除去します。

遷移中の追加要求は無視して最初の移動先を維持します。暗転完了前の重複Overlapなどで何度もOpenLevelしません。フェードはポーズやゲーム内時間倍率に左右されない実時間で進みます。音量のフェードやゲームのポーズは追加していません。Viewportのない環境では、死亡／ゴール側が従来の即時移動へフォールバックします。

ほかのBPから使う場合は「Get Game Instance Subsystem」でLevelTransitionSubsystemを取得し、Open Level With Fadeへレベル名・3種類の時間・色を渡せます。同じイベントで既存のOpen Levelも呼ぶとフェードを待たずに移動するため、呼び出しを共通関数へ揃えてください。移動先のレベルはパッケージング設定にも含めてください。

## 確認手順

1. エディタを閉じてC++をビルドします。
2. プレイヤーを死亡させ、完全に暗くなってからGameOverへ移動することを確認します。
3. GameOverのUIも黒い画面から徐々に見えることを確認します。
4. 同じことをGoalActorからのGameClearへの移動でも確認します。
5. フェード時間を0秒にしても移動でき、画面が黒いまま残らないことを確認します。
6. レベル名を間違えた場合はログを確認し、フェードの覆いが残らないことを確認します。

`python Tests/LevelTransition/run_checks.py` は実際のフェード関数をUE／Slateの代用型でコンパイル・実行し、時間経過、ロード前後の順序、移動の重複防止、0秒、移動失敗の後片付けを検証します。UE本体の描画・実際のレベルロードはエディタで確認してください。

## 死亡モンタージュ

BP_PlayerDeadStateのDeath → Animation → Death Montageへ死亡モンタージュを指定します。Montage Play Rateは既定1、Wait For Death Montageは既定OFFです。既定では死亡モンタージュの再生開始と同時に暗転を始めます。ONなら再生速度を反映したモンタージュの長さだけ待ち、Death → Fade → Game Over Delay（既定0秒）の追加待ちを挟んでから暗転します。OFFなら再生と並行して、Game Over Delay後に暗転します。

AnimBPのポーズからOutput Poseまでの間に、死亡モンタージュと一致するSlotノードを接続してください。倒れた最後のポーズを保ちたい場合は、死亡モンタージュのEnable Auto Blend OutをOFFにします。この場合も遷移は終了イベントではなく再生時間で進めるので、最後のポーズを保持したまま暗転できます。ループするセクションは使わないでください。

モンタージュ未設定や再生失敗の場合はアニメーション時間を待たず、追加待ち時間後にゲームオーバーへ進みます。Open Game Over LevelがOFFでも死亡モンタージュは再生します。BPでOn Enter／On Update／On Exitを上書きしている場合は親関数を呼び出してください。`python Tests/PlayerDeath/run_checks.py` で待ち時間、未設定／失敗時の遷移、移動の重複防止を検証できます。

以前のBPでWait For Death MontageをONに保存している場合は、Death → AnimationでOFFに変更し、Game Over Delayを0にしてください。C++の既定値変更だけでは、BPで保存した上書き値は変わりません。
