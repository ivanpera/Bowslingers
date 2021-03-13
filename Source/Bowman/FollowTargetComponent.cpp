// Fill out your copyright notice in the Description page of Project Settings.


#include "FollowTargetComponent.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values for this component's properties
UFollowTargetComponent::UFollowTargetComponent() : Offset(FVector(.0f, .0f, .0f)),
													BlendingTime(0),
                                                    SnapDistanceThreshold(.5f),
													TargetReached(false),
                                                    RotationAxes({
                                                        EOffsetAxisSpace::WORLD,
                                                        EOffsetAxisSpace::WORLD,
                                                        EOffsetAxisSpace::WORLD
                                                    })
{
	PrimaryComponentTick.bCanEverTick = true;
}

FVector UFollowTargetComponent::GetAxisRotationDirection(EOffsetAxisSpace RotMode,
                                                         FVector const& DefaultValue,
                                                         FVector const& CurrValue,
                                                         FVector const& StartingValue) 
{
    switch (RotMode)
    {
    case EOffsetAxisSpace::STARTING:
        return StartingValue;
        break;
    case EOffsetAxisSpace::CURRENT:
        return CurrValue;
        break;
    case EOffsetAxisSpace::STARTING_INVERTED:
        return -StartingValue;
        break;
    case EOffsetAxisSpace::CURRENT_INVERTED:
        return -CurrValue;
        break;
    default:
        return DefaultValue;
        break;
    }
}

// Called every frame
void UFollowTargetComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (!Target || !Target->IsValidLowLevel())
    {
        return;
    }
    FVector const Fwd   = GetAxisRotationDirection(RotationAxes.X, FVector(1, 0, 0), Target->GetActorForwardVector(), TargetStartingForward);
    FVector const Right = GetAxisRotationDirection(RotationAxes.Y, FVector(0, 1, 0), Target->GetActorRightVector(), TargetStartingRight);
    FVector const Up    = GetAxisRotationDirection(RotationAxes.Z, FVector(0, 0, 1), Target->GetActorUpVector(), TargetStartingUp);

    FVector const _Offset = Fwd * Offset.X  +
        Right * Offset.Y +
        Up * Offset.Z;


    FVector const TargetLocation = Target->GetTargetLocation() + _Offset;
    Translate(TargetLocation);

    float DistanceToTarget = (GetOwner()->GetActorLocation() - TargetLocation).Size();

    if (DistanceToTarget < SnapDistanceThreshold && !TargetReached)
    {
        InvokeSnapEvents();
    }
    TargetReached = DistanceToTarget < SnapDistanceThreshold;
}

AActor* UFollowTargetComponent::_SetTarget(AActor* NewTarget) {
    if (NewTarget && NewTarget->IsValidLowLevel())
    {
        this->Target = NewTarget;
        TargetReached = false;
        TargetStartingForward = Target->GetActorForwardVector();
        TargetStartingRight   = Target->GetActorRightVector();
        TargetStartingUp      = Target->GetActorUpVector();
        if (OnSetTarget.IsBound())
        {
            OnSetTarget.Broadcast();
        }
    }
    return Target;
}

void UFollowTargetComponent::SetTarget(AActor* NewTarget)
{
    _SetTarget(NewTarget);
}

void UFollowTargetComponent::SetTargetAndTime(AActor* NewTarget, float NewBlendTime)
{
    if (_SetTarget(NewTarget))
    {
        BlendingTime = NewBlendTime;
    }
}

void UFollowTargetComponent::InvokeSnapEvents()
{
    if (OnSnap.IsBound())
    {
        OnSnap.Broadcast();
    }

    if (OnSnapSingle.IsBound())
    {
        OnSnapSingle.Broadcast();
        OnSnapSingle.Clear();
    }
}

void UFollowTargetComponent::Translate(FVector const& TargetLocation)
{
    FVector const& OwnerLocation = GetOwner()->GetActorLocation();
    float const Alpha = BlendingTime > 0.0f ? (1.0f / BlendingTime) * GetWorld()->GetDeltaSeconds() : 1.0f;
    FVector const NewPosition = FMath::Lerp(OwnerLocation, TargetLocation, Alpha);

    FRotator Rot = UKismetMathLibrary::FindLookAtRotation(TargetLocation, Target->GetActorLocation());

    //FMatrix const LookAt = FLookAtMatrix(NewPosition, Target->GetActorLocation(), {0, 1, 0});
    GetOwner()->SetActorLocationAndRotation(NewPosition, Rot.Quaternion()/*, LookAt.ToQuat()*/);
}