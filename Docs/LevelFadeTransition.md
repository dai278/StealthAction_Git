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
