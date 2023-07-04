#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"

class APly;

UCLASS()
class GAME_API AItem : public AActor
{
	GENERATED_BODY()

public:
	AItem();

public:
	void PickedUp();

	void Focused();

	void Unfocused();

	virtual void Primary(APly* Player);

	virtual void Secondary(APly* Player);

	virtual void Reload(APly* Player);

	UPROPERTY()
		FVector ADSLocation;

	UPROPERTY()
		FVector ItemLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
		bool bIsADS;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
		int32 Mag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
		class USkeletalMeshComponent* MeshComponent;
};