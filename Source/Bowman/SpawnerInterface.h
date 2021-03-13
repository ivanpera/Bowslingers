#pragma once

#include "CoreMinimal.h"

#include "SpawnerInterface.generated.h"

USTRUCT(BlueprintType)
struct FSpawnConfigItem
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpawnConfig")
        bool Bound;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpawnConfig")
        bool Randomize;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpawnConfig")
        float AxisBound;
};


USTRUCT(BlueprintType)
struct FSpawnConfig
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpawnConfig")
        TArray<FSpawnConfigItem> XYZConfig;
};

UINTERFACE(MinimalAPI, Blueprintable)
class USpawnerInterface : public UInterface
{
    GENERATED_BODY()
};

class ISpawnerInterface
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
        void SpawnActor(FSpawnConfig const& SpawnConfig, TSubclassOf<class AActor> ActorClass = nullptr);

};