// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PluginTestComponent.h"
#include "DummyActorr.generated.h"

UCLASS()
class GAMEAIPLUGIN_API ADummyActorr : public AActor
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(VisibleAnywhere)
		UPluginTestComponent* m_pPluginComponent;

public:	
	// Sets default values for this actor's properties
	ADummyActorr();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
	
};
