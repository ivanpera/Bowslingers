// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FollowTargetComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FVoidDelegate);

/*
* The rotation mode for an axes (ignored when TargetRelative is false):
*	-STARTING: look along the direction of the axes that the target had when assigned to this component
*	-CURRENT: look along the target's current axes direction
*	-WORLD: ignore the target axes direction (uses (1,0,0), (0,1,0), (0,0,1) vectors for the axes
*/
UENUM(BlueprintType)
enum class EOffsetAxisSpace : uint8 {
	STARTING UMETA(DisplayName = "STARTING"),
	CURRENT UMETA(DisplayName = "CURRENT"),
	STARTING_INVERTED UMETA(DisplayName = "STARTING_INVERTED"),
	CURRENT_INVERTED UMETA(DisplayName = "CURRENT_INVERTED"),
	WORLD UMETA(DisplayName = "WORLD")
};

USTRUCT(BlueprintType)
struct FOffsetAxis
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpawnConfig")
		EOffsetAxisSpace X;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpawnConfig")
		EOffsetAxisSpace Y;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpawnConfig")
		EOffsetAxisSpace Z;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BOWMAN_API UFollowTargetComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UFollowTargetComponent();

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	//Delegate that fires when the camera has reached the player
	UPROPERTY(BlueprintAssignable, Category = "Delegates")
		FVoidDelegate OnSnap;
	//Delegate that fires when the camera has reached the player, then removes all bound functions
	UPROPERTY(BlueprintAssignable, Category = "Delegates")
		FVoidDelegate OnSnapSingle;
	UPROPERTY(BlueprintAssignable, Category = "Delegates")
		FVoidDelegate OnSetTarget;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FollowBehaviour")
		FVector Offset;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FollowBehaviour")
		float BlendingTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FollowBehaviour")
		FOffsetAxis RotationAxes;

	//The minimum distance needed between the target location and current location 
	//to trigger the Snap delegates
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FollowBehaviour")
		float SnapDistanceThreshold;

	//Sets the new target
	UFUNCTION(BlueprintCallable)
		void SetTarget(AActor* NewTarget);

	//Sets the new target and, if the target is valid, the blending time
	UFUNCTION(BlueprintCallable)
		void SetTargetAndTime(AActor* Target, float NewBlendTime);

protected:
	void InvokeSnapEvents();
	virtual void Translate(FVector const& TargetLocation);
	FVector GetAxisRotationDirection(EOffsetAxisSpace RotMode, FVector const& DefaultValue, FVector const& CurrValue, FVector const& StartingValue);
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Target")
		AActor* Target;
private:
	bool TargetReached;

	AActor* _SetTarget(AActor* Target);
	FVector TargetStartingForward, TargetStartingRight, TargetStartingUp;
};
