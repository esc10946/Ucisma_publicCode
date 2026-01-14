// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

//Log_PSJ라는 로그 카테고리 추가, 카테고리 이름은 변경 가능
DECLARE_LOG_CATEGORY_EXTERN(Log_PSJ, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(Log_SGS, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(Log_KHJ, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(Log_DEBUG, Log, All);

//로그를 호출한 함수의 이름과 라인넘버를 FString으로 선언한 매크로
#define LOG_CALLINFO (FString(__FUNCTION__) + TEXT("(") + FString::FromInt(__LINE__) + TEXT(")"))

//디버그 이름만 수정하면 사용 가능
//로그를 호출한 함수의 이름과 라인넘버가 담긴 로그(PSJ)
#define LOG_PSJ(Verbosity, Format, ...) UE_LOG(Log_PSJ, Verbosity, TEXT("%s %s"), *LOG_CALLINFO, *FString::Printf(Format, ##__VA_ARGS__))
//로그를 호출한 함수의 이름과 라인넘버가 담긴 로그(SGS)
#define LOG_SGS(Verbosity, Format, ...) UE_LOG(Log_SGS, Verbosity, TEXT("%s %s"), *LOG_CALLINFO, *FString::Printf(Format, ##__VA_ARGS__))
//로그를 호출한 함수의 이름과 라인넘버가 담긴 로그(KHJ)
#define LOG_KHJ(Verbosity, Format, ...) UE_LOG(Log_KHJ, Verbosity, TEXT("%s %s"), *LOG_CALLINFO, *FString::Printf(Format, ##__VA_ARGS__))
//로그를 호출한 함수의 이름과 라인넘버가 담긴 로그(Debug)
#define LOG_DEBUG(Verbosity, Format, ...) UE_LOG(Log_DEBUG, Verbosity, TEXT("%s %s"), *LOG_CALLINFO, *FString::Printf(Format, ##__VA_ARGS__))

//화면에 로그 출력
//TIME : 출력할 시간(float), Format~ : 출력할 텍스트 | 지정된 색상, 시간, 문자열에 따라 화면에 출력하는 매크로(PSJ)
#define LOG_PSJ_SCREEN(TIME, Format, ...) GEngine->AddOnScreenDebugMessage(-1, TIME, FColor::Green, LOG_CALLINFO + FString(TEXT(" | ")) + FString::Printf(Format, ##__VA_ARGS__))
//TIME : 출력할 시간(float), Format~ : 출력할 텍스트 | 지정된 색상, 시간, 문자열에 따라 화면에 출력하는 매크로(SGS)
#define LOG_SGS_SCREEN(TIME, Format, ...) GEngine->AddOnScreenDebugMessage(-1, TIME, FColor::Cyan, LOG_CALLINFO + FString(TEXT(" | ")) + FString::Printf(Format, ##__VA_ARGS__))
//TIME : 출력할 시간(float), Format~ : 출력할 텍스트 | 지정된 색상, 시간, 문자열에 따라 화면에 출력하는 매크로(KHJ)
#define LOG_KHJ_SCREEN(TIME, Format, ...) GEngine->AddOnScreenDebugMessage(-1, TIME, FColor::Orange, LOG_CALLINFO + FString(TEXT(" | ")) + FString::Printf(Format, ##__VA_ARGS__))
//TIME : 출력할 시간(float), Format~ : 출력할 텍스트 | 지정된 색상, 시간, 문자열에 따라 화면에 출력하는 매크로(Debug)
#define LOG_DEBUG_SCREEN(TIME, Format, ...) GEngine->AddOnScreenDebugMessage(-1, TIME, FColor::Red, LOG_CALLINFO + FString(TEXT(" | ")) + FString::Printf(Format, ##__VA_ARGS__))

//ID 지정을 추가하여 같은 ID로 스크린 출력이 가능하게 함
//ID : 디버그 매세지의 아이디 TIME : 출력할 시간(float), Format~ : 출력할 텍스트 | 지정된 색상, 시간, 문자열에 따라 화면에 출력하는 매크로(PSJ)
#define LOG_PSJ_SCREEN_WITH_ID(ID, TIME, Format, ...) GEngine->AddOnScreenDebugMessage(ID, TIME, FColor::Green, LOG_CALLINFO + FString(TEXT(" | ")) + FString::Printf(Format, ##__VA_ARGS__))
//ID : 디버그 매세지의 아이디 TIME : 출력할 시간(float), Format~ : 출력할 텍스트 | 지정된 색상, 시간, 문자열에 따라 화면에 출력하는 매크로(SGS)
#define LOG_SGS_SCREEN_WITH_ID(ID, TIME, Format, ...) GEngine->AddOnScreenDebugMessage(ID, TIME, FColor::Cyan, LOG_CALLINFO + FString(TEXT(" | ")) + FString::Printf(Format, ##__VA_ARGS__))
//ID : 디버그 매세지의 아이디 TIME : 출력할 시간(float), Format~ : 출력할 텍스트 | 지정된 색상, 시간, 문자열에 따라 화면에 출력하는 매크로(KHJ)
#define LOG_KHJ_SCREEN_WITH_ID(ID, TIME, Format, ...) GEngine->AddOnScreenDebugMessage(ID, TIME, FColor::Orange, LOG_CALLINFO + FString(TEXT(" | ")) + FString::Printf(Format, ##__VA_ARGS__))
//ID : 디버그 매세지의 아이디 TIME : 출력할 시간(float), Format~ : 출력할 텍스트 | 지정된 색상, 시간, 문자열에 따라 화면에 출력하는 매크로(Debug)
#define LOG_DEBUG_SCREEN_WITH_ID(ID, TIME, Format, ...) GEngine->AddOnScreenDebugMessage(ID, TIME, FColor::Red, LOG_CALLINFO + FString(TEXT(" | ")) + FString::Printf(Format, ##__VA_ARGS__))