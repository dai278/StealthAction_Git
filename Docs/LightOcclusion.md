# ライトの遮蔽物判定

`AExtendedSpotLight::IsHit`は照射距離・角度を確認した後、光源から足元へレイキャストします。遮蔽物として採用するのはStaticMeshComponentとSkeletalMeshComponent（それぞれの派生クラスを含む）だけです。

Box・カプセルなどに当たった場合は、そのコンポーネントを除外して同じ区間を再判定します。手前のカプセルを無視した結果、奥にあるメッシュを見落とさないようにしています。

影潜りの開始・継続には、登録済みライトのどれからも足元が照らされていないことを使用します。旧方式のShadowコリジョンに触れているだけでは、照らされている場所で影に入れません。

`Light → Occlusion → Occlusion End Tolerance`（既定1cm）は、足元の床や衝突計算の誤差を遮蔽物として扱わないための終点の許容幅です。終点よりこの距離以上手前にあるメッシュを遮蔽物にします。大きくすると足元近くの本来の遮蔽物も無視するため、通常は小さい値を使ってください。

## BP・アセットの設定

| 対象 | 設定 |
| --- | --- |
| 壁・箱などのStaticMesh | Collision EnabledをQueryを含む設定にし、Object TypeをWorldStaticまたはWorldDynamicにする |
| 敵のSkeletalMesh | Collision EnabledをQueryを含む設定にし、Object TypeをEnemyMeshにする。Physics Assetも設定する |
| Box・カプセル | 移動やインタラクト用の既存設定を維持できる。ライトの遮蔽物には採用しない |

Object Typeの検索対象は従来通りWorldStatic・WorldDynamic・EnemyMeshです。判定対象を決めるために専用のタグや新しいチャンネルを追加する必要はありません。

ライトBPの `Light → Occlusion → Trace Complex For Occlusion` は既定でOFFです。従来の簡易コリジョンを使いつつ、対象コンポーネントだけをメッシュに限定します。ONにするとStaticMeshのComplex Collisionを使用して形状に沿った判定を要求します。メッシュアセットのCollision Complexity設定によっては簡易形状が使われるため、必要に応じてアセット側も確認してください。BlockAllは衝突応答のプリセットで、複雑な衝突データを自動で用意する設定ではありません。以前の値をBPに保存している場合は、ライトBPでこの項目をOFFにして比較してください。

SkeletalMeshの通常のレイ判定にはPhysics Assetの形状が使われます。描画されるスキニング後の三角形と完全一致する判定ではありません。敵の形状に合わせてPhysics Assetを調整してください。

## UEでの確認

1. Boxだけを光源と足元の間に置き、影の遮蔽物として扱われないことを確認。
2. 同じ区間にStaticMeshを置き、影として扱われることを確認。
3. 敵のカプセルを通り、メッシュには当たらないレイでは照射されることを確認。
4. カプセルの奥で敵メッシュに当たる場合は、影として扱われることを確認。

描画のCast Shadow設定とは独立したゲーム用の判定です。上記の衝突設定と、画面上の影の両方を確認してください。
