// Copyright 2020-2023 Yvr Technology Co., Ltd. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "Engine/TextureRenderTarget2D.h"
#include "YvrBusinessBPLibrary.generated.h"

USTRUCT(BlueprintType)
struct FVSTCameraFrameItem
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "YvrLibrary")
	int32 FrameNumber;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "YvrLibrary")
	int32 Width;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "YvrLibrary")
	int32 Height;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "YvrLibrary")
	int32 Format;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "YvrLibrary")
	int32 ExposureDuration;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "YvrLibrary")
	int64 SoeTimestamp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "YvrLibrary")
	int64 SoeTimestampQ;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "YvrLibrary")
	int32 Gain;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "YvrLibrary")
	int32 DataSize;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "YvrLibrary")
	TArray<uint8> LeftEyeData;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "YvrLibrary")
	TArray<uint8> RightEyeData;
};

USTRUCT(BlueprintType)
struct FYvrPose
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "YvrLibrary")
	int64 Timestamp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "YvrLibrary")
	FVector Position;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "YvrLibrary")
	FQuat Rotation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "YvrLibrary")
	uint8 Confidence;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "YvrLibrary")
	uint8 WarningFlag;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "YvrLibrary")
	FVector LinearVelocity;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "YvrLibrary")
	FVector LinearAcceleration;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "YvrLibrary")
	FVector AngularVelocity;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "YvrLibrary")
	FVector AngularAcceleration;
	
};

USTRUCT(BlueprintType)
struct FVSTCameraFrame
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "YvrLibrary")
	FVSTCameraFrameItem FrameItem;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "YvrLibrary")
	FYvrPose SixDofPose;
};

UENUM(BlueprintType)
enum class EVSTCameraResolution :uint8
{
	RESOLUTION_660_616,
	RESOLUTION_1320_1232,
	RESOLUTION_2640_2464,
};

UENUM(BlueprintType)
enum class EVSTCameraFrequency :uint8
{
	FREQUENCY_8_HZ,
	FREQUENCY_10_HZ,
	FREQUENCY_24_HZ,
	FREQUENCY_30_HZ,
};

UENUM(BlueprintType)
enum class EVSTCameraFormat :uint8
{
	FORMAT_NV21,
};

UENUM(BlueprintType)
enum class EVSTCameraSource :uint8
{
	SOURCE_LEFT_EYE,
	SOURCE_RIGHT_EYE,
	SOURCE_BOTH_EYES,
};


UENUM(BlueprintType)
enum class EVSTCameraID :uint8
{
	ID_LEFT,
	ID_RIGHT,
	ID_MAX,
};

USTRUCT(BlueprintType)
struct FVSTCameraIntrinsicExtrinsic
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "YvrLibrary")
	FVector2D FocalLength;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "YvrLibrary")
	FVector2D PrincipalPoint;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "YvrLibrary")
	FVector Position;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "YvrLibrary")
	FQuat Rotation;
};

UCLASS()
class UYvrBusinessBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()
public:
	UFUNCTION(BlueprintPure, Category = "YvrLibrary")
	static FString GetDeviceSn();
	UFUNCTION(BlueprintPure, Category = "YvrLibrary")
	static FString GetDeviceModel();
	UFUNCTION(BlueprintPure, Category = "YvrLibrary")
	static FString GetSoftwareVersion();

	// VST
	UFUNCTION(BlueprintCallable, Category = "YvrLibrary")
	static bool OpenVSTCamera();
	UFUNCTION(BlueprintCallable, Category = "YvrLibrary")
	static bool CloseVSTCamera();
	UFUNCTION(BlueprintCallable, Category = "YvrLibrary")
	static bool AcquireVSTCameraFrame(FVSTCameraFrame& OutFrame);
	UFUNCTION(BlueprintCallable, Category = "YvrLibrary")
	static bool SetVSTCameraFrequency(EVSTCameraFrequency Frequency);
	UFUNCTION(BlueprintCallable, Category = "YvrLibrary")
	static bool GetVSTCameraFrequency(EVSTCameraFrequency& OutFrequency);
	UFUNCTION(BlueprintCallable, Category = "YvrLibrary")
	static bool SetVSTCameraResolution(EVSTCameraResolution Resolution);
	UFUNCTION(BlueprintCallable, Category = "YvrLibrary")
	static bool GetVSTCameraResolution(EVSTCameraResolution& OutResolution);
	UFUNCTION(BlueprintCallable, Category = "YvrLibrary")
	static bool SetVSTCameraFormat(EVSTCameraFormat Format);
	UFUNCTION(BlueprintCallable, Category = "YvrLibrary")
	static bool GetVSTCameraFormat(EVSTCameraFormat& OutFormat);
	UFUNCTION(BlueprintCallable, Category = "YvrLibrary")
	static bool SetVSTCameraOutputSource(EVSTCameraSource Source);
	UFUNCTION(BlueprintCallable, Category = "YvrLibrary")
	static bool GetVSTCameraOutputSource(EVSTCameraSource& OutSource);
	UFUNCTION(BlueprintCallable, Category = "YvrLibrary")
	static bool GetVSTCameraIntrinsicExtrinsic(EVSTCameraID ID, FVSTCameraIntrinsicExtrinsic& OutParams);

	// Utils
	UFUNCTION(BlueprintCallable, Category = "YvrLibrary")
	static void UpdateRenderTargetFromRGB(const TArray<uint8>& RawData, int32 Width, int32 Height, UTextureRenderTarget2D* RenderTarget2D, uint8 OverrideAlpha = 255);

	UFUNCTION(BlueprintCallable, Category = "YvrLibrary")
	static void UpdateRenderTargetFromYUVNV21(const TArray<uint8>& RawData, int32 Width, int32 Height, UTextureRenderTarget2D* RenderTarget2D, uint8 OverrideAlpha = 255);
};
