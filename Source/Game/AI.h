#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AI.generated.h"

UCLASS()
class GAME_API AAI : public ACharacter
{
	GENERATED_BODY()

public:
	AAI();

protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
		class UStaticMeshComponent* AIMesh;

	UPROPERTY(VisibleAnywhere)
		class USphereComponent* InRangeComponent;

	class APly* Player;

	class AAIController* AIController;

	virtual void Tick(float DeltaTime) override;

public:
	// Health
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
		class UHealthComponent* HealthComponent;

	UPROPERTY()
		FTimerHandle ClearTimer;

	void Death(FVector DeathInstigatorLocation, FName Bone);

	void ClearRagdoll();

public:
	// Damage
	UPROPERTY()
		FTimerHandle DamageTimer;

	void Damage(APly* player);

	UFUNCTION()
		void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
