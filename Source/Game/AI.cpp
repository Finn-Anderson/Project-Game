#include "AI.h"

#include "Kismet/GameplayStatics.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "AIController.h"
#include "HealthComponent.h"
#include "Ply.h"

AAI::AAI()
{
	PrimaryActorTick.bCanEverTick = true;

	FCollisionResponseContainer collision_response;
	collision_response.SetResponse(ECC_Visibility, ECollisionResponse::ECR_Ignore);
	collision_response.SetResponse(ECC_Pawn, ECollisionResponse::ECR_Overlap);
	GetCapsuleComponent()->SetCollisionResponseToChannels(collision_response);

	AIMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AIMesh"));
	AIMesh->SetupAttachment(RootComponent);
	AIMesh->SetCollisionProfileName(TEXT("BlockAll"));
	AIMesh->SetRelativeScale3D(FVector(0.6f, 0.6f, 0.6f));
	AIMesh->SetRelativeLocation(FVector(0.0f, 0.0f, -30.0f));
	AIMesh->bCastDynamicShadow = true;
	AIMesh->CastShadow = true;

	FCollisionResponseContainer mesh_collision_response;
	mesh_collision_response.SetResponse(ECC_Pawn, ECollisionResponse::ECR_Overlap);
	AIMesh->SetCollisionResponseToChannels(mesh_collision_response);

	GetCharacterMovement()->MaxWalkSpeed = 300.0f;

	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
	HealthComponent->MaxHealth = 100;
	HealthComponent->Health = HealthComponent->MaxHealth;

	InRangeComponent = CreateDefaultSubobject<USphereComponent>("CollisionSphere");
	InRangeComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	InRangeComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 50.0f));
	InRangeComponent->SetSphereRadius(125.0f);
	InRangeComponent->SetupAttachment(AIMesh);
}

void AAI::BeginPlay()
{
	Super::BeginPlay();

	Player = Cast<APly>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	SpawnDefaultController();

	InRangeComponent->OnComponentBeginOverlap.AddDynamic(this, &AAI::OnOverlapBegin);
	InRangeComponent->OnComponentEndOverlap.AddDynamic(this, &AAI::OnOverlapEnd);
}

void AAI::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	AAIController* aiController = Cast<AAIController>(GetController());

	if (Player->HealthComponent->Health > 0) {
		aiController->MoveToActor(Player, 10.0f, true);
	}
}

void AAI::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->IsA<APly>() && HealthComponent->Health > 0) {
		APly* player = Cast<APly>(OtherActor);

		if (player->HealthComponent->Health > 0) {
			Damage(player);
			GetWorld()->GetTimerManager().SetTimer(DamageTimer, FTimerDelegate::CreateUObject(this, &AAI::Damage, player), 1.0f, true);
		}
	}
}

void AAI::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	GetWorld()->GetTimerManager().ClearTimer(DamageTimer);
}

void AAI::Damage(APly* player)
{
	player->HealthComponent->TakeHealth(34, player);
}

void AAI::Death(FVector DeathInstigatorLocation, FName Bone) 
{
	FCollisionResponseContainer collision_response;
	collision_response.SetResponse(ECC_Pawn, ECollisionResponse::ECR_Overlap);
	AIMesh->SetCollisionResponseToChannels(collision_response);

	AIMesh->SetSimulatePhysics(true);

	const FVector direction = GetActorLocation() + DeathInstigatorLocation;

	AIMesh->SetPhysicsLinearVelocity(direction, false, Bone);

	GetWorld()->GetTimerManager().SetTimer(ClearTimer, this, &AAI::ClearRagdoll, 10.0f, false);
}

void AAI::ClearRagdoll()
{
	Destroy();
}