#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Ply.generated.h"

UCLASS()
class GAME_API APly : public ACharacter
{
	GENERATED_BODY()

public:
	APly(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
		class USkeletalMeshComponent* PlyMesh;

	UPROPERTY(VisibleAnywhere)
		class UCameraComponent* CameraComponent;

	UPROPERTY()
		class APlayerCameraManager* CameraManager;

	UPROPERTY()
		class APlayerController* PController;

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


public:
	// Interact
	UPROPERTY()
		class AItem* HighlightItem;

	void Use();


public:
	// Health
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
		class UHealthComponent* HealthComponent;

	void Death(FVector DeathInstigatorLocation, FName Bone);


public:
	// Inventory
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		class USkeletalMeshComponent* ItemMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
		TArray<class AItem*> Inventory;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
		int32 InventoryLimit;

	UPROPERTY()
		class AItem* EquipedItem;

	void Slot(int SlotNum);

	void Drop();


public:
	// Actions
	void Primary();

	void Secondary();

	void Reload();


public:
	//Movement
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement")
		class UPlyMovementComponent* MovementComponent;

	void Turn(float Value);

	void LookUp(float Value);

	void MoveForward(float Value);

	void MoveRight(float Value);

	void StartJump();

	void StopJump();

	void StartCrouch();

	void StopCrouch();

	void CrouchImpl(float DeltaTime);

	void Sprint();

	void StopSprinting();

protected:
	float PlyEyeHeight;

	float PlyHeight;
};