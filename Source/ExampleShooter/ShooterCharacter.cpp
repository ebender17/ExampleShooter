// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterCharacter.h"
#include "Gun.h"
#include "Components/CapsuleComponent.h"
#include "ExampleShooterGameModeBase.h"

// Sets default values
AShooterCharacter::AShooterCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AShooterCharacter::BeginPlay()
{
	Super::BeginPlay();

	Health = MaxHealth;

	GetMesh()->HideBoneByName(TEXT("weapon_r"), EPhysBodyOp::PBO_None);

	for (int i = 0; i < GunsClass.Num(); i++)
	{
		//Spawn all weapons 
		Guns.Add(GetWorld()->SpawnActor<AGun>(GunsClass[i]));
	
		Guns[i]->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("WeaponSocket"));
		Guns[i]->SetOwner(this);
		if (WeaponSelected != i)
		{
			Guns[i]->SetActorHiddenInGame(true);
		}
	}

}

// Called every frame
void AShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AShooterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &AShooterCharacter::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &AShooterCharacter::MoveRight);
	PlayerInputComponent->BindAxis(TEXT("LookRight"), this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis(TEXT("LookUpRate"), this, &AShooterCharacter::LookUpRate);
	PlayerInputComponent->BindAxis(TEXT("LookRightRate"), this, &AShooterCharacter::LookRightRate);
	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Pressed,this, &ACharacter::Jump);
	PlayerInputComponent->BindAction(TEXT("Shoot"), IE_Pressed, this, &AShooterCharacter::Shoot);
	PlayerInputComponent->BindAction(TEXT("SelectPrimary"), IE_Pressed, this, &AShooterCharacter::SelectPrimary);
	PlayerInputComponent->BindAction(TEXT("SelectSecondary"), IE_Pressed, this, &AShooterCharacter::SelectSecondary);
}

void AShooterCharacter::MoveForward(float AxisValue)
{
	//Do not need to mult by GetDeltaSeconds as MovementInput already supposes input is a rate not an actual amount of movement
	//Does calculation for frame rate independence under the hood 
	AddMovementInput(GetActorForwardVector() * AxisValue);

}

void AShooterCharacter::MoveRight(float AxisValue)
{
	AddMovementInput(GetActorRightVector() * AxisValue);

}

void AShooterCharacter::LookUpRate(float AxisValue)
{
	AddControllerPitchInput(AxisValue * RotationRate * GetWorld()->GetDeltaSeconds());

}

void AShooterCharacter::LookRightRate(float AxisValue)
{
	AddControllerYawInput(AxisValue * RotationRate * GetWorld()->GetDeltaSeconds());
}

void AShooterCharacter::Shoot()
{
	Guns[WeaponSelected]->PullTrigger();

}

void AShooterCharacter::SelectPrimary()
{
	WeaponSelected = 0; 
	UE_LOG(LogTemp, Warning, TEXT("Primary Selected"));

	ShowWeaponSelected(); 
}

void AShooterCharacter::SelectSecondary()
{
	WeaponSelected = 1; 
	UE_LOG(LogTemp, Warning, TEXT("Secondary Selected"));
	
	ShowWeaponSelected();
}

void AShooterCharacter::ShowWeaponSelected()
{
	for (int i = 0; i < GunsClass.Num(); i++)
	{
		if (WeaponSelected == i)
		{
			Guns[i]->SetActorHiddenInGame(false);
		}
		else
		{
			Guns[i]->SetActorHiddenInGame(true);
		}
	}
}

float AShooterCharacter::GetHealthPercent() const
{
	return Health / MaxHealth;
}

float AShooterCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	float DamageToApply = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	DamageToApply = FMath::Min(Health, DamageToApply);
	Health -= DamageToApply; 
	UE_LOG(LogTemp, Warning, TEXT("Health left %f"), Health);

	if (IsDead())
	{
		//Call to game mode that character has been killed
		AExampleShooterGameModeBase* GameMode = GetWorld()->GetAuthGameMode<AExampleShooterGameModeBase>();

		if (GameMode != nullptr)
		{
			GameMode->PawnKilled(this);
		}

		//Detach controller from character, stops AI from shooting and us from moving and shooting
		DetachFromControllerPendingDestroy(); 
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision); 

	}

	return DamageToApply;
}

bool AShooterCharacter::IsDead() const
{
	return Health <= 0;
}