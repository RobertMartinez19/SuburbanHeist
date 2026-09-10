#include "UI/SHHUD.h"
#include "UI/SHHUDWidgetBase.h"
#include "Blueprint/UserWidget.h"

void ASHHUD::BeginPlay()
{
	Super::BeginPlay();

	if (HUDWidgetClass && PlayerOwner)
	{
		HUDWidgetInstance = CreateWidget<USHHUDWidgetBase>(PlayerOwner, HUDWidgetClass);
		if (HUDWidgetInstance)
		{
			HUDWidgetInstance->AddToViewport();
		}
	}
}
