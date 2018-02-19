// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "Distractor.generated.h"

/**
 * 
 */
UCLASS()
class GAMEAIPLUGIN_API ADistractor : public ABaseCharacter
{
	GENERATED_BODY()
	
public:
	ADistractor();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void Die();
	
private:
	UPROPERTY(EditAnywhere, DisplayName = "Lifespan")
		float m_fLifespan = 5.f;
};
