// Fill out your copyright notice in the Description page of Project Settings.

#include "Distractor.h"

ADistractor::ADistractor()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called every frame
void ADistractor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (m_fTime > m_fLifespan)
	{
		Die();
	}
}

void ADistractor::Die()
{
	Super::Die();

	Destroy();
}