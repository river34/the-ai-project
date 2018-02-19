// Fill out your copyright notice in the Description page of Project Settings.

#include "GameAIPluginGameModeBase.h"
#include "BaseCharacter.h"
#include "GameGOAPLib.h"

int GameGOAP::Node::lastID = 0;

void AGameAIPluginGameModeBase::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);
}