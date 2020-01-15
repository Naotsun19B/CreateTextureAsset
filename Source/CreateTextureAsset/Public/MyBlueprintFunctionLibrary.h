// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MyBlueprintFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class CREATETEXTUREASSET_API UMyBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Test")
	static bool CreateTextureAsset(const FString& Filename, class UTexture2D* &LoadedTexture);

	UFUNCTION(BlueprintCallable, Category = "Test")
	static bool LoadTextureAssetFromFile(const FString& FilePath, class UTexture2D* &LoadedTexture);
};
