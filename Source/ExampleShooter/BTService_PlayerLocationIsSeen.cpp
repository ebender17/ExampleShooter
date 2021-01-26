// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_PlayerLocationIsSeen.h"
#include "AIController.h"
#include "ShooterCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTService_PlayerLocationIsSeen::UBTService_PlayerLocationIsSeen()
{
	NodeName = "Update Player Location if Seen"; 
}

void UBTService_PlayerLocationIsSeen::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	if (OwnerComp.GetAIOwner() == nullptr) return;

	AAIController* AIOwner = Cast<AAIController>(OwnerComp.GetAIOwner());

	if (AIOwner == nullptr) return;

	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

	if (PlayerPawn == nullptr) return; 

	if (AIOwner->LineOfSightTo(PlayerPawn))
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsObject(GetSelectedBlackboardKey(), PlayerPawn);
	}
	else
	{
		OwnerComp.GetBlackboardComponent()->ClearValue(GetSelectedBlackboardKey());
	}
}
