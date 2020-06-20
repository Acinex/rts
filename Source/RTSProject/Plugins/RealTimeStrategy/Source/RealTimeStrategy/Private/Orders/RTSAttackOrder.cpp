#include "Orders/RTSAttackOrder.h"

#include "Combat/RTSAttackComponent.h"
#include "Libraries/RTSGameplayTagLibrary.h"

URTSAttackOrder::URTSAttackOrder(const FObjectInitializer& ObjectInitializer /*= FObjectInitializer::Get()*/)
    : Super(ObjectInitializer)
{
    TargetType = ERTSOrderTargetType::ORDERTARGET_Actor;

    IssueTagRequirements.SourceRequiredTags.AddTag(URTSGameplayTagLibrary::Status_Permanent_CanAttack());

    IssueTagRequirements.SourceBlockedTags.AddTag(URTSGameplayTagLibrary::Status_Changing_Unarmed());

    IssueTagRequirements.TargetRequiredTags.AddTag(URTSGameplayTagLibrary::Status_Permanent_CanBeAttacked());
    IssueTagRequirements.TargetRequiredTags.AddTag(URTSGameplayTagLibrary::Status_Changing_Alive());
    IssueTagRequirements.TargetRequiredTags.AddTag(URTSGameplayTagLibrary::Relationship_Visible());

    IssueTagRequirements.TargetBlockedTags.AddTag(URTSGameplayTagLibrary::Status_Changing_Invulnerable());
    IssueTagRequirements.TargetBlockedTags.AddTag(URTSGameplayTagLibrary::Relationship_Friendly());

    SuccessTagRequirements.TargetBlockedTags.AddTag(URTSGameplayTagLibrary::Status_Changing_Alive());
}

float URTSAttackOrder::GetRequiredRange(const AActor* OrderedActor, int32 Index) const
{
    if (!IsValid(OrderedActor))
    {
        return 0.0f;
    }

    URTSAttackComponent* AttackComponent = OrderedActor->FindComponentByClass<URTSAttackComponent>();
    return IsValid(AttackComponent) ? AttackComponent->GetAttacks()[0].Range : 0.0f;
}
