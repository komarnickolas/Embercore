// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractionWidget.h"

void UInteractionWidget::SetPercent(float InTimeHeld, float InMaxTimeHeld) {
	this->TimePercent = InTimeHeld / InMaxTimeHeld;
}

void UInteractionWidget::Reset() {
	SetPercent(0, 1);
}
