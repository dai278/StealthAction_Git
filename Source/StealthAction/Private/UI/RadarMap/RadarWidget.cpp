// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/RadarMap/RadarWidget.h"

#include "Kismet/GameplayStatics.h" 
#include "Enemy/Enemy_1.h" 
#include "StealthAction/PlayerCharacter/PlayerCharacter.h" 

#include "SlateCore.h" 

//=============================== 
//コンストラクタ 
//=============================== 
URadarWidget::URadarWidget(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    bHasScriptImplementedPaint = true;
}

//=============================== 
//NativeConstruct 
//=============================== 
void URadarWidget::NativeConstruct()
{
    Super::NativeConstruct();
    UE_LOG(LogTemp, Warning, TEXT("RadarWidget NativeConstruct"));
}

//===============================
//Player設定 
//=============================== 
void URadarWidget::SetOwnerPlayer(APlayerCharacter* Player)
{
    OwnerPlayer = Player;
}

//=============================== 
//レーダー描画処理 
//=============================== 
int32 URadarWidget::NativePaint(
    const FPaintArgs& Args,
    const FGeometry& AllottedGeometry,
    const FSlateRect& MyCullingRect,
    FSlateWindowElementList& OutDrawElements,
    int32 LayerId,
    const FWidgetStyle& InWidgetStyle,
    bool bParentEnabled
) const
{
    LayerId = Super::NativePaint(
        Args,
        AllottedGeometry,
        MyCullingRect,
        OutDrawElements,
        LayerId,
        InWidgetStyle,
        bParentEnabled
    );

    //=============================== 
    //プレイヤー取得 
    //===============================
    APawn* PlayerPawn = GetOwningPlayerPawn();
    if (!PlayerPawn) return LayerId;

    const FVector PlayerLocation = PlayerPawn->GetActorLocation();

    //===============================
    //敵表示
    //===============================
    const float PlayerYaw = PlayerPawn->GetActorRotation().Yaw;
    const FVector2D RadarCenter = AllottedGeometry.GetLocalSize() * 0.5f;

    TArray<AActor*> Enemies;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemy_1::StaticClass(), Enemies);

    for (AActor* Enemy : Enemies)
    {
        if (!Enemy) continue;

        FVector Delta = Enemy->GetActorLocation() - PlayerLocation;
        Delta.Z = 0.f;

        // プレイヤー向き基準に回転 
        FRotator Rot(0.f, -PlayerYaw + 270.f, 0.f);
        FVector Rotated = Rot.RotateVector(Delta);

        float Distance = Rotated.Size();
        if (Distance > RadarRange) continue;

        FVector2D RadarPos;
        RadarPos.X = (Rotated.Y / RadarRange) * RadarRadius;
        RadarPos.Y = (-Rotated.X / RadarRange) * RadarRadius;

        FVector2D DrawPos = RadarCenter + RadarPos;

        FSlateDrawElement::MakeBox(
            OutDrawElements,
            LayerId++,
            AllottedGeometry.ToPaintGeometry(DrawPos - FVector2D(4.f, 4.f), FVector2D(8.f, 8.f)),
            FCoreStyle::Get().GetBrush("WhiteBrush"),
            ESlateDrawEffect::None,
            FLinearColor(1, 0, 0, 1) // 赤
        );
    }

    //===============================
    // プレイヤー△（中央固定・移動方向に回転）
    //===============================

    // staticで前回角度を保持
    static float LastAngle = 0.f;

    FVector Velocity = PlayerPawn->GetVelocity();
    Velocity.Z = 0.f; // Z軸は無視

    float AngleDeg = LastAngle;
    if (!Velocity.IsNearlyZero())
    {
        // X前, Y右の座標系
        AngleDeg = FMath::RadiansToDegrees(FMath::Atan2(Velocity.Y, Velocity.X));
        // 北固定（上向き）に合わせるため -90度補正
        AngleDeg -= 90.f;
        LastAngle = AngleDeg; // 前回角度を更新
    }

    auto RotatePoint = [&](FVector2D Point, float Deg) -> FVector2D
        {
            float Rad = FMath::DegreesToRadians(Deg);
            float CosA = FMath::Cos(Rad);
            float SinA = FMath::Sin(Rad);
            FVector2D Dir = Point - RadarCenter;
            FVector2D Rotated;
            Rotated.X = Dir.X * CosA - Dir.Y * SinA;
            Rotated.Y = Dir.X * SinA + Dir.Y * CosA;
            return RadarCenter + Rotated;
        };

    const float Size = 12.f;
    FVector2D P0 = RadarCenter + FVector2D(0, -Size);
    FVector2D P1 = RadarCenter + FVector2D(-Size * 0.7f, Size);
    FVector2D P2 = RadarCenter + FVector2D(Size * 0.7f, Size);

    P0 = RotatePoint(P0, AngleDeg);
    P1 = RotatePoint(P1, AngleDeg);
    P2 = RotatePoint(P2, AngleDeg);

    TArray<FVector2D> Points;
    Points.Add(P0);
    Points.Add(P1);
    Points.Add(P2);
    Points.Add(P0); // 閉じる

    FSlateDrawElement::MakeLines(
        OutDrawElements,
        LayerId++,
        AllottedGeometry.ToPaintGeometry(),
        Points,
        ESlateDrawEffect::None,
        FLinearColor::Green,
        true,
        2.f
    );

    return LayerId;
}

