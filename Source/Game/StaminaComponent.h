#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StaminaComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GAME_API UStaminaComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UStaminaComponent();

public:
	UPROPERTY()
		int32 MaxStamina;

	UPROPERTY()
		int32 Stamina;

	void TakeStamina();

	void AddStamina();

	UFUNCTION(BlueprintPure, Category = "Stamina")
		float GetStamina();
};