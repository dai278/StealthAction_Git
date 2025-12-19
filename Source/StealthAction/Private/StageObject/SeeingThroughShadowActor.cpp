//-------------------------------------
//　透過処理、影が出せるActor
//------------------------------------


#include "StageObject/SeeingThroughShadowActor.h"
#include "Components/StaticMeshComponent.h"
#include "Shadow/ShadowComponent.h"

const float ASeeingThroughShadowActor::MinOpacity = 0.f;
const float ASeeingThroughShadowActor::MaxOpacity = 1.f;

//----------------------------------------------
// コンストラクタ
//----------------------------------------------
ASeeingThroughShadowActor::ASeeingThroughShadowActor()
    :m_bSeeingThrough(false)
    , m_pMesh(nullptr)
    , m_pShadow(nullptr)
    , m_timeCnt(0.f)
    , m_seeingThroughtime(0.5f)
{
    //毎フレームTickを呼ぶか
    PrimaryActorTick.bCanEverTick = true;

    // メッシュオブジェクト生成
    m_pMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SeeingThroughMesh"));
    if (m_pMesh) {
        SetRootComponent(m_pMesh);
    }
    //影オブジェクトの生成
    m_pShadow = CreateDefaultSubobject<UShadowComponent>(TEXT("Shadow"));
    if (m_pShadow) {
    // m_pShadow->AttachToComponent(this->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
		m_pShadow->SetupAttachment(m_pMesh);
        m_pShadow->SetRelativeLocation(FVector::ZeroVector);
        m_pShadow->SetRelativeRotation(FRotator::ZeroRotator);
    }
}

//-----------------------------------------------
// ゲームスタートまたは生成時に呼ばれる処理
//-----------------------------------------------
void ASeeingThroughShadowActor::BeginPlay()
{
    Super::BeginPlay();

    //if (m_pShadow) {
    //    m_pShadow->AttachToComponent(this->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
    //    m_pShadow->SetRelativeLocation(FVector::ZeroVector);
    //    m_pShadow->SetRelativeRotation(FRotator::ZeroRotator);
   // }


    //UE標準影が出ないようにする
    //m_pMesh->SetCastShadow(false);
}


//-------------------------------------------
// 更新処理
//-------------------------------------------
void ASeeingThroughShadowActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    
    if (m_timeCnt > 0) {
        // 透過時間カウントを上げる
        m_timeCnt += DeltaTime;

        // 不透明度の更新
        // 透過させる時は不透明度をMAX_OPACITYからMIN_OPACITYにする
        // 不透明にさせる時は不透明度をMIN_OPACITYからMAX_OPACITYにする
        float opacity = 0.0f;
        opacity = FMath::Lerp((m_bSeeingThrough ? MaxOpacity : MinOpacity),
            (m_bSeeingThrough ? MinOpacity : MaxOpacity),
            m_timeCnt * (1.0f / m_seeingThroughtime));

        // マテリアル側の「Opacity」パラメータに数値を設定する
        m_pMesh->SetScalarParameterValueOnMaterials(TEXT("Opacity"), opacity);

        // 透明～不透明を変える時間が終わったら
        if (m_timeCnt > m_seeingThroughtime)
        {
            // 透過時間カウントをリセット
            m_timeCnt = 0.0f;
        }
    }
}

