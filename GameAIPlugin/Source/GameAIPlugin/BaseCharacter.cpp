// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseCharacter.h"
#include <iostream>
#include <stdlib.h> 
#include <stdio.h> 
#include "HAL/PlatformFilemanager.h"
#include "Misc/Paths.h"
#include "Misc/FileHelper.h"
#include "GenericPlatform/GenericPlatformFile.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TextRenderComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "GameFrameworkLib.h"
#include "GameMathLib.h"
#include "Distractor.h"
#include "../Helpers/LStream.h"
#include "../HFSM/State/AttackState.h"
#include "../HFSM/State/PatrolState.h"
#include "../HFSM/State/SearchState.h"
#include "../HFSM/Transition/EnemyLostTransition.h"
#include "../HFSM/Transition/EnemySpottedTransition.h"
#include "../BT/Condition/SeeEnemyCondition.h"
#include "../BT/Condition/HaveLastKnownPositionCondition.h"
#include "../BT/Action/AttackEnemyAction.h"
#include "../BT/Action/MoveToEnemyAction.h"
#include "../BT/Action/MoveToLastKnownPositionAction.h"
#include "../BT/Action/MoveToRandomPositionAction.h"
#include "../BT/Action/WaitAction.h"
#include "../BT/Action/ClearLastKnownPositionAction.h"
#include "../Utility/Action/AttackAction.h"
#include "../Utility/Action/AlertAction.h"
#include "../Utility/Action/InvestigateAction.h"
#include "../Utility/Action/MoveToEnemyAction.h"
#include "../Utility/Action/RunAction.h"
#include "../Utility/Action/SearchAction.h"
#include "../Utility/Action/TakeCoverAction.h"
#include "../Utility/Input/DistanceInput.h"
#include "../Utility/Input/EnemyHealthInput.h"
#include "../Utility/Input/HaveLKPInput.h"
#include "../Utility/Input/HealthInput.h"
#include "../Utility/Input/HitByEnemyInput.h"
#include "../Utility/Input/SeeEnemyInput.h"
#include "../Utility/Input/TimeInput.h"
#include "../GOAP/Action/AttackAction.h"
#include "../GOAP/Action/GoToEnemyAction.h"
#include "../GOAP/Action/SearchAction.h"
#include "../GOAP/Variable/EnemyInRangeVariable.h"
#include "../GOAP/Variable/EnemyDeadVariable.h"
#include "../GOAP/Variable/EnemyLostVariable.h"

using namespace std;

// Sets default values
ABaseCharacter::ABaseCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Redirect the cout stream into new buffer
	m_pStream = new LStream;
	std::cout.rdbuf(m_pStream);
	std::cout << "ABaseCharacter" << std::endl;

	m_pTargetPosition = new FVector;
	m_pLastKnownPosition = new FVector(0, 0, 0);

	m_fTime = 0;

	if (m_fHealth > 0)
	{
		m_fMaxHealth = m_fHealth;
	}
	else
	{
		m_fMaxHealth = 1.f;
	}

	// create health text
	//m_pHealthText = CreateDefaultSubobject<UTextRenderComponent>("HealthText");
	//m_pHealthText->SetupAttachment(RootComponent);
	//m_pHealthText->SetRelativeLocation(FVector(0.f, 0.f, 20.0f));
	//m_pHealthText->SetXScale(1.f);
	//m_pHealthText->SetYScale(1.f);
	//m_pHealthText->SetWorldSize(60);
	//m_pHealthText->SetTextRenderColor(FColor::Red);
	//m_pHealthText->HorizontalAlignment = EHorizTextAligment::EHTA_Center;
	//m_pHealthText->SetText(FText::FromString(FString::SanitizeFloat(m_fHealth)));

	m_LastHit = { nullptr, 0 };
}

// Called when the game starts or when spawned
void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Create target
	*m_pTargetPosition = GetActorLocation();

	// Default enemy?
	if (m_pEnemy == nullptr)
	{
		if (m_EnemyList.empty() == false)
		{
			m_pEnemy = m_EnemyList.front();
		}
	}

	// Initialize blackboard
	m_pBlackboard = new Blackboard();
	m_pBlackboard->registerEntry("self", this);
	m_pBlackboard->registerEntry("enemy", m_pEnemy);
	m_pBlackboard->registerEntry("view", &m_fView);
	m_pBlackboard->registerEntry("speed", &m_fSpeed);
	m_pBlackboard->registerEntry("target", m_pTargetPosition);
	m_pBlackboard->registerEntry("lkp", m_pLastKnownPosition);
	m_pBlackboard->registerEntry("searchRange", &m_fSearchRange);
	m_pBlackboard->registerEntry("saveDistance", &m_fSaveDistance);
	//m_pBlackboard->registerEntry("health", &m_fHealth);
	//m_pBlackboard->registerEntry("damage", &m_fDamage);

	// Set speed
	UCharacterMovementComponent* movement = GetCharacterMovement();
	movement->MaxWalkSpeed = m_fSpeed;

	// Check blackboard
	std::cout << "Blackboard " << std::endl;
	std::cout << "view = " << *((float*)(m_pBlackboard->getEntry("view"))) << std::endl;
	AActor* self = (AActor*)(m_pBlackboard->getEntry("self"));
	FVector selfPosition = self->GetActorLocation();
	GameMath::Vector3 pos = GameMath::Vector3(selfPosition.X, selfPosition.Y, selfPosition.Z);
	std::cout << pos.toString() << std::endl;

	// Select AI 
	if (m_eAIMode == EAIMode::VE_HFSM)
	{
		m_bUseFSM = true;
	}
	else if (m_eAIMode == EAIMode::VE_BT)
	{
		m_bUseBT = true;
	}
	else if (m_eAIMode == EAIMode::VE_Utility)
	{
		m_bUseUtility = true;
	}
	else if (m_eAIMode == EAIMode::VE_GOAP)
	{
		m_bUseGOAP = true;
	}

	if (m_bUseFSM == true)
	{
		// Create state machine
		m_pStateMachine = new GameFSM::StateMachine;
		m_pStatePool = new vector<GameFSM::State*>();
		m_pStatePool->reserve(20);
		m_pTransitionPool = new vector<GameFSM::Transition*>();
		m_pTransitionPool->reserve(20);

		// Create factory
		GameFSM::StateFactory* stateFactory = new GameFSM::StateFactory;
		stateFactory->registerClass("Attack", &AttackState::create);
		stateFactory->registerClass("Patrol", &PatrolState::create);
		stateFactory->registerClass("Search", &SearchState::create);
		GameFSM::TransitionFactory* transitionFactory = new GameFSM::TransitionFactory;
		transitionFactory->registerClass("EnemyLost", &EnemyLostTransition::create);
		transitionFactory->registerClass("EnemySpotted", &EnemySpottedTransition::create);

		// Load FSM from file
		GameFSM::StateMachineLoader* loader = new GameFSM::StateMachineLoader;
		std::string file = std::string(TCHAR_TO_UTF8(*ReadFile(m_sFilename)));
		std::cout << "File loaded: " << file.size() << std::endl;
		if (file.size() > 0)
		{
			loader->loadStateMachine(file, m_pStateMachine, *m_pStatePool, *m_pTransitionPool, stateFactory, transitionFactory, false);

			// check FSM
			std::cout << "m_pStatePool: " << m_pStatePool->size() << std::endl;
			std::cout << "m_pTransitionPool: " << m_pTransitionPool->size() << std::endl;
		}
		else
		{
			m_bUseFSM = false;
		}
	}

	if (m_bUseBT == true)
	{
		// create behavior tree
		m_pBehaviorTree = new GameBT::BehaviorTree;

		// Create factory
		GameBT::BehaviorFactory* behaviorFactory = new GameBT::BehaviorFactory;
		behaviorFactory->registerClass("BehaviorTree", &Root::create);
		behaviorFactory->registerClass("Selector", &Selector::create);
		behaviorFactory->registerClass("ActiveSelector", &ActiveSelector::create);
		behaviorFactory->registerClass("Filter", &Filter::create);
		behaviorFactory->registerClass("Sequence", &Sequence::create);
		behaviorFactory->registerClass("Repeater", &Repeater::create);
		behaviorFactory->registerClass("SeeEnemyCondition", &SeeEnemyCondition::create);
		behaviorFactory->registerClass("MoveToEnemyAction", &MoveToEnemyAction::create);
		behaviorFactory->registerClass("AttackEnemyAction", &AttackEnemyAction::create);
		behaviorFactory->registerClass("HaveLastKnownPositionCondition", &HaveLastKnownPositionCondition::create);
		behaviorFactory->registerClass("MoveToLastKnownPositionAction", &MoveToLastKnownPositionAction::create);
		behaviorFactory->registerClass("MoveToRandomPositionAction", &MoveToRandomPositionAction::create);
		behaviorFactory->registerClass("WaitAction", &WaitAction::create);
		behaviorFactory->registerClass("ClearLastKnownPositionAction", &ClearLastKnownPositionAction::create);

		// Load BT from file
		GameBT::BehaviorTreeLoader* loader = new GameBT::BehaviorTreeLoader;
		std::string file = std::string(TCHAR_TO_UTF8(*ReadFile(m_sFilename)));
		std::cout << "File loaded: " << file.size() << std::endl;
		if (file.size() > 0)
		{
			loader->loadBehaviorTree(file, m_pBehaviorTree, behaviorFactory, false);
		}
		else
		{
			m_bUseBT = false;
		}
	}

	if (m_bUseUtility == true)
	{
		// create utility system
		m_pUtility = new GameUtility::Utility;

		// Create factory
		GameUtility::ResponseCurveFactory* curveFactory = new GameUtility::ResponseCurveFactory;
		curveFactory->registerClass("ResponseCurve", &GameUtility::ResponseCurve::create);

		GameUtility::ConsiderationFactory* considerationFactory = new GameUtility::ConsiderationFactory;
		considerationFactory->registerClass("Consideration", &GameUtility::Consideration::create);

		GameUtility::ActionFactory* actionFactory = new GameUtility::ActionFactory;
		actionFactory->registerClass("AlertAction", &GameUtility::AlertAction::create);
		actionFactory->registerClass("AttackAction", &GameUtility::AttackAction::create);
		actionFactory->registerClass("InvestigateAction", &GameUtility::InvestigateAction::create);
		actionFactory->registerClass("MoveToEnemyAction", &GameUtility::MoveToEnemyAction::create);
		actionFactory->registerClass("RunAction", &GameUtility::RunAction::create);
		actionFactory->registerClass("SearchAction", &GameUtility::SearchAction::create);
		actionFactory->registerClass("TakeCoverAction", &GameUtility::TakeCoverAction::create);

		GameUtility::InputFactory* inputFactory = new GameUtility::InputFactory;
		inputFactory->registerClass("DistanceInput", &GameUtility::DistanceInput::create);
		inputFactory->registerClass("EnemyHealthInput", &GameUtility::EnemyHealthInput::create);
		inputFactory->registerClass("HaveLKPInput", &GameUtility::HaveLKPInput::create);
		inputFactory->registerClass("HealthInput", &GameUtility::HealthInput::create);
		inputFactory->registerClass("HitByEnemyInput", &GameUtility::HitByEnemyInput::create);
		inputFactory->registerClass("SeeEnemyInput", &GameUtility::SeeEnemyInput::create);
		inputFactory->registerClass("TimeInput", &GameUtility::TimeInput::create);

		// create pool
		vector<GameUtility::Input*> inputPool;
		inputPool.reserve(MAX_INPUTS);

		// load utility input from file 2
		GameUtility::InputLoader* inputLoader = new GameUtility::InputLoader;
		std::string file2 = std::string(TCHAR_TO_UTF8(*ReadFile(m_sFilename2)));
		std::cout << "File loaded: " << file2.size() << std::endl;
		if (file2.size() > 0)
		{
			inputLoader->loadInput(file2, m_pUtility, inputFactory, inputPool, false);
		}
		else
		{
			m_bUseUtility = false;
		}

		if (m_bUseUtility == true)
		{
			// Load utility system from file
			GameUtility::UtilityLoader* utilityLoader = new GameUtility::UtilityLoader;
			std::string file = std::string(TCHAR_TO_UTF8(*ReadFile(m_sFilename)));
			std::cout << "File loaded: " << file.size() << std::endl;
			if (file.size() > 0)
			{
				utilityLoader->loadUtility(file, m_pUtility, curveFactory, considerationFactory, actionFactory, inputPool, false);
			}
			else
			{
				m_bUseUtility = false;
			}
		}
	}

	if (m_bUseGOAP == true)
	{
		// create action list
		m_Actions.reserve(MAX_ACTIONS);

		// create variable list
		m_Variables.reserve(MAX_VARIABLES);

		// create initial and goal states
		m_InitialState = new GameGOAP::WorldState;
		m_GoalState = new GameGOAP::WorldState;

		// create factory
		GameGOAP::ActionFactory* actionFactory = new GameGOAP::ActionFactory;
		actionFactory->registerClass("AttackAction", &GameGOAP::AttackAction::create);
		actionFactory->registerClass("GoToEnemyAction", &GameGOAP::GoToEnemyAction::create);
		actionFactory->registerClass("SearchAction", &GameGOAP::SearchAction::create);
		GameGOAP::VariableFactory* variableFactory = new GameGOAP::VariableFactory;
		variableFactory->registerClass("EnemyInRangeVariable", &GameGOAP::EnemyInRangeVariable::create);
		variableFactory->registerClass("EnemyDeadVariable", &GameGOAP::EnemyDeadVariable::create);
		variableFactory->registerClass("EnemyLostVariable", &GameGOAP::EnemyLostVariable::create);

		// load from file
		GameGOAP::GOAPLoader* loader = new GameGOAP::GOAPLoader;
		std::string file = std::string(TCHAR_TO_UTF8(*ReadFile(m_sFilename)));
		std::cout << "File loaded: " << file.size() << std::endl;
		if (file.size() > 0)
		{
			loader->loaGOAP(file, actionFactory, m_Actions, variableFactory, m_Variables, *m_InitialState, *m_GoalState, false);

			// create planner
			m_pPlanner = new GameGOAP::Planner;

			// create runner
			m_pRunner = new GameGOAP::Runner(m_Actions, m_pPlanner, m_InitialState, m_GoalState);

			// check actions
			for (auto action : m_Actions)
			{
				std::cout << "  " << action->getName() << " preconditions = " << action->preconditiins() << ", effects = " << action->effects() << std::endl;
			}

			// check variables
			for (auto var : m_Variables)
			{
				std::cout << "  " << var->getName() << std::endl;
			}
		}
		else
		{
			m_bUseGOAP = false;
		}
	}

	// check rate
	if (m_iRate <= 1)
	{
		m_iRate = 1;
	}

	m_iRateIt = 0;

	TArray<UStaticMeshComponent*> comps;
	GetComponents(comps);
	for (int i = 0; i < comps.Num(); i++)
	{
		if (comps[i]->GetName() == "Body")
		{
			m_pMesh = comps[i];
			break;
		}
	}

	TArray<UTextRenderComponent*> textComps;
	GetComponents(textComps);
	for (int i = 0; i < textComps.Num(); i++)
	{
		if (textComps[i]->GetName() == "HealthText")
		{
			m_pHealthText = textComps[i];
			m_pHealthText->SetText(FText::FromString(FString::FromInt(m_fHealth)));
			break;
		}
	}

	// Unique id
	FString label = GetActorLabel();
	std::cout << TCHAR_TO_UTF8(*label) << " unique id = " << GetUniqueID() << std::endl;

	// group id 
	std::cout << TCHAR_TO_UTF8(*label) << " groupID = " << GetGroupID() << std::endl;

	// Find all characters in this level
	TSubclassOf<ABaseCharacter> classToFind;
	classToFind = ABaseCharacter::StaticClass();
	TArray<AActor*> found;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), classToFind, found);
	for (int i = 0; i < found.Num(); i++) {
		AActor* actor = found[i];
		ABaseCharacter* baseActor = (ABaseCharacter*)actor;
		if (baseActor != nullptr)
		{
			if (baseActor->GetUniqueID() != GetUniqueID())
			{
				AddToList(baseActor);
			}
		}
	}
}

// Called every frame
void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	m_fTime += DeltaTime;

	if (m_iRateIt % m_iRate == 0)
	{
		m_iRateIt = 0;

		if (m_bUseFSM == true && m_pStateMachine != nullptr)
		{
			std::cout << "---------------------" << std::endl;
			std::cout << "time: " << m_fTime << std::endl;
			m_pStateMachine->update(DeltaTime, m_pBlackboard);
		}
		else if (m_bUseBT == true && m_pBehaviorTree != nullptr)
		{
			std::cout << "---------------------" << std::endl;
			std::cout << "time: " << m_fTime << std::endl;
			m_pBehaviorTree->tick(m_pBlackboard);
		}
		else if (m_bUseUtility == true && m_pUtility != nullptr)
		{
			std::cout << "---------------------" << std::endl;
			std::cout << "time: " << m_fTime << std::endl;
			m_pUtility->tick(m_pBlackboard);
		}
		else if (m_bUseGOAP == true && m_pPlanner != nullptr && m_pRunner != nullptr)
		{
			std::cout << "---------------------" << std::endl;
			std::cout << "time: " << m_fTime << std::endl;
			m_pRunner->tick(m_pBlackboard);
			for (auto var : m_Variables)
			{
				//std::cout << "update variable: " << var->getName() << std::endl;
				var->onUpdate(m_pBlackboard);
				//std::cout << "  Var: " << var->getName() << " = " << var->getValue() << std::endl;
			}
			if (m_InitialState->onUpdate(m_Variables))
			{
				//std::cout << "Is changed" << std::endl;
				m_pRunner->tick(m_pBlackboard, true);
			}
			std::cout << "Current state: " << *m_InitialState << std::endl;
		}
	}

	m_iRateIt++;
}

// Called to bind functionality to input
void ABaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	check(PlayerInputComponent);
}
 
FString ABaseCharacter::ReadFile(FString filename)
{
	//Read file ini [project]/Content/Data/ 
	FString directory = FPaths::ProjectContentDir() + "/Data";
	FString result;
	IPlatformFile& file = FPlatformFileManager::Get().GetPlatformFile();
	if (file.CreateDirectory(*directory)) {
		FString myFile = directory + "/" + filename;
		std::cout << std::string(TCHAR_TO_UTF8(*myFile)) << std::endl;
		FFileHelper::LoadFileToString(result, *myFile);
	}
	return result;
}

void ABaseCharacter::HittingTargetIsDead(ABaseCharacter* _target)
{
	FString label = GetActorLabel();
	std::cout << TCHAR_TO_UTF8(*label) << " HittingTargetIsDead" << std::endl;
}

void ABaseCharacter::OnHit(ABaseCharacter* _other)
{
	if (IsDead() == true)
	{
		_other->HittingTargetIsDead(this);
		return;
	}

	m_LastHit.other = _other;
	m_LastHit.time = GetWorld()->GetTimeSeconds();

	m_fHealth -= _other->GetDamage();
	if (m_fHealth <= 0)
	{
		m_fHealth = 0;
		Die();
	}

	FString label = GetActorLabel();
	std::cout << TCHAR_TO_UTF8(*label) << " OnHit. health = " << m_fHealth << std::endl;
	if (m_pMesh != nullptr)
	{
		UMaterialInstanceDynamic* materialInstance = m_pMesh->CreateDynamicMaterialInstance(0);
		if (materialInstance != nullptr)
		{
			materialInstance->SetScalarParameterValue(FName("Alpha"), m_fHealth / m_fMaxHealth);
		}
	}
	if (m_pHealthText != nullptr)
	{
		m_pHealthText->SetText(FText::FromString(FString::FromInt(m_fHealth)));
	}
}

float ABaseCharacter::GetHealth()
{
	return m_fHealth;
}

float ABaseCharacter::GetDamage()
{
	return m_fDamage;
}

bool ABaseCharacter::IsDead()
{
	if (m_fHealth <= 0)
	{
		return true;
	}
	return false;
}

void ABaseCharacter::Die()
{
	FString label = GetActorLabel();
	std::cout << TCHAR_TO_UTF8(*label) << " Die" << std::endl;
	SetActorTickEnabled(false);
	for (auto c : m_FriendList)
	{
		c->RemoveFromList(this);
	}
	for (auto c : m_EnemyList)
	{
		c->RemoveFromList(this);
	}
}

void ABaseCharacter::ThrowObject()
{
	FString label = GetActorLabel();
	std::cout << TCHAR_TO_UTF8(*label) << " ThrowObject" << std::endl;

	if (m_ThrowedObject != nullptr)
	{
		FActorSpawnParameters spawnParams;
		spawnParams.Owner = this;
		FRotator rotator = GetActorRotation();
		FVector spawnLocation = GetActorLocation() + GetActorForwardVector() * m_fThrowRange;
		ADistractor* newActor = GetWorld()->SpawnActor<ADistractor>(m_ThrowedObject, spawnLocation, rotator, spawnParams);
		newActor->m_iGroupID = GetGroupID();
		AddToList(newActor);
		for (auto c : m_EnemyList)
		{
			c->AddToList(newActor);
		}
	}
}

void ABaseCharacter::Fire()
{
	FString label = GetActorLabel();
	std::cout << TCHAR_TO_UTF8(*label) << " Fire" << std::endl;

	// cast a ray in forward direction, last for 1 sec
	// if hit, send onHit command to the hitted actor
	FHitResult Hit;
	FVector StartLocation = GetActorLocation();
	FVector EndLocation = StartLocation + GetActorForwardVector() * m_fFireRange;
	APlayerController* PC = Cast<APlayerController>(Controller);
	if (PC != nullptr)
	{
		FVector CameraLocation = PC->PlayerCameraManager->GetCameraLocation();
		FRotator Rotation = PC->PlayerCameraManager->GetCameraRotation();
		FVector Direction = FRotationMatrix(Rotation).GetScaledAxis(EAxis::X);
		EndLocation = StartLocation + Direction * m_fFireRange;
		//DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Blue, true, 1.f, 0, 1.f);
	}
	FCollisionQueryParams CollisionParameters;
	if (GetWorld()->LineTraceSingleByChannel(Hit, StartLocation, EndLocation, ECollisionChannel::ECC_GameTraceChannel2, CollisionParameters))
	{
		DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Purple, true, 1.f, 0, 1.f);

		AActor* other = Hit.GetActor();
		if (other != nullptr)
		{
			FString label2 = other->GetActorLabel();
			std::cout << TCHAR_TO_UTF8(*label) << " Fire at " << TCHAR_TO_UTF8(*label2) << std::endl;
			ABaseCharacter* base = (ABaseCharacter*)other;
			if (base != nullptr && base->GetGroupID() != m_iGroupID)
			{
				base->OnHit(this);
			}
		}
	}
}

int ABaseCharacter::GetGroupID()
{
	return m_iGroupID;
}

float ABaseCharacter::GetView()
{
	return m_fView;
}

const ABaseCharacter::LastHit& ABaseCharacter::GetLastHitInfo()
{
	return m_LastHit;
}

void ABaseCharacter::AddToList(ABaseCharacter* c)
{
	if (c->GetUniqueID() == GetUniqueID()) return;

	FString label = GetActorLabel();
	FString labelOther = c->GetActorLabel();
	std::cout << TCHAR_TO_UTF8(*label) << " groupID = " << GetGroupID() << " AddToList " << TCHAR_TO_UTF8(*labelOther) << " groupID = " << c->GetGroupID() << std::endl;

	if (c->GetGroupID() == GetGroupID())
	{
		m_FriendList.push_back(c);
		std::cout << TCHAR_TO_UTF8(*label) << " add to friend list = " << TCHAR_TO_UTF8(*labelOther) << std::endl;
	}
	else
	{
		m_EnemyList.push_back(c);
		std::cout << TCHAR_TO_UTF8(*label) << " add to enemy list = " << TCHAR_TO_UTF8(*labelOther) << std::endl;
		if (m_pUtility != nullptr)
		{
			std::string targetName = "Character_" + std::to_string(c->GetUniqueID());
			GameUtility::Target* target = new GameUtility::Target(targetName, c);
			m_pUtility->addTarget(target);
			std::cout << TCHAR_TO_UTF8(*label) << " add utility target = " << targetName << std::endl;
		}
	}
}

void ABaseCharacter::RemoveFromList(ABaseCharacter* c)
{
	if (c->GetUniqueID() == GetUniqueID()) return;

	FString label = GetActorLabel();
	FString labelOther = c->GetActorLabel();
	std::cout << TCHAR_TO_UTF8(*label) << " RemoveFromList " << TCHAR_TO_UTF8(*labelOther) << std::endl;

	for (auto a : m_FriendList)
	{
		std::cout << "a->GetUniqueID() = " << a->GetUniqueID() << " c->GetUniqueID() = " << c->GetUniqueID() << std::endl;
		if (a->GetUniqueID() == c->GetUniqueID())
		{
			std::cout << TCHAR_TO_UTF8(*label) << " RemoveFrom Friend List" << std::endl;
			m_FriendList.remove(a);
			break;
		}
	}
	std::cout << TCHAR_TO_UTF8(*label) << " m_FriendList.size() = " << m_FriendList.size() << std::endl;

	for (auto a : m_EnemyList)
	{
		std::cout << "a->GetUniqueID() = " << a->GetUniqueID() << " c->GetUniqueID() = " << c->GetUniqueID() << std::endl;
		if (a->GetUniqueID() == c->GetUniqueID())
		{
			std::cout << TCHAR_TO_UTF8(*label) << " RemoveFrom Enemy List" << std::endl;
			m_EnemyList.remove(a);
			break;
		}
	}
	std::cout << TCHAR_TO_UTF8(*label) << " m_EnemyList.size() = " << m_EnemyList.size() << std::endl;

	if (m_pUtility != nullptr)
	{
		m_pUtility->removeTarget("Character_" + std::to_string(c->GetUniqueID()));
	}
}

void ABaseCharacter::SetActionTime(const std::string& name)
{
	m_ActionTimeMap[name] = m_fTime;
}

float ABaseCharacter::GetActionTime(const std::string& name)
{
	return m_ActionTimeMap[name];
}