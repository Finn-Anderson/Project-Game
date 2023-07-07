#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "Weapon.generated.h"

UCLASS()
class GAME_API AWeapon : public AItem
{
	GENERATED_BODY()

public:	
	virtual void Primary(APly* Player) override;

	virtual void Secondary(APly* Player) override;

	virtual void Reload(APly* Player) override;
};
