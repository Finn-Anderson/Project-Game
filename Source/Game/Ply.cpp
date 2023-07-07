#include "Ply.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Math/UnrealMathUtility.h"
#include "Kismet/GameplayStatics.h"
#include "Delegates/Delegate.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "Blueprint/UserWidget.h"

#include "AI.h"
#include "Item.h"
#include "HealthComponent.h"
#include "Weapon.h"
#include "StaminaComponent.h"
#include "PlyMovementComponent.h"

APly::APly(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer.DoNotCreateDefaultSubobject(APly::MeshComponentName).SetDefaultSubobjectClass<UPlyMovementComponent>(APly::CharacterMovementComponentName))
{
    PrimaryActorTick.bCanEverTick = true;

    RootComponent = GetCapsuleComponent();

    PlyEyeHeight = 160.0f;
    PlyHeight = GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();

    PlyMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("PlyMesh"));
    PlyMesh->SetupAttachment(RootComponent);
    PlyMesh->SetCollisionObjectType(ECC_WorldStatic);
    PlyMesh->SetCollisionProfileName(TEXT("BlockAll"));
    PlyMesh->SetRelativeLocation(FVector(0.0f, 0.0f, -88.0f));
    PlyMesh->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
    PlyMesh->bCastDynamicShadow = true;
    PlyMesh->CastShadow = true;

    CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
    CameraComponent->SetRelativeLocation(FVector(0.0f, 30.0f, PlyEyeHeight));
    CameraComponent->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));
    CameraComponent->SetupAttachment(PlyMesh);
    CameraComponent->bUsePawnControlRotation = true;

    ItemMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GunMesh"));
    ItemMesh->SetupAttachment(CameraComponent);

    HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
    HealthComponent->MaxHealth = 100;
    HealthComponent->Health = HealthComponent->MaxHealth;

    MovementComponent = Cast<UPlyMovementComponent>(GetMovementComponent());

    InventoryLimit = 20;
}

void APly::BeginPlay()
{
    Super::BeginPlay();

    PController = UGameplayStatics::GetPlayerController(GetWorld(), 0);

    CameraManager = Cast<APlayerCameraManager>(UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0));

    PController->SetInputMode(FInputModeGameOnly());
}

void APly::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (Controller) {
        CrouchImpl(DeltaTime);

        if (EquipedItem && EquipedItem->bIsADS) {
            ItemMesh->SetRelativeLocation(FMath::VInterpTo(ItemMesh->GetRelativeLocation(), FVector(EquipedItem->ItemLocation.X - EquipedItem->ADSLocation.X, EquipedItem->ItemLocation.Y - EquipedItem->ADSLocation.Y, EquipedItem->ItemLocation.Z - EquipedItem->ADSLocation.Z), DeltaTime, 10.0f));
        }
        else {
            FVector camLoc;
            FRotator camRot;

            Controller->GetPlayerViewPoint(camLoc, camRot);

            FVector velocity = GetVelocity();
            velocity = camRot.UnrotateVector(velocity) * 0.005;

            ItemMesh->SetRelativeLocation(FMath::VInterpTo(ItemMesh->GetRelativeLocation(), FVector(30.0f, 10.0f, -10.0f) - velocity, DeltaTime, 10.0f));
        }
    }

    if (HealthComponent->Health > 0) {
        FVector camLoc;
        FRotator camRot;

        Controller->GetPlayerViewPoint(camLoc, camRot);

        const FVector startTrace = camLoc;
        const FVector direction = camRot.Vector();
        const FVector endTrace = startTrace + direction * 200;

        FCollisionQueryParams queryParams;
        queryParams.AddIgnoredActor(this);
        queryParams.AddIgnoredActor(EquipedItem);

        FHitResult hit(ForceInit);

        if (GetWorld()->LineTraceSingleByChannel(hit, startTrace, endTrace, ECC_Visibility, queryParams)) {
            AActor* hoverActor = hit.GetActor();

            if (HighlightItem) {
                HighlightItem->Unfocused();
                HighlightItem = nullptr;
            }

            if (hoverActor->IsA<AItem>()) {
                HighlightItem = Cast<AItem>(hoverActor);
                HighlightItem->Focused();
            }
        }
        else {
            if (HighlightItem) {
                HighlightItem->Unfocused();
                HighlightItem = nullptr;
            }
        }
    }
}

void APly::Use()
{
    AItem* item = Cast<AItem>(HighlightItem);

    if (!EquipedItem) {
        EquipedItem = item;
    }

    if (Inventory.Num() != InventoryLimit) {
        Inventory.Emplace(item);
    }
    else {
        Drop();
    }

    item->PickedUp();

    ItemMesh->SetSkeletalMeshAsset(item->MeshComponent->SkeletalMesh);
}

void APly::Slot(int SlotNum)
{
    if (Inventory.IsValidIndex(SlotNum)) {
        EquipedItem = Inventory[SlotNum];
    }
    else {
        // Equip fists.
    }
}

void APly::Drop()
{
    if (EquipedItem) {
        EquipedItem->bIsADS = false;

        ItemMesh->SetSkeletalMesh(0);

        FVector camLoc;
        FRotator camRot;

        Controller->GetPlayerViewPoint(camLoc, camRot);

        GetWorld()->SpawnActor<AItem>(EquipedItem->GetClass(), camLoc + camRot.Vector() * 50, camRot);
        EquipedItem->Destroy();

        EquipedItem->MeshComponent->SetAllPhysicsLinearVelocity(300 * camRot.Vector(), false);

        Inventory.RemoveSingle(EquipedItem);
        EquipedItem = NULL;
    }
}

void APly::Death(FVector DeathInstigatorLocation, FName Bone) {
    for (int i = 0; i < Inventory.Num();) {
        EquipedItem = Inventory[i];
        Drop();
    }

    DisableInput(PController);

    GetCapsuleComponent()->SetCollisionProfileName(TEXT("OverlapOnlyPawn"));

    PlyMesh->SetSimulatePhysics(true);

    const FVector direction = GetActorLocation() + DeathInstigatorLocation;

    PlyMesh->SetPhysicsLinearVelocity(direction, false, Bone);
}

void APly::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    // Set up "axis" bindings.
    PlayerInputComponent->BindAxis("MoveForward", this, &APly::MoveForward);
    PlayerInputComponent->BindAxis("MoveRight", this, &APly::MoveRight);

    PlayerInputComponent->BindAxis("Turn", this, &APly::Turn);
    PlayerInputComponent->BindAxis("LookUp", this, &APly::LookUp);

    // Set up "action" bindings
    PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &APly::StartJump);
    PlayerInputComponent->BindAction("Jump", IE_Released, this, &APly::StopJump);

    PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &APly::StartCrouch);
    PlayerInputComponent->BindAction("Crouch", IE_Released, this, &APly::StopCrouch);

    PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &APly::Sprint);
    PlayerInputComponent->BindAction("Sprint", IE_Released, this, &APly::StopSprinting);

    PlayerInputComponent->BindAction("Primary", IE_Pressed, this, &APly::Primary);

    PlayerInputComponent->BindAction("Secondary", IE_Pressed, this, &APly::Secondary);

    PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &APly::Reload);

    // Inventory
    DECLARE_DELEGATE_OneParam(FSlotNumDelegate, const int);

    PlayerInputComponent->BindAction<FSlotNumDelegate>("Slot1", IE_Pressed, this, &APly::Slot, 0);
    PlayerInputComponent->BindAction<FSlotNumDelegate>("Slot2", IE_Pressed, this, &APly::Slot, 1);
    PlayerInputComponent->BindAction<FSlotNumDelegate>("Slot3", IE_Pressed, this, &APly::Slot, 2);

    PlayerInputComponent->BindAction("Drop", IE_Pressed, this, &APly::Drop);

    PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &APly::Use);
}

void APly::Primary()
{
    if (EquipedItem) {
        EquipedItem->Primary(this);
    }
}

void APly::Secondary() {
    if (EquipedItem) {
        EquipedItem->Secondary(this);
    }
}

void APly::Reload() {
    if (EquipedItem) {
        EquipedItem->Reload(this);
    }
}

void APly::Turn(float Value)
{
    MovementComponent->Turn(Value);
}

void APly::LookUp(float Value)
{
    MovementComponent->LookUp(Value);
}

void APly::MoveForward(float Value)
{
    MovementComponent->MoveForward(Value);
}

void APly::MoveRight(float Value)
{
    MovementComponent->MoveRight(Value);
}

void APly::StartJump()
{
    bPressedJump = true;
}

void APly::StopJump()
{
    bPressedJump = false;
}

void APly::StartCrouch()
{
    MovementComponent->StartCrouch();
}

void APly::StopCrouch()
{
    MovementComponent->StopCrouch();
}

void APly::CrouchImpl(float DeltaTime)
{
    MovementComponent->CrouchImpl(DeltaTime, PlyEyeHeight, PlyHeight);
}

void APly::Sprint()
{
    MovementComponent->Sprint();
}

void APly::StopSprinting()
{
    MovementComponent->StopSprinting();
}