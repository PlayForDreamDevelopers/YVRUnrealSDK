// Copyright 2020-2024 Yvr Technology Co., Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Delegates/DelegateCombinations.h"
#include "YvrXRCore.h"

#include "YvrXRMRTypes.generated.h"

#define YVRXR_UUID_SIZE 16
#define YVRXR_SUCCESS(Result) ((Result) >= 0)
#define YVRXR_FAILURE(Result) ((Result) < 0)

UENUM(BlueprintType)
enum EYvrResult
{
	Success = 0,
	TimeoutExpired = 1,

	Error_ValidationFailure = -1,
	Error_RuntimeFailure = -2,
	Error_OutOfMemory = -3,
	Error_APIVersionUnsupported = -4,
	Error_InitializationFailed = -6,
	Error_FunctionUnsupported = -7,
	Error_FeatureUnsupported = -8,
	Error_LimitReached = -10,
	Error_SizeInsufficient = -11,
	Error_HandleInvalid = -12,

	// spatial entity
	Error_ComponentNotSupported,
	Error_ComponentConflict,
	Error_ComponentNotAdded,
	Error_ComponentAdded,

	Error_Unknow
};

UENUM(BlueprintType)
enum class EYvrSpatialSceneDataTypeFlag : uint8
{
	SpatialSceneDataTypeFlag_Locatable = 0,
	SpatialSceneDataTypeFlag_Storable = 1,
	SpatialSceneDataTypeFlag_Sharable = 2,
	SpatialSceneDataTypeFlag_Bounded_2D = 3,
	SpatialSceneDataTypeFlag_Bounded_3D = 4,
	SpatialSceneDataTypeFlag_Semantic_Labels = 5,
	SpatialSceneDataTypeFlag_Room_Layout = 6,
	SpatialSceneDataTypeFlag_Space_Container = 7,
	//SpatialSceneDataTypeFlag_Triangle_Mesh = 1000269000,

	SpatialSceneDataTypeFlag_Max = 255,
};

UENUM(BlueprintType)
enum class EYvrAnchorSceneLabel : uint8
{
	SceneLabel_Unknown		= 0,
	SceneLabel_Floor		= 1,
	SceneLabel_Ceiling		= 2,
	SceneLabel_Wall			= 3,
	SceneLabel_Door			= 4,
	SceneLabel_Window		= 5,
	SceneLabel_Opening		= 6,
	SceneLabel_Table		= 7,
	SceneLabel_Sofa			= 8,
};

UENUM(BlueprintType)
enum class EYvrSaveLocation : uint8
{
	SaveLocation_None		= 0  UMETA(Hidden),
	SaveLocation_Local		= 1,
	SaveLocation_Remote		= 2,
};

UENUM(BlueprintType)
enum class EYvrLoadFilterType : uint8
{
	LoadFilterType_None				= 0,
	LoadFilterType_UUID				= 1,
	LoadFilterType_SpatialSceneData = 2
};

USTRUCT(BlueprintType)
struct YVRXRMR_API FYvrAnchor
{
	GENERATED_BODY()

	uint64_t Value;

	operator uint64_t() const { return Value; }

	FYvrAnchor() { this->Value = 0; }
	FYvrAnchor(const uint64_t& NewValue) { this->Value = NewValue; }
	FYvrAnchor(const FYvrAnchor& Anchor) { this->Value = Anchor.GetValue(); }

	bool operator==(const FYvrAnchor& Other) const
	{
		return Other.GetValue() == Value;
	}

	bool IsValid() const { return Value > 0; }

	uint64_t GetValue() const { return Value; }

	FString ToString() const
	{
		return LexToString((uint64)Value);
	}
};

FORCEINLINE uint32 GetTypeHash(const FYvrAnchor& Anchor)
{
	uint64 Origin = (uint64)Anchor.GetValue();
	uint32 A = Origin & 0xffffffff;
	uint32 B = (Origin >> 32) & 0xffffffff;
	uint32 Hash = HashCombine(A, B);
	return Hash;
}

template<>
struct TStructOpsTypeTraits<FYvrAnchor> : public TStructOpsTypeTraitsBase2<FYvrAnchor>
{
	enum
	{
		WithIdenticalViaEquality = true,
	};
};

USTRUCT(BlueprintType)
struct YVRXRMR_API FYvrAnchorUUID
{
	GENERATED_BODY()

	FYvrAnchorUUID()
	{
		FMemory::Memzero(UUIDArray, YVRXR_UUID_SIZE);
	}

	FYvrAnchorUUID(const uint8_t(&NewUUIDArray)[YVRXR_UUID_SIZE])
	{
		FMemory::Memcpy(UUIDArray, NewUUIDArray);
	}

	bool operator==(const FYvrAnchorUUID& UUID) const
	{
		return IsEqual(UUID);
	}

	bool operator!=(const FYvrAnchorUUID& UUID) const
	{
		return !IsEqual(UUID);
	}

	bool IsValid() const
	{
		static uint8_t ZeroUUID[YVRXR_UUID_SIZE] = { 0 };
		return FMemory::Memcmp(UUIDArray, ZeroUUID, YVRXR_UUID_SIZE) != 0;
	}

	bool IsEqual(const FYvrAnchorUUID& UUID) const
	{
		return FMemory::Memcmp(UUIDArray, UUID.UUIDArray, YVRXR_UUID_SIZE) == 0;
	}

	FString ToString() const
	{
		FString Result = FormatToHexString(UUIDArray[0]);
		for (int32 Index = 1; Index < YVRXR_UUID_SIZE; ++Index)
		{
			Result += TEXT("-");
			Result += FormatToHexString((uint64)UUIDArray[Index]);
		}
		return Result;
	}

	friend FArchive& operator<<(FArchive& Ar, FYvrAnchorUUID& UUID)
	{
		for (int32 Index = 0; Index < YVRXR_UUID_SIZE; ++Index)
		{
			Ar.Serialize(&UUID.UUIDArray[Index], sizeof(uint8_t));
		}
		return Ar;
	}

	bool Serialize(FArchive& Ar)
	{
		Ar << *this;
		return true;
	}

	uint8_t UUIDArray[YVRXR_UUID_SIZE];
};

FORCEINLINE uint32 GetTypeHash(const FYvrAnchorUUID& AnchorUUID)
{
	uint32 Hash = 0;
	for (int32 Index = 0; Index < YVRXR_UUID_SIZE; ++Index)
	{
		uint64 Origin = (uint64)AnchorUUID.UUIDArray[Index];
		uint32 A = Origin & 0xffffffff;
		uint32 B = (Origin >> 32) & 0xffffffff;
		Hash = HashCombine(Hash, A);
		Hash = HashCombine(Hash, B);
	}
	return Hash;
}

template<>
struct TStructOpsTypeTraits<FYvrAnchorUUID> : public TStructOpsTypeTraitsBase2<FYvrAnchorUUID>
{
	enum
	{
		WithIdenticalViaEquality = true,
		WithSerializer = true,
	};
};

USTRUCT(BlueprintType)
struct YVRXRMR_API FYvrAnchorLoadInfo
{
	GENERATED_BODY()

	FYvrAnchorLoadInfo() :
		LoadMaxCount(1024),
		Timeout(0),
		SaveLocation(EYvrSaveLocation::SaveLocation_Local),
		IncludeSpatialSceneDataTypeFilter(EYvrSpatialSceneDataTypeFlag::SpatialSceneDataTypeFlag_Max),
		ExcludeSpatialSceneDataTypeFilter(EYvrSpatialSceneDataTypeFlag::SpatialSceneDataTypeFlag_Max)
	{}

	UPROPERTY(BlueprintReadWrite, Category = "YvrXRLibrary|MR")
	int LoadMaxCount;

	UPROPERTY(BlueprintReadWrite, Category = "YvrXRLibrary|MR")
	float Timeout;

	UPROPERTY(BlueprintReadWrite, Category = "YvrXRLibrary|MR")
	EYvrSaveLocation SaveLocation;

	UPROPERTY(BlueprintReadWrite, Category = "YvrXRLibrary|MR")
	TArray<FYvrAnchorUUID> IncludeUUIDFilter;

	UPROPERTY(BlueprintReadWrite, Category = "YvrXRLibrary|MR")
	TArray<FYvrAnchorUUID> ExcludeUUIDFilter;

	UPROPERTY(BlueprintReadWrite, Category = "YvrXRLibrary|MR")
	EYvrSpatialSceneDataTypeFlag IncludeSpatialSceneDataTypeFilter;

	UPROPERTY(BlueprintReadWrite, Category = "YvrXRLibrary|MR")
	EYvrSpatialSceneDataTypeFlag ExcludeSpatialSceneDataTypeFilter;
};

USTRUCT(BlueprintType)
struct YVRXRMR_API FAnchorLoadResult
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "YvrXRLibrary|MR")
	FYvrAnchor AnchorHandle;

	UPROPERTY(BlueprintReadWrite, Category = "YvrXRLibrary|MR")
	FYvrAnchorUUID AnchorUUID;
};
