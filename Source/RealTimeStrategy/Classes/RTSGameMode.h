#pragma once

#include "CoreMinimal.h"
#include "RTSRace.h"

#include "GameFramework/GameModeBase.h"
#include "Templates/SubclassOf.h"

#include "RTSGameMode.generated.h"


class URTSOwnerComponent;
class AController;

class URTSGameInstanceSubSystem;
class ARTSPlayerAIController;
class ARTSPlayerController;
class ARTSPlayerStart;
class ARTSTeamInfo;


/**
* Game mode with RTS features, such as spawning initial units for each player.
*/
UCLASS(Category="RTS")
class REALTIMESTRATEGY_API ARTSGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ARTSGameMode(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void BeginPlay() override;
	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;

	ARTSPlayerStart* FindRTSPlayerStart(AController* Player);

	virtual void RestartPlayer(AController* NewPlayer) override;
	virtual void RestartPlayerAtPlayerStart(AController* NewPlayer, AActor* StartSpot) override;

	UFUNCTION(BlueprintPure)
	ARTSTeamInfo* GetWinnerTeam() const { return WinnerTeam; }
	
	UFUNCTION(BlueprintPure)
	AController* GetController(int32 Index);

	virtual ARTSPlayerAIController* StartAIPlayer();

	UFUNCTION(BlueprintPure)
	FLinearColor GetPlayerColor(uint8 Index);

	UFUNCTION(BlueprintPure)
	URTSRace* GetPlayerRace(uint8 Index);

	/** Spawns an actor of the specified type and transfers ownership to the specified player. */
	virtual AActor* SpawnActorForPlayer(TSubclassOf<AActor> ActorClass, AController* ActorOwner, const FTransform& SpawnTransform);

	/** Sets the specified player as the owner of the passed actor. */
	UFUNCTION(BlueprintCallable)
	void TransferOwnership(AActor* Actor, AController* NewOwner);

	/** Gets the teams of the current match. */
	UFUNCTION(BlueprintPure)
	TArray<ARTSTeamInfo*> GetTeams() const;

	/** Event when an actor has been killed. */
	virtual void NotifyOnActorKilled(AActor* Actor, AController* ActorOwner);

	/** Event when a player has been defeated. */
	virtual void NotifyOnPlayerDefeated(AController* Player);

	virtual void NotifyGameOver(ARTSTeamInfo* InWinnerTeam);

	/** Event when a player has been defeated. */
	UFUNCTION(BlueprintImplementableEvent, Category = "RTS", meta = (DisplayName = "OnPlayerDefeated"))
	void ReceiveOnPlayerDefeated(AController* Player);

	/** Event when a team has won. */
	UFUNCTION(BlueprintImplementableEvent, Category = "RTS", meta = (DisplayName = "OnGameOver"))
	void ReceiveOnGameOver(ARTSTeamInfo* Winner);

	UFUNCTION(BlueprintCallable, Category = "RTS")
	FRaceUnitData GetRaceUnitData(URTSRace* Race, uint8 PlayerIndex);


protected:
	UPROPERTY(EditAnywhere, Category="RTS|Players")
	TArray<FLinearColor> PlayerColors;
	UPROPERTY(EditAnywhere, Category="RTS|Players")
	TArray<URTSRace*> PlayerRaces;

	UPROPERTY(EditDefaultsOnly, Category="RTS|Races")
	TArray<FRaceUnitData> RaceUnitData;

private:
	UPROPERTY()
	URTSGameInstanceSubSystem* GameInstanceSubSystem;

	/** Class of TeamInfo to spawn. */
	UPROPERTY(EditDefaultsOnly, Category = "RTS|Team")
	TSubclassOf<ARTSTeamInfo> TeamClass;

	/** Number of teams to create. */
	UPROPERTY(EditDefaultsOnly, Category = "RTS|Team", meta = (ClampMin = 0))
	uint8 NumTeams;

	/** AIController class to spawn for AI players. */
	UPROPERTY(EditDefaultsOnly, Category = "RTS|AI")
	TSubclassOf<ARTSPlayerAIController> PlayerAIControllerClass;

	/** Number of AI players to spawn. */
	UPROPERTY(EditDefaultsOnly, Category = "RTS|AI")
	int32 NumAIPlayers;

	/** Teams of the current match. */
	UPROPERTY()
	TArray<ARTSTeamInfo*> Teams;

	UPROPERTY()
	ARTSTeamInfo* WinnerTeam;

	TArray<int32> DefeatedPlayers;

	/** Gets the first player index that isn't assigned to any player. */
	uint8 GetAvailablePlayerIndex() const;

	UFUNCTION()
	void OnActorRegistered(UActorComponent* Component);

	/** Will transfer the ownership from every actor which has no owner currently */
	void CorrectAllDefaultOwnerStates();
	bool CorrectDefaultOwnerState(URTSOwnerComponent* OwnerComponent);
};
