// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameHFSMLib.h"
#include "GameBTLib.h"
#include "GameUtilityLib.h"
#include "GameGOAPLib.h"
#include <list>
#include <map>
#include "BaseCharacter.generated.h"

UENUM(BlueprintType)
enum class EAIMode : uint8
{
	VE_NONE 		UMETA(DisplayName = "None"),
	VE_HFSM 		UMETA(DisplayName = "HFSM"),
	VE_BT 			UMETA(DisplayName = "BT"),
	VE_Utility		UMETA(DisplayName = "Utility"),
	VE_GOAP			UMETA(DisplayName = "GOAP")
};

UCLASS()
class GAMEAIPLUGIN_API ABaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABaseCharacter();

	struct LastHit
	{
		ABaseCharacter* other;
		float time;
	};

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void Die();

private:
	FString ReadFile(FString filename);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Called by another character
	void OnHit(ABaseCharacter*);

	void HittingTargetIsDead(ABaseCharacter*);

	float GetHealth();

	float GetDamage();

	bool IsDead();

	int GetGroupID();

	float GetView();

	const LastHit& GetLastHitInfo();

	void AddToList(ABaseCharacter*);

	void RemoveFromList(ABaseCharacter*);
	
	void SetActionTime(const std::string&);

	float GetActionTime(const std::string&);

	//UFUNCTION()
	//	void OnEnterAnotherCharacter(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& ASweepResult);

public:
	void ThrowObject();

	void Fire();

protected:
	class LStream* m_pStream;

	class Blackboard* m_pBlackboard;

	// HFSM
	GameFSM::StateMachine* m_pStateMachine;
	std::vector <GameFSM::State*>* m_pStatePool;
	std::vector <GameFSM::Transition*>* m_pTransitionPool;
	
	// BT
	GameBT::BehaviorTree* m_pBehaviorTree;

	// Utility
	GameUtility::Utility* m_pUtility;

	// GOAP
	std::vector <GameGOAP::Action*> m_Actions;
	std::vector <GameGOAP::Variable*> m_Variables;
	GameGOAP::WorldState* m_InitialState;
	GameGOAP::WorldState* m_GoalState;
	GameGOAP::Planner* m_pPlanner;
	GameGOAP::Runner* m_pRunner;

	UPROPERTY(EditAnywhere, DisplayName="AI Node")
		EAIMode m_eAIMode;

	UPROPERTY(EditAnywhere, DisplayName = "Enemy")
		class AActor* m_pEnemy;

	UPROPERTY(EditAnywhere, DisplayName="View")
		float m_fView = 1.f;

	UPROPERTY(EditAnywhere, DisplayName = "Speed")
		float m_fSpeed = 100.f;

	UPROPERTY(EditAnywhere, DisplayName = "Search Range")
		float m_fSearchRange = 1000.f;

	UPROPERTY(EditAnywhere, DisplayName = "SaveDistance")
		float m_fSaveDistance = 3000.f;

	UPROPERTY(EditAnywhere, DisplayName = "Health")
		float m_fHealth = 100.f;

	UPROPERTY(EditAnywhere, DisplayName = "Damage")
		float m_fDamage = 10.f;

	UPROPERTY(EditAnywhere, DisplayName = "Fire Range")
		float m_fFireRange = 1000.f;

	UPROPERTY(EditAnywhere, DisplayName = "Throw Range")
		float m_fThrowRange = 200.f;

	UPROPERTY(EditAnywhere, DisplayName = "AI Data File")
		FString m_sFilename;

	UPROPERTY(EditAnywhere, DisplayName = "Input File")
		FString m_sFilename2;

	UPROPERTY(EditAnywhere, DisplayName = "AI Tick Rate")
		int m_iRate = 100;

	UPROPERTY(EditAnywhere, DisplayName = "Group ID")
		int m_iGroupID = 0;

	UPROPERTY(EditAnywhere, DisplayName = "Throwed Object")
		TSubclassOf<class ADistractor> m_ThrowedObject;

	bool m_bUseFSM = false;

	bool m_bUseBT = false;

	bool m_bUseUtility = false;

	bool m_bUseGOAP = false;

	int m_iRateIt = 0;

	FVector* m_pTargetPosition;

	FVector* m_pLastKnownPosition;

	float m_fTime;

	float m_fMaxHealth;

	class UStaticMeshComponent* m_pMesh;

	class UTextRenderComponent* m_pHealthText;

	LastHit m_LastHit;

	std::list<ABaseCharacter*> m_EnemyList;

	std::list<ABaseCharacter*> m_FriendList;

	std::map<std::string, float> m_ActionTimeMap;
};