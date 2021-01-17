// Fill out your copyright notice in the Description page of Project Settings.


#include "KillAllGameMode.h"
#include "EngineUtils.h"
#include "GameFramework/Controller.h"
#include "ShooterAIController.h"

void AKillAllGameMode::PawnKilled(APawn* PawnKilled)
{
	Super::PawnKilled(PawnKilled); 

	APlayerController* PlayerController = Cast<APlayerController>(PawnKilled->GetController());
	if (PlayerController != nullptr)
	{
		EndGame(false); 
	}

	for (AShooterAIController* Controller : TActorRange<AShooterAIController>(GetWorld()))
	{
		if (!Controller->IsDead()) return;
	}

	EndGame(true); 
}

void AKillAllGameMode::EndGame(bool bIsPlayerWinner) 
{
	for (AController* Controller : TActorRange<AController>(GetWorld()))
	{
		//Conditions handled by bIsWinner
		//Controller is PlayerController and PlayerWins => true 
		//Controller is not PlayerController and PlayerWins => false 
		//Controller is PlayerController and PlayerWins is false => false
		//Controller is not PlayerController and PlayerWins is false => true 

		bool bIsWinner = Controller->IsPlayerController() == bIsPlayerWinner; 
		Controller->GameHasEnded(Controller->GetPawn(), bIsWinner);
	}

}