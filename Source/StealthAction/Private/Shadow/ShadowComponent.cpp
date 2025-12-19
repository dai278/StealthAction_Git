// Fill out your copyright notice in the Description page of Project Settings.


// Fill out your copyright notice in the Description page of Project Settings.


#include "Shadow/ShadowComponent.h"
#include "Shadow/ShadowManager.h"
#include "Light/ExtendedSpotLight.h"

#include "Components/BoxComponent.h"

//---------------------------------------
// コンストラクタ
//---------------------------------------
UShadowComponent::UShadowComponent()
	:m_lightIndex(-1)
	, m_pLight(nullptr)
	, m_shadowShape(EShadowShape::box)
	, m_pShadowMesh(nullptr)
	, m_pCollisionBox(nullptr)
	, m_scaleType(EShadowScaleType::OriginalScale)
	, m_isUsingAbsoluteRotation(true)
{
	PrimaryComponentTick.bCanEverTick = false;
	//PrimaryComponentTick.bStartWithTickEnabled = true;
	//PrimaryComponentTick.bAllowTickOnDedicatedServer = true;
	//PrimaryComponentTick.bTickEvenWhenPaused = true;      // ← NEW!!!

	//PrimaryComponentTick.TickGroup = TG_PrePhysics;       // ← NEW!!!
	//bWantsInitializeComponent = true;                     // ← NEW!!!
	//bAutoActivate = true;                                 // ← NEW!!!

	SetComponentTickEnabled(true);
	SetAutoActivate(true);


	m_pShadowMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("shadowMesh"));
	if (m_pShadowMesh) {
		//メッシュのコリジョンをOFF
		m_pShadowMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		//オーバーラップイベントは発生させない
		m_pShadowMesh->SetGenerateOverlapEvents(false);
		m_pShadowMesh->SetVisibility(true);
		m_pShadowMesh->SetHiddenInGame(false);

		//m_pShadowMesh->SetRelativeLocation(FVector::ZeroVector);
		//m_pShadowMesh->SetRelativeRotation(FRotator::ZeroRotator);
	}

	//コリジョン
	m_pCollisionBox = CreateDefaultSubobject < UBoxComponent >(TEXT("m_pCollisionBox"));
	if (m_pCollisionBox) {

		//コリジョンが重なっているかの検知のみ
		m_pCollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		//コリジョンの全グループ(コリジョンチャンネル)を一度すべて無視
		m_pCollisionBox->SetCollisionResponseToAllChannels(ECR_Ignore);
		//そしてプレイヤーだけ追加
		m_pCollisionBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
		//仮サイズ
		m_pCollisionBox->SetBoxExtent(FVector{ 40.f,40.f,40.f });
		//
		//m_pCollisionBox->SetRelativeLocation(FVector::ZeroVector);
		//オーバラップイベントを発生させるか
		m_pCollisionBox->SetGenerateOverlapEvents(true);
	}


	/*if (m_pShadowMesh && m_pCollisionBox)
	{
		m_pShadowMesh->SetupAttachment(this);
		m_pCollisionBox->SetupAttachment(this);
	}*/
	
}

//------------------------------------------
//BeginPlayの後に呼ばれる関数
//------------------------------------------
void UShadowComponent::OnRegister()
{
	Super::OnRegister();

	//回転をアタッチするか？デフォはアタッチ
	if (!m_isUsingAbsoluteRotation) {
		SetUsingAbsoluteRotation(true);
	}

	if (m_pShadowMesh && m_pCollisionBox)
	{
		// Mesh と Collision は ShadowComponent に直接アタッチ
		/*m_pShadowMesh->AttachToComponent(this, FAttachmentTransformRules::KeepRelativeTransform);
		m_pCollisionBox->AttachToComponent(this, FAttachmentTransformRules::KeepRelativeTransform);*/

		m_pCollisionBox->SetCollisionProfileName(TEXT("Shadow"));
	}
	SetActive(true);

	//アタッチ
	if (m_pShadowMesh && m_pCollisionBox)
	{
		m_pShadowMesh->SetupAttachment(this);
		m_pCollisionBox->SetupAttachment(this);
	}
}


//void UShadowComponent::OnComponentCreated()
//{
//	Super::OnComponentCreated();
//
//	
//}


//--------------------------------------
// BeginPlay
//--------------------------------------
void UShadowComponent::BeginPlay()
{

	Super::BeginPlay();	

	//影マネージャーに登録
	GetWorld()->GetSubsystem<UShadowManager>()->AddShadowComponent(this);

	//オーナーのZ軸スケールを取得
	//Z軸はオーナーのスケールに依存しないようにする
	//オーナを取得
	AActor* owner = GetOwner();

	UStaticMeshComponent* ownerMesh = owner->FindComponentByClass<UStaticMeshComponent>();
	if (!owner) return;

	//メッシュコリジョンのスケール
	FVector newScale = GetComponentScale();
	//オーナーのスケール
	FVector ownerScale = owner->GetActorScale3D();
	 //ｚ軸は１固定
	 newScale.Z = 1.f;

	 //ローカルスケールを取得
	 FVector localScale = GetRelativeScale3D();

	 //XY軸のスケールを反映
	 switch (m_scaleType)
	 {
	 case EShadowScaleType::OriginalScale:
		 //オリジナルは何をしない
	 {
		 break;
	 }
	 case EShadowScaleType::OwnerScaleX:
		 //X軸のスケールを反映
		 //Y軸は１固定
	 {
		 newScale.Y = ownerScale.Z * 0.5f;
		 break;
	 }
	 case EShadowScaleType::OwnerScaleY:
	 {
		 //Y軸のスケールを反映
		 //X軸は１固定
		 newScale.X = ownerScale.Z*0.5f;
		 break;
	 }
	}
	 //ワールドサイズを補正
	 SetWorldScale3D(newScale);

	////コリジョンにスケールを反映
	//m_pCollisionBox->SetWorldScale3D(newScale);
	////メッシュにスケールを反映
	//m_pShadowMesh->SetWorldScale3D(newScale);
	//FVector newLocation = m_pCollisionBox->GetRelativeLocation();
	//FVector meshLocation = m_pShadowMesh->GetRelativeLocation();
	//newLocation.Z = meshLocation.Z;
	//m_pCollisionBox->SetRelativeLocation(newLocation);
	


//
//	// ===== サイズを Mesh の Bounds に合わせる ===== 	
//	float collisionSizeZ = 40.f;
//
//	// ① ローカルBounds取得（Min / Max）
//	FVector Min, Max;
//	m_pShadowMesh->GetLocalBounds(Min, Max);
//
//	// ② ローカル実サイズ（直径）
//	FVector LocalSize = Max - Min;
//
//	// ③ スケールを反映（重要！）
//	FVector WorldSize = LocalSize * owner->GetActorScale();
//
//	// ④ BoxExtent は半径（half-size）
//	FVector HalfExtent = WorldSize * 0.5f;
//
//	HalfExtent.Z = collisionSizeZ;
//	HalfExtent.Y = HalfExtent.Y / ownerScale.Y;
//
//	m_pCollisionBox->SetBoxExtent(HalfExtent);
//
//	
//	//メッシュのスケールを取得
//	FVector newSceal = m_pShadowMesh->GetComponentScale();
//	////メッシュのY軸の長さを高さに依存
//	newSceal = { newSceal.X,newSceal.Y/ownerScale.Y*ownerScale.Z,newSceal.Z};
//	//m_pShadowMesh->SetWorldScale3D(newSceal);
//	m_pCollisionBox->SetWorldScale3D(FVector{newSceal.X/ownerScale.X,newSceal.Y,1.f});
//
//
//
//	//メッシュの半径を取得
//	FVector extent = m_pShadowMesh->GetStaticMesh()->GetBounds().BoxExtent;
//	//FVector bounds = m_pCollisionBox();
//	//=============ここ要変更======================
//	
//	// ====== BoxCollision のスケール反映後の半径取得 ======
//	FVector BoxExtent = m_pCollisionBox->GetScaledBoxExtent();
//
//	m_pCollisionBox->SetWorldLocation(m_pShadowMesh->GetComponentLocation());
//// ====== Y軸に半径分オフセット ======
//	FVector NewLocation = m_pShadowMesh-> GetRelativeLocation();
//	//NewLocation.Y += BoxExtent.Y/2.f;
//	m_pCollisionBox->SetRelativeLocation(NewLocation);
//
//	//FVector componentLocation = GetRelativeLocation();
//	//componentLocation.Y -= BoxExtent.Y ;
//	//SetRelativeLocation(componentLocation);
//
//	//アクティブにする
//	SetActive(true);
//	m_pShadowMesh->SetActive(true);
//	m_pCollisionBox->SetActive(true);
//		if (!m_pShadowMesh || !m_pCollisionBox)
//			return;
//
//
//
//	
	

}

//--------------------------------------
//更新処理
//--------------------------------------
void UShadowComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	//UE_LOG(LogTemp, Warning, TEXT("Shadow Tick!!"));
	//UpdateTransform();
}


//--------------------------------------
//オーナーに対して位置などを合わせる
//--------------------------------------
void UShadowComponent::UpdateTransform()
{

	////オーナーの取得
	//AActor* owner = GetOwner();
	////nullチェック
	//if (!owner || !m_pLight) {
	//	return;
	//}

	//

	////オーナのメッシュ
	//UStaticMeshComponent* ownerMesh = owner->FindComponentByClass<UStaticMeshComponent>();
	//if (!ownerMesh) { return; }

	////ワールド空間の上でのサイズ位置を取得
	//const const FBoxSphereBounds worldBounds = ownerMesh->CalcBounds(ownerMesh->GetComponentTransform());	
	//const FVector worldCenter = worldBounds.Origin;//中心位置
	//const FVector worldExtent = worldBounds.BoxExtent;//半径
	//const float width = worldExtent.X*2.f;//幅
	//const float depth = worldExtent.Y*2.f;//奥行き
	//const float hight = worldExtent.Z*2.f;//高さ
	//const FVector  bottom = { worldCenter.X,worldCenter.Y,worldCenter.Z - worldExtent.Z };//地面の高さの中心
	//const FVector top = { worldCenter.X,worldCenter.Y,worldCenter.Z + worldExtent.Z };//頂点の中心


	////float width = extents.X * 2.0f;  // メッシュの横幅
	////float depth = extents.Y * 2.0f;  // メッシュの奥行
	////float height = extents.Z * 2.0f;// メッシュの高さ
	////FVector bottom = origin - FVector{ 0, 0, extents.Z };              //下
	////FVector top = origin + FVector(0, 0, extents.Z);                 //上

	//const FVector lightPos = m_pLight->GetLightLocation();           //ライトの位置
	//const FVector lightForward = m_pLight->GetLightForwardVector();  //ライトの向き
	//float outerConeAngleDeg = m_pLight->GetLightOuterAngle();        //ライトの外側角度
	//float lightLength = m_pLight->GetLightLength();					 //ライトの届く最大距離
	//float groundZ = 0.f;                                             //地面の高さ
	//FVector outShadowPos;                                            //影出力位置

	////ライト→オブジェクトへの向き
	//const FVector lightvecter = (top - lightPos);	
	////ライトの最大距離よりとおければ処理しない
	//if (lightLength < lightvecter.Length()) { return; }

	////正規化したベクトル
	//const FVector direction = lightvecter.GetSafeNormal();
	//
	////Z軸を消したベクトルを正規化
	//FVector shadowDir = FVector(direction.X, direction.Y, 0.f).GetSafeNormal();
	////Z成分がほぼ0ライトが水平だからかげが落ちない
	//if (direction.Z > 0)
	//{
	//	return;
	//}
	//else {
	//}

	//// 影の方向Yaw	
	////度数に変換
	//float yawDeg = FMath::RadiansToDegrees(FMath::Atan2(shadowDir.Y, shadowDir.X));


	////影メッシュのサイズ
	//FVector shadowMeshExtent = m_pShadowMesh->Bounds.BoxExtent;

	////影の長さ
	//float shadowLength = height / abs(direction.Z) * 1.5f;//少し短いから1.5倍で補正
	////引数2~3までの範囲に収める関数極端に長くなるなどを防ぐ
	//shadowLength = FMath::Clamp(shadowLength, 0.3f, height * 5.f);
	////スケールに変換
	//float shadowLengthScale = shadowLength / (height);

	////Z軸はオーナーのスケールと一致させたくないので
	//
	////スケール
	//FVector newScale = FVector{ shadowLengthScale ,1.f,m_collisionScaleZ };
	//m_pShadowMesh->SetRelativeScale3D(newScale);

	//m_pCollisionBox->SetRelativeScale3D(newScale);


	//// Pivot のローカル座標は常に (0,0,0)
	//FVector localPivot = FVector::ZeroVector;

	//// 世界座標に変換
	//FVector worldPivot = ownerMesh->GetComponentTransform().TransformPosition(localPivot);

	////描画開始地点（オーナーから見た
	//FVector startShadowPos = FVector{ origin.X, origin.Y, bottom.Z };

	////影の中心位置
	//outShadowPos = startShadowPos + shadowDir * (shadowLength * 0.5f);
	////ローカル座標に変換
	//FVector local = GetOwner()->GetRootComponent()
	//	->GetComponentTransform()
	//	.InverseTransformPosition(outShadowPos);


	////位置
	//SetRelativeLocation(local);
	////回転
	//SetRelativeRotation(FRotator(0, yawDeg, 0));


}


//----------------------------------------
//ライトをセット
//----------------------------------------
void UShadowComponent::SetLight(const AExtendedSpotLight* _pLight)
{
	m_pLight = _pLight;
}



//----------------------------------------
//ライトインデックスの取得
//----------------------------------------
int32 UShadowComponent::GetLightIndex()const
{
	return m_lightIndex;
}


