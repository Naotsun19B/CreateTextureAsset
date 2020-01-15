// Fill out your copyright notice in the Description page of Project Settings.

#include "Public/MyBlueprintFunctionLibrary.h"
#include "Engine/Texture2D.h"
#include "Misc/Paths.h"
#include "Misc/FileHelper.h"
#include "Misc/PackageName.h"
#include "AssetRegistryModule.h"
#include "IImageWrapperModule.h"
#include "IImageWrapper.h"

bool UMyBlueprintFunctionLibrary::CreateTextureAsset(const FString& Filename, class UTexture2D* &LoadedTexture)
{
	int Width = 1024;
	int Height = 1024;

	TArray<uint8> Pixels;
	Pixels.SetNum(Height * Width * 4);

	// �s�N�Z���f�[�^�̍쐬
	for (int Y = 0; Y < Height; Y++)
	{
		for (int X = 0; X < Width; X++)
		{
			if (X < Width / 2)
			{
				if (Y < Height / 2)
				{
					// �E��@��
					Pixels[Y * 4 * Width + X * 4 + 0] = 255;	//R
					Pixels[Y * 4 * Width + X * 4 + 1] = 0;		//G
					Pixels[Y * 4 * Width + X * 4 + 2] = 0;		//B
					Pixels[Y * 4 * Width + X * 4 + 3] = 255;	//A
				}
				else
				{
					// �����@��
					Pixels[Y * 4 * Width + X * 4 + 0] = 0;		//R
					Pixels[Y * 4 * Width + X * 4 + 1] = 255;	//G
					Pixels[Y * 4 * Width + X * 4 + 2] = 0;		//B
					Pixels[Y * 4 * Width + X * 4 + 3] = 255;	//A
				}
			}
			else
			{
				if (Y < Height / 2)
				{
					// ����@��
					Pixels[Y * 4 * Width + X * 4 + 0] = 0;		//R
					Pixels[Y * 4 * Width + X * 4 + 1] = 0;		//G
					Pixels[Y * 4 * Width + X * 4 + 2] = 255;	//B
					Pixels[Y * 4 * Width + X * 4 + 3] = 255;	//A
				}
				else
				{
					// �E���@��
					Pixels[Y * 4 * Width + X * 4 + 0] = 255;	//R
					Pixels[Y * 4 * Width + X * 4 + 1] = 255;	//G
					Pixels[Y * 4 * Width + X * 4 + 2] = 255;	//B
					Pixels[Y * 4 * Width + X * 4 + 3] = 255;	//A
				}
			}
		}
	}

	// �p�b�P�[�W���쐬
	FString PackagePath(TEXT("/Game/CreatedTexture/"));
	FString AbsolutePackagePath = FPaths::ProjectContentDir() + TEXT("/CreatedTexture/");
	
	FPackageName::RegisterMountPoint(PackagePath, AbsolutePackagePath);

	PackagePath += Filename;

	UPackage* Package = CreatePackage(nullptr, *PackagePath);
	Package->FullyLoad();

	// �e�N�X�`�����쐬
	FName TextureName = MakeUniqueObjectName(Package, UTexture2D::StaticClass(), FName(*Filename));
	UTexture2D* Texture = NewObject<UTexture2D>(Package, TextureName, RF_Public | RF_Standalone);

	// �e�N�X�`���̐ݒ�
	Texture->PlatformData = new FTexturePlatformData();
	Texture->PlatformData->SizeX = Width;
	Texture->PlatformData->SizeY = Height;
	Texture->PlatformData->PixelFormat = PF_R8G8B8A8;
	
	// �s�N�Z���f�[�^���e�N�X�`���ɏ�������
	FTexture2DMipMap* Mip = new FTexture2DMipMap();
	Texture->PlatformData->Mips.Add(Mip);
	Mip->SizeX = Width;
	Mip->SizeY = Height;
	Mip->BulkData.Lock(LOCK_READ_WRITE);
	uint8* TextureData = (uint8*)Mip->BulkData.Realloc(Height * Width * sizeof(uint8) * 4);
	FMemory::Memcpy(TextureData, Pixels.GetData(), Height * Width * sizeof(uint8) * 4);
	Mip->BulkData.Unlock();
	
	// �e�N�X�`�����X�V
	Texture->AddToRoot();
	Texture->Source.Init(Width, Height, 1, 1, ETextureSourceFormat::TSF_BGRA8, Pixels.GetData());
	Texture->UpdateResource();
	Texture->MarkPackageDirty();
	LoadedTexture = Texture;

	FString PackageFilename = FPackageName::LongPackageNameToFilename(PackagePath, FPackageName::GetAssetPackageExtension());
	return UPackage::SavePackage(Package, Texture, RF_Public | RF_Standalone, *PackageFilename, GError, nullptr, true, true, SAVE_NoError);
}

bool UMyBlueprintFunctionLibrary::LoadTextureAssetFromFile(const FString& FilePath, class UTexture2D* &LoadedTexture)
{
	IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
	TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::JPEG);

	// �摜�f�[�^��ǂݍ���
	TArray<uint8> RawFileData;
	if (FFileHelper::LoadFileToArray(RawFileData, *FilePath))
	{
		// �񈳏k�̉摜�f�[�^���擾
		const TArray<uint8>* UncompressedRawData = nullptr;
		if (ImageWrapper.IsValid() &&
			ImageWrapper->SetCompressed(RawFileData.GetData(), RawFileData.Num()) &&
			ImageWrapper->GetRaw(ERGBFormat::BGRA, 8, UncompressedRawData)
			)
		{
			FString Filename = FPaths::GetBaseFilename(FilePath);
			int Width = ImageWrapper->GetWidth();
			int Height = ImageWrapper->GetHeight();

			// �p�b�P�[�W���쐬
			FString PackagePath(TEXT("/Game/LoadedTexture/"));
			FString AbsolutePackagePath = FPaths::ProjectContentDir() + TEXT("/LoadedTexture/");

			FPackageName::RegisterMountPoint(PackagePath, AbsolutePackagePath);

			PackagePath += Filename;

			UPackage* Package = CreatePackage(nullptr, *PackagePath);
			Package->FullyLoad();

			// �e�N�X�`�����쐬
			FName TextureName = MakeUniqueObjectName(Package, UTexture2D::StaticClass(), FName(*Filename));
			UTexture2D* Texture = NewObject<UTexture2D>(Package, TextureName, RF_Public | RF_Standalone);

			// �e�N�X�`���̐ݒ�
			Texture->PlatformData = new FTexturePlatformData();
			Texture->PlatformData->SizeX = Width;
			Texture->PlatformData->SizeY = Height;
			Texture->MipGenSettings = TextureMipGenSettings::TMGS_NoMipmaps;
			Texture->NeverStream = false;

			// �s�N�Z���f�[�^���e�N�X�`���ɏ�������
			FTexture2DMipMap* Mip = new FTexture2DMipMap();
			Texture->PlatformData->Mips.Add(Mip);
			Mip->SizeX = Width;
			Mip->SizeY = Height;
			Mip->BulkData.Lock(LOCK_READ_WRITE);
			uint8* TextureData = (uint8*)Mip->BulkData.Realloc(UncompressedRawData->Num());
			FMemory::Memcpy(TextureData, UncompressedRawData->GetData(), UncompressedRawData->Num());
			Mip->BulkData.Unlock();

			// �e�N�X�`�����X�V
			Texture->AddToRoot();
			Texture->Source.Init(Width, Height, 1, 1, ETextureSourceFormat::TSF_BGRA8, UncompressedRawData->GetData());
			Texture->UpdateResource();

			// �p�b�P�[�W��ۑ�
			Package->MarkPackageDirty();
			FAssetRegistryModule::AssetCreated(Texture);
			LoadedTexture = Texture;

			FString PackageFilename = FPackageName::LongPackageNameToFilename(PackagePath, FPackageName::GetAssetPackageExtension());
			return UPackage::SavePackage(Package, Texture, RF_Public | RF_Standalone, *PackageFilename, GError, nullptr, true, true, SAVE_NoError);
		}
	}

	return false;
}
