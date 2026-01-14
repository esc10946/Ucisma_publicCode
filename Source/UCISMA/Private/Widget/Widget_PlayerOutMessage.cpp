// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Widget_PlayerOutMessage.h"

void UWidget_PlayerOutMessage::PrintPlayerOutMessage(float AnimSpeed)
{
	PlayAnimation(PrintMessage, 0.0f, 1, EUMGSequencePlayMode::Forward, AnimSpeed);
}