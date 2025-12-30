// Fill out your copyright notice in the Description page of Project Settings.


// Fill out your copyright notice in the Description page of Project Settings.


#include "Shadow/ShadowComponent.h"
#include "Shadow/ShadowManager.h"
#include "Light/ExtendedSpotLight.h"

#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"

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
	// Tickを有効にするなんか有効かしなかったからいっぱい試してる
	PrimaryComponentTick.bCanEverTick = true;
	SetComponentTickEnabled(true);
	SetAutoActivate(true);

	//メッシュコンポーネント生成
	m_pShadowMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("shadowMesh"));
	if (m_pShadowMesh) {
		//メッシュのコリジョンをOFF
		m_pShadowMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		//オーバーラップイベントは発生させない
		m_pShadowMesh->SetGenerateOverlapEvents(false);
		m_pShadowMesh->SetVisibility(true);
		m_pShadowMesh->SetHiddenInGame(false);
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
		m_pCollisionBox->SetCollisionProfileName(TEXT("Shadow"));
		//仮サイズ
		m_pCollisionBox->SetBoxExtent(FVector{ 40.f,40.f,40.f });
		//オーバラップイベントを発生させるか
		m_pCollisionBox->SetGenerateOverlapEvents(true);
	}
	
}

//------------------------------------------
//BeginPlayの後に呼ばれる関数
//------------------------------------------
void UShadowComponent::OnRegister()
{
	Super::OnRegister();	

	if (m_pShadowMesh && m_pCollisionBox)
	{
		m_pCollisionBox->SetCollisionProfileName(TEXT("Shadow"));
	}
	SetActive(true);

	//アタッチ
	if (m_pShadowMesh && m_pCollisionBox)
	{
		m_pShadowMesh->SetupAttachment(this);
		m_pCollisionBox->SetupAttachment(this);
	}

	// 回転を親の影響にしない（あなたの意図通り）
	SetUsingAbsoluteRotation(true);

}


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
}

//--------------------------------------
//更新処理
//--------------------------------------
void UShadowComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	//UE_LOG(LogTemp, Warning, TEXT("Shadow Tick!!"));
	UpdateTransform();
}


//--------------------------------------
//オーナーに対して角度などを合わせる
//--------------------------------------
void UShadowComponent::UpdateTransform()
{
	AActor* owner = GetOwner();
	if (!owner || !m_pLight)
	{
		return;
	}
	// オーナーの位置
	FVector origin = owner->GetActorLocation();
	// カプセルコンポーネントがあれば上端に調整
	if (UCapsuleComponent* capsule = owner->FindComponentByClass<UCapsuleComponent>())
	{
		const float halfHeight = capsule->GetScaledCapsuleHalfHeight();
		origin += FVector(0.f, 0.f, halfHeight); // 上端
	}

	// ライト位置
	const FVector lightPos = m_pLight->GetLightLocation();

	// 距離がライトの届く範囲外ならスキップ
	const float lightLength = m_pLight->GetLightLength();
	const float dist = FVector::Distance(origin, lightPos);
	if (lightLength > 0.f && dist > lightLength)
	{
		return;
	}

	// 光の進行方向
	FVector lightToObj = (origin - lightPos);
	if (lightToObj.IsNearlyZero())
	{
		return;
	}
	lightToObj.Normalize();

	// 影方向（オブジェクト→影が伸びる方向）
	FVector shadowDir = -lightToObj;

	// 地面（XY平面）へ投
	shadowDir.Z = 0.f;

	// 真上か真下だと方向が定まらない
	if (shadowDir.IsNearlyZero())
	{
		return;
	}
	//正規化
	shadowDir.Normalize();

	// RadiansをDegreesに変換
	const float yawDeg = FMath::RadiansToDegrees(FMath::Atan2(shadowDir.Y, shadowDir.X));

	// ワールド回転に変換
	SetWorldRotation(FRotator(0.f, yawDeg+90.f, 0.f));

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


