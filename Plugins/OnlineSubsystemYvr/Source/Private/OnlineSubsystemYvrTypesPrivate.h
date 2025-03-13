// Copyright 2020-2021 Yvr Technology Co., Ltd. All Rights Reserved.

#pragma once
#include "OnlineSubsystemYvrModule.h"
#include "CoreMinimal.h"
#include "OnlineSubsystemTypes.h"

class FUniqueNetIdYvr : public FUniqueNetId {
private:
	int ID;

protected:
	bool Compare(const FUniqueNetId& Other) const override
	{
		if (Other.GetType() != GetType())
		{
			return false;
		}

		if (Other.GetSize() != sizeof(int))
		{
			return false;
		}

		return ID == static_cast<const FUniqueNetIdYvr&>(Other).ID;
	}

public:
	/** Default constructor */
	FUniqueNetIdYvr()
	{
		ID = 0;
	}

	FUniqueNetIdYvr(const int& id)
	{
		ID = id;
	}

	FUniqueNetIdYvr(const FString& id)
	{
		ID = FCString::Atoi(*id);
	}

	/**
	* Copy Constructor
	*
	* @param Src the id to copy
	*/
	explicit FUniqueNetIdYvr(const FUniqueNetId& Src)
	{
		if (Src.GetSize() == sizeof(int))
		{
			ID = static_cast<const FUniqueNetIdYvr&>(Src).ID;
		}
	}

	virtual FName GetType() const override
	{
		return YVR_SUBSYSTEM;
	}

	// IOnlinePlatformData

	virtual const uint8* GetBytes() const override
	{
		return reinterpret_cast<const uint8*>(&ID);
	}

	virtual int32 GetSize() const override
	{
		return sizeof(ID);
	}

	virtual bool IsValid() const override
	{
		// Not completely accurate, but safe to assume numbers below this is invalid
		return ID > 100000;
	}

	int GetID() const
	{
		return ID;
	}

	virtual FString ToString() const override
	{
		return FString::Printf(TEXT("%llu"), ID);
	}

	virtual FString ToDebugString() const override
	{
		const FString UniqueNetIdStr = FString::Printf(TEXT("%llu"), ID);
		return TEXT("int:") + OSS_UNIQUEID_REDACT(*this, UniqueNetIdStr);
	}

#if ENGINE_MAJOR_VERSION > 4 && ENGINE_MINOR_VERSION > 0
	virtual uint32 GetTypeHash() const override
	{
		return ::GetTypeHash((uint64)ID);
	}
#else
	friend uint32 GetTypeHash(const FUniqueNetIdYvr& A)
	{
		return GetTypeHash((uint64)A.ID);
	}
#endif

	/** global static instance of invalid (zero) id */
	static const TSharedRef<const FUniqueNetId>& EmptyId()
	{
		static const TSharedRef<const FUniqueNetId> EmptyId(MakeShared<FUniqueNetIdYvr>());
		return EmptyId;
	}
};