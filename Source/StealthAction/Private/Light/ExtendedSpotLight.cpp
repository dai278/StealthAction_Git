// Fill out your copyright notice in the Description page of Project Settings.


#include "Light/ExtendedSpotLight.h"
#include "Components/SpotLightComponent.h"

#include "Light/ExtendedSpotLightManager.h"

//------------------------------------------
// コンストラクタ
//------------------------------------------
AExtendedSpotLight::AExtendedSpotLight()
	: m_bCanEverTick(false)
	, m_lightIndex(-1)
	, m_pSpotLight(nullptr)
	, m_pMesh(nullptr)
	, m_isEnemy(false)
	, m_automaticRotateYawSpeed(30.f)
	, m_bAutomaticRotateYaw(false)
	,m_bAutomaticRotatePitch(false)
	, m_automaticRotatePitchSpeed(30.f)
	, m_bRotateTurn(true)
	, m_minTurnRotate(0.f)
	, m_maxTurnRotate(180.f)
	, m_turnDir(1)
	, m_bBlink(false)
	, m_blinkOffTime(2.f)
	, m_blinkOnTime(2.f)
	, m_canItBeTurned(false)
	, m_bIsOn(true)
	, m_blinkTimer(0.f)
	, m_bManualRotateing(false)
	, m_LightStatus(ELightStatus::None)
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
	//インデックスが登録されていなければ登録しない
	if(m_lightIndex==-1){ m_pSpotLight->SetCastShadows(false); }
	//影を出す
	else { m_pSpotLight->SetCastShadows(true); }

	//ライトがついていない設定になっていれば消す
	if (!m_bIsOn) { m_pSpotLight->SetVisibility(false); }
}


//------------------------------------------
// 更新処理
//------------------------------------------
void AExtendedSpotLight::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


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
		GetWorld(),      // ワールド
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

	switch (m_LightStatus)
	{
	case ELightStatus::AutoYawRotate:
		UpdateAutoYawRotate(DeltaTime);
		break;
	case ELightStatus::AutoPitchRotate:
		UpdateAutoPitchRotate(DeltaTime);
		break;
	case ELightStatus::ManualYawRotate:
		UpdateManualYawRotate(DeltaTime);
		break;
	case ELightStatus::ManualPitchRotat:
		UpdateManualPitchRotate(DeltaTime);
		break;
	case ELightStatus::Blink:
		UpdateBlink(DeltaTime);
		break;
	default:
		break;
	}
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
	if (!m_bIsOn) { return false; }

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
	FCollisionObjectQueryParams ObjParams;
	ObjParams.AddObjectTypesToQuery(ECC_WorldStatic);
	ObjParams.AddObjectTypesToQuery(ECC_WorldDynamic);
	// Shadow専用ObjectTypeは「入れない」

	FCollisionQueryParams Params(SCENE_QUERY_STAT(LightOcclusion), false);
	Params.AddIgnoredActor(this);

	/* 影が複数コンポーネントなら全部 AddIgnoredComponent */
	bool bHit = GetWorld()->LineTraceSingleByObjectType(
		Hit,
		lightPos,
		_pos,
		ObjParams,
		Params
	);

	if (bHit)
	{
		// 途中で遮られたらアウト（少しマージン）
		if (Hit.Distance < distance - 100.0f)
		{
			return false;
		}
	}

	return true;

}



//-----------------------------------------------------
//ライトを付ける処理
//-----------------------------------------------------
void AExtendedSpotLight::TurnOn()
{
	if (m_canItBeTurned) {
		m_pSpotLight->SetVisibility(true);
		m_bIsOn = true;
		m_blinkTimer = 0.f;
	}
}

//-----------------------------------------------------
//ライトを消す
//-----------------------------------------------------
void AExtendedSpotLight::TurnOff()
{
	if (m_canItBeTurned) {
		m_pSpotLight->SetVisibility(false);
		m_bIsOn = false;
		m_blinkTimer = 0.f;
	}
}

void AExtendedSpotLight::SwitchTurn()
{
	if (m_bIsOn)
	{
		TurnOff();
	}
	else
	{
		TurnOn();
	}
}


//------------------------------------------------------
//最大値最小値までの回転
//------------------------------------------------------
void AExtendedSpotLight::OnRotateTargetAngleStop()
{
	m_bManualRotateing = true;
	//現在の回転
	FRotator  currentRot = GetActorRotation();

	if (m_minTurnRotate == currentRot.Pitch||m_minTurnRotate==currentRot.Yaw)
	{
		m_turnDir = 1.f;
	}
	else if (m_maxTurnRotate == currentRot.Pitch || m_maxTurnRotate == currentRot.Yaw)
	{
		m_turnDir = -1.f;
	}
	else
	{
		m_turnDir = 1.f;
	}
}

//-----------------------------------------------------
//自動YAw回転の更新処理
//-----------------------------------------------------
void AExtendedSpotLight::UpdateAutoYawRotate(const float& _deltaTime)
{
	FRotator newRotator = GetActorRotation();
	
	//現在の回転量が最大値最小値の範囲内でなければ範囲内へ回転
	if (newRotator.Yaw< m_minTurnRotate )
	{
		m_turnDir = 1;
	}
	else if (newRotator.Yaw > m_maxTurnRotate)
	{
		m_turnDir = -1;
	}
	
	//回転
	newRotator.Yaw += m_automaticRotateYawSpeed * _deltaTime*m_turnDir;
	if (!m_bRotateTurn) {
		SetActorRotation(newRotator);
		return;
	}
	
	//最大値を超えたか
	bool isOver=false;
	//最大値を超えたら
	if (newRotator.Yaw > m_maxTurnRotate)
	{
		newRotator.Yaw = m_maxTurnRotate;
		isOver = true;
	}
	//最小値未満になったら
	else if (newRotator.Yaw < m_minTurnRotate)
	{
		newRotator.Yaw = m_minTurnRotate;
		isOver = true;
	}	
	//超えた際の処理
	if (isOver)
	{
			m_turnDir *= -1.f;
	}

	SetActorRotation(newRotator);
}

//-----------------------------------------------------
//自動Pitch回転の更新処理
//-----------------------------------------------------
void AExtendedSpotLight::UpdateAutoPitchRotate(const float& _deltaTime)
{

	FRotator newRotator = GetActorRotation();

	//回転
	newRotator.Pitch += m_automaticRotateYawSpeed * _deltaTime * m_turnDir;
	SetActorRotation(newRotator);

	//現在の回転量が最大値最小値の範囲内でなければ処理しない
	if (newRotator.Pitch < m_minTurnRotate)
	{
		m_turnDir = 1;
	}
	else if (newRotator.Pitch > m_maxTurnRotate)
	{
		m_turnDir = -1;
	}
	//回転
	newRotator.Pitch += m_automaticRotatePitchSpeed * _deltaTime * m_turnDir;
	//最大値を超えたか
	bool isOver = false;
	//最大値を超えたら
	if (newRotator.Pitch > m_maxTurnRotate)
	{
		newRotator.Pitch = m_maxTurnRotate;
		isOver = true;
	}
	//最小値未満になったら
	else if (newRotator.Pitch < m_minTurnRotate)
	{
		newRotator.Pitch = m_minTurnRotate;
		isOver = true;
	}

	//超えた際の処理
	if (isOver)
	{
			m_turnDir *= -1.f;
	}

	SetActorRotation(newRotator);
}

//-----------------------------------------------------
//手動回転
//-----------------------------------------------------
void AExtendedSpotLight::UpdateManualYawRotate(const float& _deltaTime)
{

	if (!m_bManualRotateing) { return; }
	FRotator newRotator = GetActorRotation();

	//現在の回転量が最大値最小値の範囲内でなければ範囲内へ回転
	if (newRotator.Yaw < m_minTurnRotate)
	{
		m_turnDir = 1;
	}
	else if (newRotator.Yaw > m_maxTurnRotate)
	{
		m_turnDir = -1;
	}

	//回転
	newRotator.Yaw += m_automaticRotateYawSpeed * _deltaTime * m_turnDir;
	//最大値を超えたか
	bool isOver = false;
	//最大値を超えたら
	if (newRotator.Yaw > m_maxTurnRotate)
	{
		newRotator.Yaw = m_maxTurnRotate;
		isOver = true;
	}
	//最小値未満になったら
	else if (newRotator.Yaw < m_minTurnRotate)
	{
		newRotator.Yaw = m_minTurnRotate;
		isOver = true;
	}
	//超えた際の処理
	if (isOver)
	{
		m_bManualRotateing = false;
	}

	SetActorRotation(newRotator);

}

//-----------------------------------------------------
//手動回転
//-----------------------------------------------------
void AExtendedSpotLight::UpdateManualPitchRotate(const float& _deltaTime)
{

	if (!m_bManualRotateing) { return; }
	FRotator newRotator = GetActorRotation();

	//回転
	newRotator.Pitch += m_automaticRotateYawSpeed * _deltaTime * m_turnDir;
	SetActorRotation(newRotator);

	//現在の回転量が最大値最小値の範囲内でなければ処理しない
	if (newRotator.Pitch < m_minTurnRotate)
	{
		m_turnDir = 1;
	}
	else if (newRotator.Pitch > m_maxTurnRotate)
	{
		m_turnDir = -1;
	}
	//回転
	newRotator.Pitch += m_automaticRotatePitchSpeed * _deltaTime * m_turnDir;
	//最大値を超えたか
	bool isOver = false;
	//最大値を超えたら
	if (newRotator.Pitch > m_maxTurnRotate)
	{
		newRotator.Pitch = m_maxTurnRotate;
		isOver = true;
	}
	//最小値未満になったら
	else if (newRotator.Pitch < m_minTurnRotate)
	{
		newRotator.Pitch = m_minTurnRotate;
		isOver = true;
	}

	//超えた際の処理
	if (isOver)
	{
		m_bManualRotateing = false;
	}

	SetActorRotation(newRotator);
}



//----------------------------------------------------
//点滅
//----------------------------------------------------
void AExtendedSpotLight::UpdateBlink(const float& _deltaTime)
{
	if (!m_bBlink) { return; }

	//タイマー加算
	m_blinkTimer += _deltaTime;

	//ついていて時間を超えていれば
	if (m_bIsOn && m_blinkTimer > m_blinkOnTime) {
		//ライトを消す
		TurnOff();
	}
	else if (!m_bIsOn && m_blinkTimer > m_blinkOffTime)
	{
		//ライトを付ける
		TurnOn();
	}
}

