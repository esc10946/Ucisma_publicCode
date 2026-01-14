#pragma once

#include "UCISMA/UCISMA.h"
#include "Enum_CamDirection.generated.h"  

UENUM(BlueprintType)
enum class ECamDirection : uint8
{
    Start UMETA(DisplayName = "Start"),
    Forward UMETA(DisplayName = "Up"),
    Backward UMETA(DisplayName = "Down"),
    Left UMETA(DisplayName = "Left"),
    Right UMETA(DisplayName = "Right")
};