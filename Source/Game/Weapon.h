#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "Weapon.generated.h"

class APly;

UCLASS()
class GAME_API AWeapon : public AItem
{
	GENERATED_BODY()

public:	
	virtual void Primary(APly* Player);

	virtual void Secondary(APly* Player);

	virtual void Reload(APly* Player);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
		bool bIsADS;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
		int32 Mag;
};
