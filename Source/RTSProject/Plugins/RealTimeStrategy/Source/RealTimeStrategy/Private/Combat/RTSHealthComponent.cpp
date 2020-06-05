#include "Combat/RTSHealthComponent.h"

#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

#include "RTSGameMode.h"
#include "RTSLog.h"


URTSHealthComponent::URTSHealthComponent(const FObjectInitializer& ObjectInitializer /*= FObjectInitializer::Get()*/)
	: Super(ObjectInitializer)
{
	SetIsReplicatedByDefault(true);

	// Set reasonable default values.
	MaximumHealth = 100.0f;
}

void URTSHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(URTSHealthComponent, CurrentHealth);
}

void URTSHealthComponent::BeginPlay()
{
    Super::BeginPlay();

    // Set initial health.
    CurrentHealth = MaximumHealth;

    // Register for events.
    AActor* Owner = GetOwner();

    if (!IsValid(Owner))
    {
        return;
    }

    Owner->OnTakeAnyDamage.AddDynamic(this, &URTSHealthComponent::OnTakeAnyDamage);
}

float URTSHealthComponent::GetMaximumHealth() const
{
    return MaximumHealth;
}

float URTSHealthComponent::GetCurrentHealth() const
{
    return CurrentHealth;
}

void URTSHealthComponent::OnTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
    float OldHealth = CurrentHealth;
    CurrentHealth -= Damage;
    float NewHealth = CurrentHealth;

    UE_LOG(LogRTS, Log, TEXT("Actor %s has taken %f damage from %s, reducing health to %f."),
        *GetOwner()->GetName(),
        Damage,
        *DamageCauser->GetName(),
        CurrentHealth);

    // Notify listeners.
    OnHealthChanged.Broadcast(GetOwner(), OldHealth, NewHealth, DamageCauser);

    // Check if we've just died.
    if (CurrentHealth <= 0)
    {
        UE_LOG(LogRTS, Log, TEXT("Actor %s has been killed."), *GetOwner()->GetName());

        // Get owner before destruction.
        AController* OwningPlayer = Cast<AController>(GetOwner()->GetOwner());

        // Notify listeners.
        OnKilled.Broadcast(GetOwner(), OwningPlayer, DamageCauser);

        // Destroy this actor.
        GetOwner()->Destroy();

        // Notify game mode.
        ARTSGameMode* GameMode = Cast<ARTSGameMode>(UGameplayStatics::GetGameMode(GetWorld()));

        if (GameMode != nullptr)
        {
            GameMode->NotifyOnActorKilled(GetOwner(), OwningPlayer);
        }
    }
}
