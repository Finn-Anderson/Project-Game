#include "Item.h"

AItem::AItem()
{
	PrimaryActorTick.bCanEverTick = true;

	MeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetMobility(EComponentMobility::Movable);
	MeshComponent->SetCollisionProfileName(TEXT("BlockAll"));
	MeshComponent->SetCustomDepthStencilValue(255);
	MeshComponent->WakeRigidBody();
	MeshComponent->SetSimulatePhysics(true);

	FCollisionResponseContainer collision_response;
	collision_response.SetResponse(ECC_Pawn, ECollisionResponse::ECR_Overlap);
	MeshComponent->SetCollisionResponseToChannels(collision_response);

	RootComponent = MeshComponent;
}

void AItem::PickedUp()
{
	SetActorHiddenInGame(true);

	SetActorRotation(FRotator(0.0f, 0.0f, 0.0f));
	ItemLocation = GetActorLocation();

	if (MeshComponent->DoesSocketExist(TEXT("ADSSocket"))) {
		ADSLocation = MeshComponent->GetSocketLocation(TEXT("ADSSocket"));
	}
}

void AItem::Focused() {
	MeshComponent->SetRenderCustomDepth(true);
}

void AItem::Unfocused() {
	MeshComponent->SetRenderCustomDepth(false);
}

void AWeapon::Primary(APly* Player)
{

}

void AWeapon::Secondary(APly* Player)
{

}

void AWeapon::Reload(APly* Player)
{

}