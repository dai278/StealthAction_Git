// Fill out your copyright notice in the Description page of Project Settings.


#include "Light/ExtendedSpotLight.h"
#include "Components/SpotLightComponent.h"

#include "Light/ExtendedSpotLightManager.h"


//------------------------------------------
// コンストラクタ
//------------------------------------------
AExtendedSpotLight::AExtendedSpotLight()
	:m_bCanEverTick(false)
	, m_lightIndex(-1)
	, m_pSpotLight(nullptr)
{
	//毎フレーム更新処理を行うか.
	PrimaryActorTick.bCanEverTick = true;

	//スポットライト生成
	m_pSpotLight = CreateDefaultSubobject<USpotLightComponent>(TEXT("SpotLight"));

	//Rootを作成
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	//メッシュの生成
	m_pMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));

	//RootにSpotLightをアタッチ
	m_pSpotLight->SetupAttachment(RootComponent);

}

//------------------------------------------
// BeginPlay（ゲーム開始時）
//------------------------------------------
void AExtendedSpotLight::BeginPlay()
{
	Super::BeginPlay();

	//BP上で設定した毎フレーム更新するか
	//動かすライトは毎フレーム
	PrimaryActorTick.bCanEverTick = m_bCanEverTick;

	//マネージャーに登録
	GetWorld()->GetSubsystem<UExtendedSpotLightManager>()->AddLight(this);
	//デフォルとで影を出さない
	m_pSpotLight->SetCastShadows(false);

}


//------------------------------------------
// 更新処理
//------------------------------------------
void AExtendedSpotLight::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector newPos = GetActorLocation();
	newPos.X += 200.f * DeltaTime;
	
	SetActorLocation(newPos);

	// スポットライトの位置（ワールド座標）
	FVector Origin = m_pSpotLight->GetComponentLocation();

	// スポットライトの方向（ワールド座標系の前方ベクトル = 照射方向）
	FVector Direction = m_pSpotLight->GetForwardVector();

	// 照射距離（照射範囲の長さ）
	float Length = m_pSpotLight->AttenuationRadius;

	// 円錐の角度（OuterConeAngle）をラジアン変換
	float AngleWidth = FMath::DegreesToRadians(m_pSpotLight->OuterConeAngle);
	float AngleHeight = AngleWidth; // 円錐なので同じ角度を使用

	// 白色で描画
	FColor ConeColor = FColor::White;

	// DrawDebugConeの描画パラメータ
	DrawDebugCone(
		GetWorld(),      // ワールドコンテキスト
		Origin,          // 円錐の発生点
		Direction,       // 照射方向ベクトル
		Length,          // 円錐の長さ(高さ)
		AngleWidth,      // 円錐の水平半角（ラジアン）
		AngleHeight,     // 円錐の垂直半角（ラジアン）
		16,              // 分割数（滑らかさ）
		ConeColor,       // 色
		false,           // 永続描画ではない（毎フレーム描画で更新）
		-1.f,            // ライフタイム（-1＝1フレームのみ）
		0,               // 深度優先度
		2.f              // 線の太さ
	);


}

//------------------------------------------
//ライトインデックスの取得
//------------------------------------------
int32 AExtendedSpotLight::GetIndex()const
{
	return m_lightIndex;
}

//------------------------------------------
//ライトの位置取得
//------------------------------------------
FVector AExtendedSpotLight::GetLightLocation()const
{
	return m_pSpotLight->GetComponentLocation();
}

//-----------------------------------
//ライトの向き取得
//-----------------------------------
FVector AExtendedSpotLight::GetLightForwardVector()const
{
	return m_pSpotLight->GetForwardVector();
}

//---------照射範囲-----------

// --------------------------------
//円錐の内側角度
//---------------------------------
float AExtendedSpotLight::GetLightInnerAngle()const
{
	return m_pSpotLight->InnerConeAngle;
}

//---------------------------------
//円錐外側角度
//----------------------------------
float AExtendedSpotLight::GetLightOuterAngle()const
{
	return m_pSpotLight->OuterConeAngle;
}

//-----------------------------------
//光の届く距離
//-----------------------------------
float AExtendedSpotLight::GetLightLength()const
{
	return m_pSpotLight->AttenuationRadius;
}

//-----------------------------------
//ライトのON OFF設定
//----------------------------------
void AExtendedSpotLight::SetVisibility(const bool isVisibility)
{
	m_pSpotLight->SetVisibility(isVisibility);
}

//----------------------------------
//ライトが動くか?
//----------------------------------
bool AExtendedSpotLight::IsMove()const
{
	return m_bCanEverTick;
}


//------------------------------
//ライトとの衝突チェック
//------------------------------
bool AExtendedSpotLight::IsHit(const FVector& _pos)const
{

	FVector lightPos = m_pSpotLight->GetComponentLocation();
	FVector lightDir = m_pSpotLight->GetForwardVector();

	FVector ToPoint = (_pos - lightPos);
	//距離
	float distance = ToPoint.Size();

	// 距離チェック
	if (distance > m_pSpotLight->AttenuationRadius)
		return false;

	ToPoint.Normalize();

	// 角度チェック
	float CosAngle = FVector::DotProduct(lightDir, ToPoint);
	float CosOuterCone = FMath::Cos(FMath::DegreesToRadians(m_pSpotLight->OuterConeAngle));
	//角度に入っていなければ
	if (CosAngle < CosOuterCone) { return false; }

	// 遮蔽物チェック（影判定）
	FHitResult Hit;
	bool bHit = GetWorld()->LineTraceSingleByChannel(
		Hit,
		lightPos,
		_pos,
		ECC_Visibility
	);

	if (bHit && Hit.Location != _pos) { return false; }

	return true;

}