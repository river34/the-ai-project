// Fill out your copyright notice in the Description page of Project Settings.

#include "DummyActorr.h"
#include "GameMathLib.h"

// Sets default values
ADummyActorr::ADummyActorr()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	m_pPluginComponent = CreateDefaultSubobject<UPluginTestComponent>(FName("PluginTestComponent"));
}

// Called when the game starts or when spawned
void ADummyActorr::BeginPlay()
{
	Super::BeginPlay();
	
	GameMath::Vector3 vec = GameMath::Vector3();
	FString s = FString(vec.toString().c_str());
	UE_LOG(LogTemp, Warning, TEXT("%s"), *s);
}

// Called every frame
void ADummyActorr::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

