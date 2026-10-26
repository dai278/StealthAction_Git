//’S“–“úîà
//-------------------------------------
//@“§‰ßˆ—A‰e‚ªo‚¹‚éActor
//------------------------------------


#include "StageObject/SeeingThroughShadowActor.h"
#include "Components/StaticMeshComponent.h"
#include "Shadow/ShadowComponent.h"

const float ASeeingThroughShadowActor::MinOpacity = 0.f;
const float ASeeingThroughShadowActor::MaxOpacity = 1.f;

//----------------------------------------------
// ƒRƒ“ƒXƒgƒ‰ƒNƒ^
//----------------------------------------------
ASeeingThroughShadowActor::ASeeingThroughShadowActor()
    :m_bSeeingThrough(false)
    , m_pMesh(nullptr)
    , m_pShadow(nullptr)
    , m_timeCnt(0.f)
    , m_seeingThroughtime(0.5f)
{
    //–ˆƒtƒŒ[ƒ€Tick‚ğŒÄ‚Ô‚©
    PrimaryActorTick.bCanEverTick = true;

    // ƒƒbƒVƒ…ƒIƒuƒWƒFƒNƒg¶¬
    m_pMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SeeingThroughMesh"));
    if (m_pMesh) {
        SetRootComponent(m_pMesh);
    }
    //‰eƒIƒuƒWƒFƒNƒg‚Ì¶¬
    m_pShadow = CreateDefaultSubobject<UShadowComponent>(TEXT("Shadow"));
    if (m_pShadow) {
    // m_pShadow->AttachToComponent(this->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
		m_pShadow->SetupAttachment(m_pMesh);
        m_pShadow->SetRelativeLocation(FVector::ZeroVector);
        m_pShadow->SetRelativeRotation(FRotator::ZeroRotator);
    }
}

//-----------------------------------------------
// ƒQ[ƒ€ƒXƒ^[ƒg‚Ü‚½‚Í¶¬‚ÉŒÄ‚Î‚ê‚éˆ—
//-----------------------------------------------
void ASeeingThroughShadowActor::BeginPlay()
{
    Super::BeginPlay();

    //if (m_pShadow) {
    //    m_pShadow->AttachToComponent(this->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
    //    m_pShadow->SetRelativeLocation(FVector::ZeroVector);
    //    m_pShadow->SetRelativeRotation(FRotator::ZeroRotator);
   // }


    //UE•W€‰e‚ªo‚È‚¢‚æ‚¤‚É‚·‚é
    //m_pMesh->SetCastShadow(false);
}


//-------------------------------------------
// XVˆ—
//-------------------------------------------
void ASeeingThroughShadowActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
	if (!IsValid(m_pMesh)) { return; }

    
    if (m_timeCnt > 0) {
        // “§‰ßŠÔƒJƒEƒ“ƒg‚ğã‚°‚é
        m_timeCnt += DeltaTime;

        // •s“§–¾“x‚ÌXV
        // “§‰ß‚³‚¹‚é‚Í•s“§–¾“x‚ğMAX_OPACITY‚©‚çMIN_OPACITY‚É‚·‚é
        // •s“§–¾‚É‚³‚¹‚é‚Í•s“§–¾“x‚ğMIN_OPACITY‚©‚çMAX_OPACITY‚É‚·‚é
        float opacity = 0.0f;
        opacity = FMath::Lerp((m_bSeeingThrough ? MaxOpacity : MinOpacity),
            (m_bSeeingThrough ? MinOpacity : MaxOpacity),
            m_timeCnt * (1.0f / m_seeingThroughtime));

        // ƒ}ƒeƒŠƒAƒ‹‘¤‚ÌuOpacityvƒpƒ‰ƒ[ƒ^‚É”’l‚ğİ’è‚·‚é
        m_pMesh->SetScalarParameterValueOnMaterials(TEXT("Opacity"), opacity);

        // “§–¾`•s“§–¾‚ğ•Ï‚¦‚éŠÔ‚ªI‚í‚Á‚½‚ç
        if (m_timeCnt > m_seeingThroughtime)
        {
            // “§‰ßŠÔƒJƒEƒ“ƒg‚ğƒŠƒZƒbƒg
            m_timeCnt = 0.0f;
        }
    }
}

