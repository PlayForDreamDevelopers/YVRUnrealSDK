#include "OnlineSubsystemYvrWrapper.h"

#if PLATFORM_ANDROID
bool OnlineSubsystemYvrWrapper::InitPlatformSDK(long AppId)
{
	bool Ret = false;
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "initYvrPlatformSdk", "(J)Z", false);
		Ret = FJavaWrapper::CallBooleanMethod(Env, FJavaWrapper::GameActivityThis, Method, AppId);
	}
	return Ret;
}

jobject OnlineSubsystemYvrWrapper::PopMessage()
{
	jobject Ret = nullptr;
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "yvr_PopMessage", "()Ljava/lang/Object;", false);
		Ret = FJavaWrapper::CallObjectMethod(Env, FJavaWrapper::GameActivityThis, Method);
	}
	return Ret;
}


int OnlineSubsystemYvrWrapper::GetRequestId(jobject Obj)
{
	int Ret = 0;
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "yvr_getRequestId", "(Ljava/lang/Object;)I", false);
		Ret = FJavaWrapper::CallIntMethod(Env, FJavaWrapper::GameActivityThis, Method, Obj);
	}
	return Ret;
}

FString OnlineSubsystemYvrWrapper::GetRequestType(jobject Obj)
{
	FString Ret = FString("");
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "yvr_getRequestType", "(Ljava/lang/Object;)Ljava/lang/String;", false);
		jstring JavaString = (jstring)FJavaWrapper::CallObjectMethod(Env, FJavaWrapper::GameActivityThis, Method, Obj);
		if (JavaString != NULL)
		{
			const char* JavaChars = Env->GetStringUTFChars(JavaString, 0);
			Ret = FString(UTF8_TO_TCHAR(JavaChars));
			Env->ReleaseStringUTFChars(JavaString, JavaChars);
			Env->DeleteLocalRef(JavaString);
		}
	}
	return Ret;
}

bool OnlineSubsystemYvrWrapper::IsMessageError(jobject Obj)
{
	bool Ret = false;
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "yvr_isError", "(Ljava/lang/Object;)Z", false);
		Ret = FJavaWrapper::CallBooleanMethod(Env, FJavaWrapper::GameActivityThis, Method, Obj);
	}
	return Ret;
}

FString OnlineSubsystemYvrWrapper::GetErrorMessage(jobject Obj)
{
	FString Ret = FString("");
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "yvr_getErrorMsg", "(Ljava/lang/Object;)Ljava/lang/String;", false);
		jstring JavaString = (jstring)FJavaWrapper::CallObjectMethod(Env, FJavaWrapper::GameActivityThis, Method, Obj);
		if (JavaString != NULL)
		{
			const char* JavaChars = Env->GetStringUTFChars(JavaString, 0);
			Ret = FString(UTF8_TO_TCHAR(JavaChars));
			Env->ReleaseStringUTFChars(JavaString, JavaChars);
			Env->DeleteLocalRef(JavaString);
		}
	}
	return Ret;
}

int OnlineSubsystemYvrWrapper::GetErrorCode(jobject Obj)
{
	int Ret = 0;
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "yvr_getErrorCode", "(Ljava/lang/Object;)I", false);
		Ret = FJavaWrapper::CallIntMethod(Env, FJavaWrapper::GameActivityThis, Method, Obj);
	}
	return Ret;
}

void OnlineSubsystemYvrWrapper::FreeRequest(int RequestId)
{
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "yvr_freeRequest", "(I)V", false);
		FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis, Method, RequestId);
	}
}

int OnlineSubsystemYvrWrapper::GetLoggedInUser()
{
	int Ret = 0;
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "yvr_user_GetLoggedInUser", "()I", false);
		Ret = FJavaWrapper::CallIntMethod(Env, FJavaWrapper::GameActivityThis, Method);
	}
	return Ret;
}

int OnlineSubsystemYvrWrapper::GetAccountId(jobject Obj)
{
	int Ret = 0;
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "getYvrAccountId", "(Ljava/lang/Object;)I", false);
		Ret = FJavaWrapper::CallIntMethod(Env, FJavaWrapper::GameActivityThis, Method, Obj);
	}
	return Ret;
}

FString OnlineSubsystemYvrWrapper::GetUserName(jobject Obj)
{
	FString Ret = FString("");
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "getYvrUserName", "(Ljava/lang/Object;)Ljava/lang/String;", false);
		jstring JavaString = (jstring)FJavaWrapper::CallObjectMethod(Env, FJavaWrapper::GameActivityThis, Method, Obj);
		if (JavaString != NULL)
		{
			const char* JavaChars = Env->GetStringUTFChars(JavaString, 0);
			Ret = FString(UTF8_TO_TCHAR(JavaChars));
			Env->ReleaseStringUTFChars(JavaString, JavaChars);
			Env->DeleteLocalRef(JavaString);
		}
	}
	return Ret;
}

FString OnlineSubsystemYvrWrapper::GetUserIcon(jobject Obj)
{
	FString Ret = FString("");
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "getYvrUserIcon", "(Ljava/lang/Object;)Ljava/lang/String;", false);
		jstring JavaString = (jstring)FJavaWrapper::CallObjectMethod(Env, FJavaWrapper::GameActivityThis, Method, Obj);
		if (JavaString != NULL)
		{
			const char* JavaChars = Env->GetStringUTFChars(JavaString, 0);
			Ret = FString(UTF8_TO_TCHAR(JavaChars));
			Env->ReleaseStringUTFChars(JavaString, JavaChars);
			Env->DeleteLocalRef(JavaString);
		}
	}
	return Ret;
}

int OnlineSubsystemYvrWrapper::GetUserSex(jobject Obj)
{
	int Ret = 0;
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "getYvrUserSex", "(Ljava/lang/Object;)I", false);
		Ret = FJavaWrapper::CallIntMethod(Env, FJavaWrapper::GameActivityThis, Method, Obj);
	}
	return Ret;
}

int OnlineSubsystemYvrWrapper::AddCount(FString Name, long Count)
{
	int Ret = 0;
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		jstring AchievementName = Env->NewStringUTF(TCHAR_TO_UTF8(*Name));
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "addCount", "(Ljava/lang/String;J)I", false);
		Ret = FJavaWrapper::CallIntMethod(Env, FJavaWrapper::GameActivityThis, Method, AchievementName, Count);
		Env->DeleteLocalRef(AchievementName);
	}
	return Ret;
}

int OnlineSubsystemYvrWrapper::AddFields(FString Name, FString Fields)
{
	int Ret = 0;
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		jstring AchievementName = Env->NewStringUTF(TCHAR_TO_UTF8(*Name));
		jstring AchievementFields = Env->NewStringUTF(TCHAR_TO_UTF8(*Fields));
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "addFields", "(Ljava/lang/String;Ljava/lang/String;)I", false);
		Ret = FJavaWrapper::CallIntMethod(Env, FJavaWrapper::GameActivityThis, Method, AchievementName, AchievementFields);
		Env->DeleteLocalRef(AchievementName);
		Env->DeleteLocalRef(AchievementFields);

	}
	return Ret;
}

int OnlineSubsystemYvrWrapper::UnlockAchievement(FString Name)
{
	int Ret = 0;
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		jstring AchievementName = Env->NewStringUTF(TCHAR_TO_UTF8(*Name));
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "unlockAchievement", "(Ljava/lang/String;)I", false);
		Ret = FJavaWrapper::CallIntMethod(Env, FJavaWrapper::GameActivityThis, Method, AchievementName);
		Env->DeleteLocalRef(AchievementName);
	}
	return Ret;
}

int OnlineSubsystemYvrWrapper::GetAllDefinitions()
{
	int Ret = 0;
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "getAllDefinitions", "()I", false);
		Ret = FJavaWrapper::CallIntMethod(Env, FJavaWrapper::GameActivityThis, Method);
	}
	return Ret;
}

int OnlineSubsystemYvrWrapper::GetDefinitionByNames(TArray<FString> Names)
{
	int Ret = 0;
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		auto AchievementNames = NewScopedJavaObject(Env, Env->NewObjectArray(Names.Num(), FJavaWrapper::JavaStringClass, NULL));
		if (AchievementNames)
		{
			for (int Index = 0; Index < Names.Num(); ++Index)
			{
				auto StringValue = FJavaHelper::ToJavaString(Env, Names[Index]);
				Env->SetObjectArrayElement(*AchievementNames, Index, *StringValue);
			}
		}

		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "getDefinitionByNames", "([Ljava/lang/String;)I", false);
		Ret = FJavaWrapper::CallIntMethod(Env, FJavaWrapper::GameActivityThis, Method, *AchievementNames);
	}
	return Ret;
}

int OnlineSubsystemYvrWrapper::GetSizeOfAllDefinitions(jobject Obj)
{
	int Ret = 0;
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "getSizeOfAllDefinitions", "(Ljava/lang/Object;)I", false);
		Ret = FJavaWrapper::CallIntMethod(Env, FJavaWrapper::GameActivityThis, Method, Obj);
	}
	return Ret;
}

jobject OnlineSubsystemYvrWrapper::GetElementOfDefinitions(jobject Obj, int Index)
{
	jobject Ret = 0;
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "getElementOfDefinitions", "(Ljava/lang/Object;I)Ljava/lang/Object;", false);
		Ret = FJavaWrapper::CallObjectMethod(Env, FJavaWrapper::GameActivityThis, Method, Obj, Index);
	}
	return Ret;
}

int OnlineSubsystemYvrWrapper::GetIdFromElementOfDefinitions(jobject Obj)
{
	int Ret = 0;
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "getIdFromElementOfDefinitions", "(Ljava/lang/Object;)I", false);
		Ret = FJavaWrapper::CallIntMethod(Env, FJavaWrapper::GameActivityThis, Method, Obj);
	}
	return Ret;
}

FString OnlineSubsystemYvrWrapper::GetApiNameFromElementOfDefinitions(jobject Obj)
{
	FString Ret = FString("");
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "getApiNameFromElementOfDefinitions", "(Ljava/lang/Object;)Ljava/lang/String;", false);
		jstring JavaString = (jstring)FJavaWrapper::CallObjectMethod(Env, FJavaWrapper::GameActivityThis, Method, Obj);
		if (JavaString != NULL)
		{
			const char* JavaChars = Env->GetStringUTFChars(JavaString, 0);
			Ret = FString(UTF8_TO_TCHAR(JavaChars));
			Env->ReleaseStringUTFChars(JavaString, JavaChars);
			Env->DeleteLocalRef(JavaString);
		}
	}
	return Ret;
}

int OnlineSubsystemYvrWrapper::GetAchievementTypeFromElementOfDefinitions(jobject Obj)
{
	int Ret = 0;
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "getAchievementTypeFromElementOfDefinitions", "(Ljava/lang/Object;)I", false);
		Ret = FJavaWrapper::CallIntMethod(Env, FJavaWrapper::GameActivityThis, Method, Obj);
	}
	return Ret;
}

int OnlineSubsystemYvrWrapper::GetPolicyFromElementOfDefinitions(jobject Obj)
{
	int Ret = 0;
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "getPolicyFromElementOfDefinitions", "(Ljava/lang/Object;)I", false);
		Ret = FJavaWrapper::CallIntMethod(Env, FJavaWrapper::GameActivityThis, Method, Obj);
	}
	return Ret;
}

int OnlineSubsystemYvrWrapper::GetTargetFromElementOfDefinitions(jobject Obj)
{
	int Ret = 0;
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "getTargetFromElementOfDefinitions", "(Ljava/lang/Object;)I", false);
		Ret = FJavaWrapper::CallIntMethod(Env, FJavaWrapper::GameActivityThis, Method, Obj);
	}
	return Ret;
}

int OnlineSubsystemYvrWrapper::GetBitfieldLengthFromElementOfDefinitions(jobject Obj)
{
	int Ret = 0;
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "getBitfieldLengthFromElementOfDefinitions", "(Ljava/lang/Object;)I", false);
		Ret = FJavaWrapper::CallIntMethod(Env, FJavaWrapper::GameActivityThis, Method, Obj);
	}
	return Ret;
}

bool OnlineSubsystemYvrWrapper::IsAchievedFromElementOfDefinitions(jobject Obj)
{
	bool Ret = false;
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "getIsAchievedFromElementOfDefinitions", "(Ljava/lang/Object;)Z", false);
		Ret = FJavaWrapper::CallBooleanMethod(Env, FJavaWrapper::GameActivityThis, Method, Obj);
	}
	return Ret;
}

FString OnlineSubsystemYvrWrapper::GetTitleFromElementOfDefinitions(jobject Obj)
{
	FString Ret = FString("");
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "getTitleFromElementOfDefinitions", "(Ljava/lang/Object;)Ljava/lang/String;", false);
		jstring JavaString = (jstring)FJavaWrapper::CallObjectMethod(Env, FJavaWrapper::GameActivityThis, Method, Obj);
		if (JavaString != NULL)
		{
			const char* JavaChars = Env->GetStringUTFChars(JavaString, 0);
			Ret = FString(UTF8_TO_TCHAR(JavaChars));
			Env->ReleaseStringUTFChars(JavaString, JavaChars);
			Env->DeleteLocalRef(JavaString);
		}
	}
	return Ret;
}

FString OnlineSubsystemYvrWrapper::GetDescriptionFromElementOfDefinitions(jobject Obj)
{
	FString Ret = FString("");
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "getDescriptionFromElementOfDefinitions", "(Ljava/lang/Object;)Ljava/lang/String;", false);
		jstring JavaString = (jstring)FJavaWrapper::CallObjectMethod(Env, FJavaWrapper::GameActivityThis, Method, Obj);
		if (JavaString != NULL)
		{
			const char* JavaChars = Env->GetStringUTFChars(JavaString, 0);
			Ret = FString(UTF8_TO_TCHAR(JavaChars));
			Env->ReleaseStringUTFChars(JavaString, JavaChars);
			Env->DeleteLocalRef(JavaString);
		}
	}
	return Ret;
}

FString OnlineSubsystemYvrWrapper::GetUnlockedDescriptionFromElementOfDefinitions(jobject Obj)
{
	FString Ret = FString("");
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "getUnlockedDescriptionFromElementOfDefinitions", "(Ljava/lang/Object;)Ljava/lang/String;", false);
		jstring JavaString = (jstring)FJavaWrapper::CallObjectMethod(Env, FJavaWrapper::GameActivityThis, Method, Obj);
		if (JavaString != NULL)
		{
			const char* JavaChars = Env->GetStringUTFChars(JavaString, 0);
			Ret = FString(UTF8_TO_TCHAR(JavaChars));
			Env->ReleaseStringUTFChars(JavaString, JavaChars);
			Env->DeleteLocalRef(JavaString);
		}
	}
	return Ret;
}

bool OnlineSubsystemYvrWrapper::IsSecretFromElementOfDefinitions(jobject Obj)
{
	bool Ret = false;
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "getIsSecretFromElementOfDefinitions", "(Ljava/lang/Object;)Z", false);
		Ret = FJavaWrapper::CallBooleanMethod(Env, FJavaWrapper::GameActivityThis, Method, Obj);
	}
	return Ret;
}

FString OnlineSubsystemYvrWrapper::GetLockedImageFromElementOfDefinitions(jobject Obj)
{
	FString Ret = FString("");
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "getLockedImageFromElementOfDefinitions", "(Ljava/lang/Object;)Ljava/lang/String;", false);
		jstring JavaString = (jstring)FJavaWrapper::CallObjectMethod(Env, FJavaWrapper::GameActivityThis, Method, Obj);
		if (JavaString != NULL)
		{
			const char* JavaChars = Env->GetStringUTFChars(JavaString, 0);
			Ret = FString(UTF8_TO_TCHAR(JavaChars));
			Env->ReleaseStringUTFChars(JavaString, JavaChars);
			Env->DeleteLocalRef(JavaString);
		}
	}
	return Ret;
}

FString OnlineSubsystemYvrWrapper::GetUnlockedImageFromElementOfDefinitions(jobject Obj)
{
	FString Ret = FString("");
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "getUnlockedImageFromElementOfDefinitions", "(Ljava/lang/Object;)Ljava/lang/String;", false);
		jstring JavaString = (jstring)FJavaWrapper::CallObjectMethod(Env, FJavaWrapper::GameActivityThis, Method, Obj);
		if (JavaString != NULL)
		{
			const char* JavaChars = Env->GetStringUTFChars(JavaString, 0);
			Ret = FString(UTF8_TO_TCHAR(JavaChars));
			Env->ReleaseStringUTFChars(JavaString, JavaChars);
			Env->DeleteLocalRef(JavaString);
		}
	}
	return Ret;
}

long OnlineSubsystemYvrWrapper::GetCreatedTimeFromElementOfDefinitions(jobject Obj)
{
	long Ret = 0;
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "getCreatedTimeFromElementOfDefinitions", "(Ljava/lang/Object;)J", false);
		Ret = FJavaWrapper::CallLongMethod(Env, FJavaWrapper::GameActivityThis, Method, Obj);
	}
	return Ret;
}

long OnlineSubsystemYvrWrapper::GetUpdateTimeFromElementOfDefinitions(jobject Obj)
{
	long Ret = 0;
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "getUpdateTimeFromElementOfDefinitions", "(Ljava/lang/Object;)J", false);
		Ret = FJavaWrapper::CallLongMethod(Env, FJavaWrapper::GameActivityThis, Method, Obj);
	}
	return Ret;
}

int OnlineSubsystemYvrWrapper::GetAllProgress()
{
	int Ret = 0;
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "GetAllProgress", "()I", false);
		Ret = FJavaWrapper::CallIntMethod(Env, FJavaWrapper::GameActivityThis, Method);
	}
	return Ret;
}

int OnlineSubsystemYvrWrapper::GetProgressByName(TArray<FString> Names)
{
	int Ret = 0;
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		auto AchievementNames = NewScopedJavaObject(Env, Env->NewObjectArray(Names.Num(), FJavaWrapper::JavaStringClass, NULL));
		if (AchievementNames)
		{
			for (int Index = 0; Index < Names.Num(); ++Index)
			{
				auto StringValue = FJavaHelper::ToJavaString(Env, Names[Index]);
				Env->SetObjectArrayElement(*AchievementNames, Index, *StringValue);
			}
		}

		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "GetProgressByName", "([Ljava/lang/String;)I", false);
		Ret = FJavaWrapper::CallIntMethod(Env, FJavaWrapper::GameActivityThis, Method, *AchievementNames);
	}
	return Ret;
}

int OnlineSubsystemYvrWrapper::GetSizeOfAchievementProgress(jobject Obj)
{
	int Ret = 0;
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "getSizeOfAchievementProgress", "(Ljava/lang/Object;)I", false);
		Ret = FJavaWrapper::CallIntMethod(Env, FJavaWrapper::GameActivityThis, Method, Obj);
	}
	return Ret;
}

jobject OnlineSubsystemYvrWrapper::GetElementOfAchievementProgress(jobject Obj, int Index)
{
	jobject Ret = 0;
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "getElementOfAchievementProgress", "(Ljava/lang/Object;I)Ljava/lang/Object;", false);
		Ret = FJavaWrapper::CallObjectMethod(Env, FJavaWrapper::GameActivityThis, Method, Obj, Index);
	}
	return Ret;
}

int OnlineSubsystemYvrWrapper::GetIdFromElementOfAchievementProgress(jobject Obj)
{
	int Ret = 0;
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "getIdFromElementOfAchievementProgress", "(Ljava/lang/Object;)I", false);
		Ret = FJavaWrapper::CallIntMethod(Env, FJavaWrapper::GameActivityThis, Method, Obj);
	}
	return Ret;
}

jobject OnlineSubsystemYvrWrapper::GetDefinitionFromElementOfAchievementProgress(jobject Obj)
{
	jobject Ret = 0;
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "getDefinitionFromElementOfAchievementProgress", "(Ljava/lang/Object;)Ljava/lang/Object;", false);
		Ret = FJavaWrapper::CallObjectMethod(Env, FJavaWrapper::GameActivityThis, Method, Obj);
	}
	return Ret;
}

FString OnlineSubsystemYvrWrapper::GetNameFromDefinitionOfAchievementProgress(jobject Obj)
{
	FString Ret = FString("");
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "getNameFromDefinitionOfAchievementProgress", "(Ljava/lang/Object;)Ljava/lang/String;", false);
		jstring JavaString = (jstring)FJavaWrapper::CallObjectMethod(Env, FJavaWrapper::GameActivityThis, Method, Obj);
		if (JavaString != NULL)
		{
			const char* JavaChars = Env->GetStringUTFChars(JavaString, 0);
			Ret = FString(UTF8_TO_TCHAR(JavaChars));
			Env->ReleaseStringUTFChars(JavaString, JavaChars);
			Env->DeleteLocalRef(JavaString);
		}
	}
	return Ret;
}

int OnlineSubsystemYvrWrapper::GetTargetFromDefinitionOfAchievementProgress(jobject Obj)
{
	int Ret = 0;
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "getTargetFromDefinitionOfAchievementProgress", "(Ljava/lang/Object;)I", false);
		Ret = FJavaWrapper::CallIntMethod(Env, FJavaWrapper::GameActivityThis, Method, Obj);
	}
	return Ret;
}

int OnlineSubsystemYvrWrapper::GetCountProgressFromElementOfAchievementProgress(jobject Obj)
{
	int Ret = 0;
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "getCountProgressFromElementOfAchievementProgress", "(Ljava/lang/Object;)I", false);
		Ret = FJavaWrapper::CallIntMethod(Env, FJavaWrapper::GameActivityThis, Method, Obj);
	}
	return Ret;
}

FString OnlineSubsystemYvrWrapper::GetBitfieldProgressFromElementOfAchievementProgress(jobject Obj)
{
	FString Ret = FString("");
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "getBitfieldProgressFromElementOfAchievementProgress", "(Ljava/lang/Object;)Ljava/lang/String;", false);
		jstring JavaString = (jstring)FJavaWrapper::CallObjectMethod(Env, FJavaWrapper::GameActivityThis, Method, Obj);
		if (JavaString != NULL)
		{
			const char* JavaChars = Env->GetStringUTFChars(JavaString, 0);
			Ret = FString(UTF8_TO_TCHAR(JavaChars));
			Env->ReleaseStringUTFChars(JavaString, JavaChars);
			Env->DeleteLocalRef(JavaString);
		}
	}
	return Ret;
}

bool OnlineSubsystemYvrWrapper::IsUnlockedFromElementOfAchievementProgress(jobject Obj)
{
	bool Ret = false;
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "isUnlockedFromElementOfAchievementProgress", "(Ljava/lang/Object;)Z", false);
		Ret = FJavaWrapper::CallBooleanMethod(Env, FJavaWrapper::GameActivityThis, Method, Obj);
	}
	return Ret;
}

long OnlineSubsystemYvrWrapper::GetUnlockTimeFromElementOfAchievementProgress(jobject Obj)
{
	long Ret = 0;
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "getUnlockTimeFromElementOfAchievementProgress", "(Ljava/lang/Object;)J", false);
		Ret = FJavaWrapper::CallLongMethod(Env, FJavaWrapper::GameActivityThis, Method, Obj);
	}
	return Ret;
}

int OnlineSubsystemYvrWrapper::GetViewerEntitled()
{
	int Ret = 0;
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "getViewerEntitled", "()I", false);
		Ret = FJavaWrapper::CallIntMethod(Env, FJavaWrapper::GameActivityThis, Method);
	}
	return Ret;
}

bool OnlineSubsystemYvrWrapper::IsViewerEntitled(jobject Obj)
{
	bool Ret = false;
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "isViewerEntitled", "(Ljava/lang/Object;)Z", false);
		Ret = FJavaWrapper::CallBooleanMethod(Env, FJavaWrapper::GameActivityThis, Method, Obj);
	}
	return Ret;
}

int OnlineSubsystemYvrWrapper::GetYvrFriends()
{
	int Ret = 0;
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "getYvrFriends", "()I", false);
		Ret = FJavaWrapper::CallIntMethod(Env, FJavaWrapper::GameActivityThis, Method);
	}
	return Ret;
}

int OnlineSubsystemYvrWrapper::GetFriendsSize(jobject Obj)
{
	int Ret = 0;
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "getFriendsSize", "(Ljava/lang/Object;)I", false);
		Ret = FJavaWrapper::CallIntMethod(Env, FJavaWrapper::GameActivityThis, Method, Obj);
	}
	return Ret;
}

jobject OnlineSubsystemYvrWrapper::GetItemOfFriendsList(jobject Obj, int Index)
{
	jobject Ret = 0;
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "getItemOfFriendsList", "(Ljava/lang/Object;I)Ljava/lang/Object;", false);
		Ret = FJavaWrapper::CallObjectMethod(Env, FJavaWrapper::GameActivityThis, Method, Obj, Index);
	}
	return Ret;
}

int OnlineSubsystemYvrWrapper::GetActIdOfFriendItem(jobject Obj)
{
	int Ret = 0;
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "getActIdOfFriendItem", "(Ljava/lang/Object;)I", false);
		Ret = FJavaWrapper::CallIntMethod(Env, FJavaWrapper::GameActivityThis, Method, Obj);
	}
	return Ret;
}

FString OnlineSubsystemYvrWrapper::GetNickOfFriendItem(jobject Obj)
{
	FString Ret = FString("");
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "getNickOfFriendItem", "(Ljava/lang/Object;)Ljava/lang/String;", false);
		jstring JavaString = (jstring)FJavaWrapper::CallObjectMethod(Env, FJavaWrapper::GameActivityThis, Method, Obj);
		if (JavaString != NULL)
		{
			const char* JavaChars = Env->GetStringUTFChars(JavaString, 0);
			Ret = FString(UTF8_TO_TCHAR(JavaChars));
			Env->ReleaseStringUTFChars(JavaString, JavaChars);
			Env->DeleteLocalRef(JavaString);
		}
	}
	return Ret;
}

int OnlineSubsystemYvrWrapper::GetAgeOfFriendItem(jobject Obj)
{
	int Ret = 0;
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "getAgeOfFriendItem", "(Ljava/lang/Object;)I", false);
		Ret = FJavaWrapper::CallIntMethod(Env, FJavaWrapper::GameActivityThis, Method, Obj);
	}
	return Ret;
}

int OnlineSubsystemYvrWrapper::GetSexOfFriendItem(jobject Obj)
{
	int Ret = 0;
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "getSexOfFriendItem", "(Ljava/lang/Object;)I", false);
		Ret = FJavaWrapper::CallIntMethod(Env, FJavaWrapper::GameActivityThis, Method, Obj);
	}
	return Ret;
}

FString OnlineSubsystemYvrWrapper::GetIconOfFriendItem(jobject Obj)
{
	FString Ret = FString("");
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "getIconOfFriendItem", "(Ljava/lang/Object;)Ljava/lang/String;", false);
		jstring JavaString = (jstring)FJavaWrapper::CallObjectMethod(Env, FJavaWrapper::GameActivityThis, Method, Obj);
		if (JavaString != NULL)
		{
			const char* JavaChars = Env->GetStringUTFChars(JavaString, 0);
			Ret = FString(UTF8_TO_TCHAR(JavaChars));
			Env->ReleaseStringUTFChars(JavaString, JavaChars);
			Env->DeleteLocalRef(JavaString);
		}
	}
	return Ret;
}

int OnlineSubsystemYvrWrapper::GetOnlineOfFriendItem(jobject Obj)
{
	int Ret = 0;
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "getOnlineOfFriendItem", "(Ljava/lang/Object;)I", false);
		Ret = FJavaWrapper::CallIntMethod(Env, FJavaWrapper::GameActivityThis, Method, Obj);
	}
	return Ret;
}

jobject OnlineSubsystemYvrWrapper::GetUsingAppOfFriendItem(jobject Obj)
{
	jobject Ret = 0;
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "getUsingAppOfFriendItem", "(Ljava/lang/Object;)Ljava/lang/Object;", false);
		Ret = FJavaWrapper::CallObjectMethod(Env, FJavaWrapper::GameActivityThis, Method, Obj);
	}
	return Ret;
}

FString OnlineSubsystemYvrWrapper::GetScoverOfUsingApp(jobject Obj)
{
	FString Ret = FString("");
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "getScoverOfUsingApp", "(Ljava/lang/Object;)Ljava/lang/String;", false);
		jstring JavaString = (jstring)FJavaWrapper::CallObjectMethod(Env, FJavaWrapper::GameActivityThis, Method, Obj);
		if (JavaString != NULL)
		{
			const char* JavaChars = Env->GetStringUTFChars(JavaString, 0);
			Ret = FString(UTF8_TO_TCHAR(JavaChars));
			Env->ReleaseStringUTFChars(JavaString, JavaChars);
			Env->DeleteLocalRef(JavaString);
		}
	}
	return Ret;
}

int OnlineSubsystemYvrWrapper::GetTypeOfUsingApp(jobject Obj)
{
	int Ret = 0;
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "getTypeOfUsingApp", "(Ljava/lang/Object;)I", false);
		Ret = FJavaWrapper::CallIntMethod(Env, FJavaWrapper::GameActivityThis, Method, Obj);
	}
	return Ret;
}

FString OnlineSubsystemYvrWrapper::GetPkgOfUsingApp(jobject Obj)
{
	FString Ret = FString("");
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "getPkgOfUsingApp", "(Ljava/lang/Object;)Ljava/lang/String;", false);
		jstring JavaString = (jstring)FJavaWrapper::CallObjectMethod(Env, FJavaWrapper::GameActivityThis, Method, Obj);
		if (JavaString != NULL)
		{
			const char* JavaChars = Env->GetStringUTFChars(JavaString, 0);
			Ret = FString(UTF8_TO_TCHAR(JavaChars));
			Env->ReleaseStringUTFChars(JavaString, JavaChars);
			Env->DeleteLocalRef(JavaString);
		}
	}
	return Ret;
}

FString OnlineSubsystemYvrWrapper::GetNameOfUsingApp(jobject Obj)
{
	FString Ret = FString("");
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "getNameOfUsingApp", "(Ljava/lang/Object;)Ljava/lang/String;", false);
		jstring JavaString = (jstring)FJavaWrapper::CallObjectMethod(Env, FJavaWrapper::GameActivityThis, Method, Obj);
		if (JavaString != NULL)
		{
			const char* JavaChars = Env->GetStringUTFChars(JavaString, 0);
			Ret = FString(UTF8_TO_TCHAR(JavaChars));
			Env->ReleaseStringUTFChars(JavaString, JavaChars);
			Env->DeleteLocalRef(JavaString);
		}
	}
	return Ret;
}

int OnlineSubsystemYvrWrapper::GetYvrFriendInfo(int Index)
{
	int Ret = 0;
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "getYvrFriendInfo", "(I)I", false);
		Ret = FJavaWrapper::CallIntMethod(Env, FJavaWrapper::GameActivityThis, Method, Index);
	}
	return Ret;
}

int OnlineSubsystemYvrWrapper::GetActIdOfUser(jobject Obj)
{
	int Ret = 0;
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "getActIdOfUser", "(Ljava/lang/Object;)I", false);
		Ret = FJavaWrapper::CallIntMethod(Env, FJavaWrapper::GameActivityThis, Method, Obj);
	}
	return Ret;
}

FString OnlineSubsystemYvrWrapper::GetNickOfUser(jobject Obj)
{
	FString Ret = FString("");
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "getNickOfUser", "(Ljava/lang/Object;)Ljava/lang/String;", false);
		jstring JavaString = (jstring)FJavaWrapper::CallObjectMethod(Env, FJavaWrapper::GameActivityThis, Method, Obj);
		if (JavaString != NULL)
		{
			const char* JavaChars = Env->GetStringUTFChars(JavaString, 0);
			Ret = FString(UTF8_TO_TCHAR(JavaChars));
			Env->ReleaseStringUTFChars(JavaString, JavaChars);
			Env->DeleteLocalRef(JavaString);
		}
	}
	return Ret;
}

int OnlineSubsystemYvrWrapper::GetAgeOfUser(jobject Obj)
{
	int Ret = 0;
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "getAgeOfUser", "(Ljava/lang/Object;)I", false);
		Ret = FJavaWrapper::CallIntMethod(Env, FJavaWrapper::GameActivityThis, Method, Obj);
	}
	return Ret;
}

int OnlineSubsystemYvrWrapper::GetSexOfUser(jobject Obj)
{
	int Ret = 0;
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "getSexOfUser", "(Ljava/lang/Object;)I", false);
		Ret = FJavaWrapper::CallIntMethod(Env, FJavaWrapper::GameActivityThis, Method, Obj);
	}
	return Ret;
}

FString OnlineSubsystemYvrWrapper::GetIconOfUser(jobject Obj)
{
	FString Ret = FString("");
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "getIconOfUser", "(Ljava/lang/Object;)Ljava/lang/String;", false);
		jstring JavaString = (jstring)FJavaWrapper::CallObjectMethod(Env, FJavaWrapper::GameActivityThis, Method, Obj);
		if (JavaString != NULL)
		{
			const char* JavaChars = Env->GetStringUTFChars(JavaString, 0);
			Ret = FString(UTF8_TO_TCHAR(JavaChars));
			Env->ReleaseStringUTFChars(JavaString, JavaChars);
			Env->DeleteLocalRef(JavaString);
		}
	}
	return Ret;
}

int OnlineSubsystemYvrWrapper::GetOnlineOfUser(jobject Obj)
{
	int Ret = 0;
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "getOnlineOfUser", "(Ljava/lang/Object;)I", false);
		Ret = FJavaWrapper::CallIntMethod(Env, FJavaWrapper::GameActivityThis, Method, Obj);
	}
	return Ret;
}

int OnlineSubsystemYvrWrapper::WriteLeaderboardItem(FString Name, float Score, void* Data, int DataLength, bool ForceUpdate)
{
	int Ret = 0;
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		jstring LeaderboardName = Env->NewStringUTF(TCHAR_TO_UTF8(*Name));
		jbyteArray LeaderboardData = Env->NewByteArray(DataLength);
		Env->SetByteArrayRegion(LeaderboardData, 0, DataLength, reinterpret_cast<const jbyte*>(Data));

		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "addItem", "(Ljava/lang/String;D[BJI)I", false);
		Ret = FJavaWrapper::CallIntMethod(Env, FJavaWrapper::GameActivityThis, Method, LeaderboardName, (double)Score, LeaderboardData, (long)DataLength, (int)ForceUpdate);
	}
	return Ret;
}

int OnlineSubsystemYvrWrapper::ReadLeaderboardItems(FString Name, long Size, FString FilterType, FString StartAt)
{
	int Ret = 0;

	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		jstring LeaderboardName = Env->NewStringUTF(TCHAR_TO_UTF8(*Name));
		jstring LeaderboardFilterType = Env->NewStringUTF(TCHAR_TO_UTF8(*FilterType));
		jstring LeaderboardStartAt = Env->NewStringUTF(TCHAR_TO_UTF8(*StartAt));

		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "reqLeaderboardList", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;J)I", false);
		Ret = FJavaWrapper::CallIntMethod(Env, FJavaWrapper::GameActivityThis, Method, LeaderboardName, LeaderboardFilterType, LeaderboardStartAt, Size);
	}

	return Ret;
}

jobject OnlineSubsystemYvrWrapper::GetLeaderboardList(jobject Obj)
{
	jobject Ret = 0;
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "getReqLDBLists", "(Ljava/lang/Object;)Ljava/lang/Object;", false);
		Ret = FJavaWrapper::CallObjectMethod(Env, FJavaWrapper::GameActivityThis, Method, Obj);
	}
	return Ret;
}

int OnlineSubsystemYvrWrapper::GetLeaderboardItemSize(jobject Obj)
{
	int Ret = 0;
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "getSizeOfLDBLists", "(Ljava/lang/Object;)I", false);
		Ret = FJavaWrapper::CallIntMethod(Env, FJavaWrapper::GameActivityThis, Method, Obj);
	}
	return Ret;
}

jobject OnlineSubsystemYvrWrapper::GetItemOfLeaderboard(jobject Obj, int Index)
{
	jobject Ret = 0;
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "getItemOfLDBLists", "(Ljava/lang/Object;I)Ljava/lang/Object;", false);
		Ret = FJavaWrapper::CallObjectMethod(Env, FJavaWrapper::GameActivityThis, Method, Obj, Index);
	}
	return Ret;
}

long OnlineSubsystemYvrWrapper::GetLeaderboardUserId(jobject Obj)
{
	long Ret = 0;
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "getLeaderboardUserIdOfCurrLDB", "(Ljava/lang/Object;)J", false);
		Ret = FJavaWrapper::CallLongMethod(Env, FJavaWrapper::GameActivityThis, Method, Obj);
	}
	return Ret;
}

FString OnlineSubsystemYvrWrapper::GetLeaderboardUserNickName(jobject Obj)
{
	FString Ret = FString("");
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "getLeaderboardUserNameOfCurrLDB", "(Ljava/lang/Object;)Ljava/lang/String;", false);
		jstring JavaString = (jstring)FJavaWrapper::CallObjectMethod(Env, FJavaWrapper::GameActivityThis, Method, Obj);
		if (JavaString != NULL)
		{
			const char* JavaChars = Env->GetStringUTFChars(JavaString, 0);
			Ret = FString(UTF8_TO_TCHAR(JavaChars));
			Env->ReleaseStringUTFChars(JavaString, JavaChars);
			Env->DeleteLocalRef(JavaString);
		}
	}
	return Ret;
}

long OnlineSubsystemYvrWrapper::GetLeaderboardRank(jobject Obj)
{
	long Ret = 0;
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "getLeaderboardSortOfCurrLDB", "(Ljava/lang/Object;)J", false);
		Ret = FJavaWrapper::CallLongMethod(Env, FJavaWrapper::GameActivityThis, Method, Obj);
	}
	return Ret;
}

double OnlineSubsystemYvrWrapper::GetLeaderboardScore(jobject Obj)
{
	double Ret = 0.0;
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "getLeaderboardScoreOfCurrLDB", "(Ljava/lang/Object;)D", false);
		Ret = FJavaWrapper::CallDoubleMethod(Env, FJavaWrapper::GameActivityThis, Method, Obj);
	}
	return Ret;
}
int OnlineSubsystemYvrWrapper::GetViewerPurchases()
{
	int Ret = 0;
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "getViewerPurchases", "()I", false);
		Ret = FJavaWrapper::CallIntMethod(Env, FJavaWrapper::GameActivityThis, Method);
	}
	return Ret;
}
int OnlineSubsystemYvrWrapper::GetPurchaseSize(jobject Obj)
{
	int Ret = 0;
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "getPurchaseSize", "(Ljava/lang/Object;)I", false);
		Ret = FJavaWrapper::CallIntMethod(Env, FJavaWrapper::GameActivityThis, Method, Obj);
	}
	return Ret;
}
jobject OnlineSubsystemYvrWrapper::GetPurchaseByIndex(jobject Obj, int Index)
{
	jobject Ret = 0;
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "getPurchaseByIndex", "(Ljava/lang/Object;I)Ljava/lang/Object;", false);
		Ret = FJavaWrapper::CallObjectMethod(Env, FJavaWrapper::GameActivityThis, Method, Obj, Index);
	}
	return Ret;
}
FString OnlineSubsystemYvrWrapper::GetSkuOfPurchase(jobject Obj)
{
	FString Ret = FString("");
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "getSkuOfPurchase", "(Ljava/lang/Object;)Ljava/lang/String;", false);
		jstring JavaString = (jstring)FJavaWrapper::CallObjectMethod(Env, FJavaWrapper::GameActivityThis, Method, Obj);
		if (JavaString != NULL)
		{
			const char* JavaChars = Env->GetStringUTFChars(JavaString, 0);
			Ret = FString(UTF8_TO_TCHAR(JavaChars));
			Env->ReleaseStringUTFChars(JavaString, JavaChars);
			Env->DeleteLocalRef(JavaString);
		}
	}
	return Ret;
}
FString OnlineSubsystemYvrWrapper::GetNameOfPurchase(jobject Obj)
{
	FString Ret = FString("");
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "getNameOfPurchase", "(Ljava/lang/Object;)Ljava/lang/String;", false);
		jstring JavaString = (jstring)FJavaWrapper::CallObjectMethod(Env, FJavaWrapper::GameActivityThis, Method, Obj);
		if (JavaString != NULL)
		{
			const char* JavaChars = Env->GetStringUTFChars(JavaString, 0);
			Ret = FString(UTF8_TO_TCHAR(JavaChars));
			Env->ReleaseStringUTFChars(JavaString, JavaChars);
			Env->DeleteLocalRef(JavaString);
		}
	}
	return Ret;
}
int OnlineSubsystemYvrWrapper::GetTypeOfPurchase(jobject Obj)
{
	int Ret = 0;
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "getTypeOfPurchase", "(Ljava/lang/Object;)I", false);
		Ret = FJavaWrapper::CallIntMethod(Env, FJavaWrapper::GameActivityThis, Method, Obj);
	}
	return Ret;
}
FString OnlineSubsystemYvrWrapper::GetScoverOfPurchase(jobject Obj)
{
	FString Ret = FString("");
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "getScoverOfPurchase", "(Ljava/lang/Object;)Ljava/lang/String;", false);
		jstring JavaString = (jstring)FJavaWrapper::CallObjectMethod(Env, FJavaWrapper::GameActivityThis, Method, Obj);
		if (JavaString != NULL)
		{
			const char* JavaChars = Env->GetStringUTFChars(JavaString, 0);
			Ret = FString(UTF8_TO_TCHAR(JavaChars));
			Env->ReleaseStringUTFChars(JavaString, JavaChars);
			Env->DeleteLocalRef(JavaString);
		}
	}
	return Ret;
}
FString OnlineSubsystemYvrWrapper::GetTradeNoOfPurchase(jobject Obj)
{
	FString Ret = FString("");
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "getTradeNoOfPurchase", "(Ljava/lang/Object;)Ljava/lang/String;", false);
		jstring JavaString = (jstring)FJavaWrapper::CallObjectMethod(Env, FJavaWrapper::GameActivityThis, Method, Obj);
		if (JavaString != NULL)
		{
			const char* JavaChars = Env->GetStringUTFChars(JavaString, 0);
			Ret = FString(UTF8_TO_TCHAR(JavaChars));
			Env->ReleaseStringUTFChars(JavaString, JavaChars);
			Env->DeleteLocalRef(JavaString);
		}
	}
	return Ret;
}
float OnlineSubsystemYvrWrapper::GetPriceOfProduct(jobject Obj)
{
	float Ret = 0;
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "getPriceOfProduct", "(Ljava/lang/Object;)F", false);
		Ret = FJavaWrapper::CallFloatMethod(Env, FJavaWrapper::GameActivityThis, Method, Obj);
	}
	return Ret;
}
jobject OnlineSubsystemYvrWrapper::GetPurchaseByMessage(jobject Obj)
{
	jobject Ret = 0;
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "getPurchaseByMessage", "(Ljava/lang/Object;)Ljava/lang/Object;", false);
		Ret = FJavaWrapper::CallObjectMethod(Env, FJavaWrapper::GameActivityThis, Method, Obj);
	}
	return Ret;
}
int OnlineSubsystemYvrWrapper::GetProductsBySKU(TArray<FString> Skus)
{
	int Ret = 0;
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		auto SkusArray = NewScopedJavaObject(Env, (jobjectArray)Env->NewObjectArray(Skus.Num(), FJavaWrapper::JavaStringClass, NULL));
		if (SkusArray)
		{
			for (uint32 Param = 0; Param < Skus.Num(); Param++)
			{
				auto StringValue = FJavaHelper::ToJavaString(Env, Skus[Param]);
				Env->SetObjectArrayElement(*SkusArray, Param, *StringValue);
			}
		}

		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "getProductsBySKU", "([Ljava/lang/String;)I", false);
		Ret = FJavaWrapper::CallIntMethod(Env, FJavaWrapper::GameActivityThis, Method, *SkusArray);
	}
	return Ret;
}
int OnlineSubsystemYvrWrapper::GetProductSize(jobject Obj)
{
	int Ret = 0;
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "getProductSize", "(Ljava/lang/Object;)I", false);
		Ret = FJavaWrapper::CallIntMethod(Env, FJavaWrapper::GameActivityThis, Method, Obj);
	}
	return Ret;
}
jobject OnlineSubsystemYvrWrapper::GetProductByIndex(jobject Obj, int Index)
{
	jobject Ret = 0;
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "getProductByIndex", "(Ljava/lang/Object;I)Ljava/lang/Object;", false);
		Ret = FJavaWrapper::CallObjectMethod(Env, FJavaWrapper::GameActivityThis, Method, Obj, Index);
	}
	return Ret;
}
FString OnlineSubsystemYvrWrapper::GetSkuOfProduct(jobject Obj)
{
	FString Ret = FString("");
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "getSkuOfProduct", "(Ljava/lang/Object;)Ljava/lang/String;", false);
		jstring JavaString = (jstring)FJavaWrapper::CallObjectMethod(Env, FJavaWrapper::GameActivityThis, Method, Obj);
		if (JavaString != NULL)
		{
			const char* JavaChars = Env->GetStringUTFChars(JavaString, 0);
			Ret = FString(UTF8_TO_TCHAR(JavaChars));
			Env->ReleaseStringUTFChars(JavaString, JavaChars);
			Env->DeleteLocalRef(JavaString);
		}
	}
	return Ret;
}
FString OnlineSubsystemYvrWrapper::GetNameOfProduct(jobject Obj)
{
	FString Ret = FString("");
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "getNameOfProduct", "(Ljava/lang/Object;)Ljava/lang/String;", false);
		jstring JavaString = (jstring)FJavaWrapper::CallObjectMethod(Env, FJavaWrapper::GameActivityThis, Method, Obj);
		if (JavaString != NULL)
		{
			const char* JavaChars = Env->GetStringUTFChars(JavaString, 0);
			Ret = FString(UTF8_TO_TCHAR(JavaChars));
			Env->ReleaseStringUTFChars(JavaString, JavaChars);
			Env->DeleteLocalRef(JavaString);
		}
	}
	return Ret;
}
int OnlineSubsystemYvrWrapper::GetTypeOfProduct(jobject Obj)
{
	int Ret = 0;
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "getTypeOfProduct", "(Ljava/lang/Object;)I", false);
		Ret = FJavaWrapper::CallIntMethod(Env, FJavaWrapper::GameActivityThis, Method, Obj);
	}
	return Ret;
}
FString OnlineSubsystemYvrWrapper::GetScoverOfProduct(jobject Obj)
{
	FString Ret = FString("");
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "getScoverOfProduct", "(Ljava/lang/Object;)Ljava/lang/String;", false);
		jstring JavaString = (jstring)FJavaWrapper::CallObjectMethod(Env, FJavaWrapper::GameActivityThis, Method, Obj);
		if (JavaString != NULL)
		{
			const char* JavaChars = Env->GetStringUTFChars(JavaString, 0);
			Ret = FString(UTF8_TO_TCHAR(JavaChars));
			Env->ReleaseStringUTFChars(JavaString, JavaChars);
			Env->DeleteLocalRef(JavaString);
		}
	}
	return Ret;
}
FString OnlineSubsystemYvrWrapper::GetBriefOfProduct(jobject Obj)
{
	FString Ret = FString("");
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "getBriefOfProduct", "(Ljava/lang/Object;)Ljava/lang/String;", false);
		jstring JavaString = (jstring)FJavaWrapper::CallObjectMethod(Env, FJavaWrapper::GameActivityThis, Method, Obj);
		if (JavaString != NULL)
		{
			const char* JavaChars = Env->GetStringUTFChars(JavaString, 0);
			Ret = FString(UTF8_TO_TCHAR(JavaChars));
			Env->ReleaseStringUTFChars(JavaString, JavaChars);
			Env->DeleteLocalRef(JavaString);
		}
	}
	return Ret;
}
int OnlineSubsystemYvrWrapper::LaunchCheckoutFlow(FString Sku, float Amount)
{
	int Ret = 0;
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		jstring ProductSku = Env->NewStringUTF(TCHAR_TO_UTF8(*Sku));
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "launchCheckoutFlow", "(Ljava/lang/String;F)I", false);
		Ret = FJavaWrapper::CallIntMethod(Env, FJavaWrapper::GameActivityThis, Method, ProductSku, Amount);
		Env->DeleteLocalRef(ProductSku);
	}
	return Ret;
}
int OnlineSubsystemYvrWrapper::ConsumePurchase(FString Sku)
{
	int Ret = 0;
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		jstring ProductSku = Env->NewStringUTF(TCHAR_TO_UTF8(*Sku));
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "consumePurchase", "(Ljava/lang/String;)I", false);
		Ret = FJavaWrapper::CallIntMethod(Env, FJavaWrapper::GameActivityThis, Method, ProductSku);
		Env->DeleteLocalRef(ProductSku);
	}
	return Ret;
}

//Sport
int OnlineSubsystemYvrWrapper::GetUseInfo()
{
	int Ret = 0;
	if(JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "getUserInfo", "()I", false);
		Ret = FJavaWrapper::CallIntMethod(Env, FJavaWrapper::GameActivityThis, Method);
	}
	return Ret;
}
int OnlineSubsystemYvrWrapper::GetGender(jobject Obj)
{
	int Ret = 0;
	if(JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "getGender", "(Ljava/lang/Object;)I", false);
		Ret = FJavaWrapper::CallIntMethod(Env, FJavaWrapper::GameActivityThis, Method, Obj);
	}
	return Ret;
}
int OnlineSubsystemYvrWrapper::GetBirthDay(jobject Obj)
{
	int Ret = 0;
	if(JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "getBirthDay", "(Ljava/lang/Object;)I", false);
		Ret = FJavaWrapper::CallIntMethod(Env, FJavaWrapper::GameActivityThis, Method, Obj);
	}
	return Ret;
}
int OnlineSubsystemYvrWrapper::GetStature(jobject Obj)
{
	int Ret = 0;
	if(JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "getStature", "(Ljava/lang/Object;)I", false);
		Ret = FJavaWrapper::CallIntMethod(Env, FJavaWrapper::GameActivityThis, Method, Obj);
	}
	return Ret;
}
int OnlineSubsystemYvrWrapper::GetWeight(jobject Obj)
{
	int Ret = 0;
	if(JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "getWeight", "(Ljava/lang/Object;)I", false);
		Ret = FJavaWrapper::CallIntMethod(Env, FJavaWrapper::GameActivityThis, Method, Obj);
	}
	return Ret;
}
int OnlineSubsystemYvrWrapper::GetPlanCalorie(jobject Obj)
{
	int Ret = 0;
	if(JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "getPlanCalorie", "(Ljava/lang/Object;)I", false);
		Ret = FJavaWrapper::CallIntMethod(Env, FJavaWrapper::GameActivityThis, Method, Obj);
	}
	return Ret;
}
int OnlineSubsystemYvrWrapper::GetPlanDurationInSeconds(jobject Obj)
{
	int Ret = 0;
	if(JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "getPlanDurationInSeconds", "(Ljava/lang/Object;)I", false);
		Ret = FJavaWrapper::CallIntMethod(Env, FJavaWrapper::GameActivityThis, Method, Obj);
	}
	return Ret;
}
int OnlineSubsystemYvrWrapper::GetDaysPerWeek(jobject Obj)
{
	int Ret = 0;
	if(JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "getDaysPerWeek", "(Ljava/lang/Object;)I", false);
		Ret = FJavaWrapper::CallIntMethod(Env, FJavaWrapper::GameActivityThis, Method, Obj);
	}
	return Ret;
}

int OnlineSubsystemYvrWrapper::GetSummary(long beginTime, long endTime)
{
	int Ret = 0;
	if(JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "getSummary", "(j;j;)I", false);
		Ret = FJavaWrapper::CallIntMethod(Env, FJavaWrapper::GameActivityThis, Method, beginTime, endTime);
	}
	return Ret;
}
int OnlineSubsystemYvrWrapper::GetSummary(FString beginTime, FString endTime)
{
	int Ret = 0;
	if(JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		jstring jst_beginTime = Env->NewStringUTF(TCHAR_TO_UTF8(*beginTime));
		jstring jst_endTime = Env->NewStringUTF(TCHAR_TO_UTF8(*endTime));
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "getSummary", "(Ljava/lang/String;Ljava/lang/String;)I", false);
		Ret = FJavaWrapper::CallIntMethod(Env, FJavaWrapper::GameActivityThis, Method, jst_beginTime, jst_endTime);
	}
	return Ret;
}
long OnlineSubsystemYvrWrapper::GetSummaryDurationInSeconds(jobject Obj)
{
	long Ret = 0;
	if(JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "getSummaryDurationInSeconds", "(Ljava/lang/Object;)j", false);
		Ret = FJavaWrapper::CallIntMethod(Env, FJavaWrapper::GameActivityThis, Method, Obj);
	}
	return Ret;
}
double OnlineSubsystemYvrWrapper::GetSummaryCalorie(jobject Obj)
{
	double Ret = 0.0;
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "getSummaryCalorie", "(Ljava/lang/Object;)D", false);
		Ret = FJavaWrapper::CallDoubleMethod(Env, FJavaWrapper::GameActivityThis, Method, Obj);
	}
	return Ret;
}

int OnlineSubsystemYvrWrapper::GetDailySummary(long beginTime, long endTime)
{
	int Ret = 0;
	if(JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "getDailySummary", "(j;j;)I", false);
		Ret = FJavaWrapper::CallIntMethod(Env, FJavaWrapper::GameActivityThis, Method, beginTime, endTime);
	}
	return Ret;
}
int OnlineSubsystemYvrWrapper::GetDailySummary(FString beginTime, FString endTime)
{
	int Ret = 0;
	if(JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		jstring jst_beginTime = Env->NewStringUTF(TCHAR_TO_UTF8(*beginTime));
		jstring jst_endTime = Env->NewStringUTF(TCHAR_TO_UTF8(*endTime));
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "getDailySummary", "(Ljava/lang/String;Ljava/lang/String;)I", false);
		Ret = FJavaWrapper::CallIntMethod(Env, FJavaWrapper::GameActivityThis, Method, jst_beginTime, jst_endTime);
	}
	return Ret;
}
int OnlineSubsystemYvrWrapper::GetDailySummarySize(jobject Obj)
{
	int Ret = 0;
	if(JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "getDailySummarySize", "(Ljava/lang/Object;)I", false);
		Ret = FJavaWrapper::CallIntMethod(Env, FJavaWrapper::GameActivityThis, Method, Obj);
	}
	return Ret;
}
jobject OnlineSubsystemYvrWrapper::GetDailySummaryByIndex(jobject Obj, int i)
{
	jobject Ret = 0;
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "getDailySummaryByIndex", "(Ljava/lang/Object;I)Ljava/lang/Object;", false);
		Ret = FJavaWrapper::CallObjectMethod(Env, FJavaWrapper::GameActivityThis, Method, Obj, i);
	}
	return Ret;
}
FString OnlineSubsystemYvrWrapper::GetDailySummaryDate(jobject Obj)
{
	FString Ret = FString("");
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "getDailySummaryDate", "(Ljava/lang/Object;)Ljava/lang/String;", false);
		jstring JavaString = (jstring)FJavaWrapper::CallObjectMethod(Env, FJavaWrapper::GameActivityThis, Method, Obj);
		if (JavaString != NULL)
		{
			const char* JavaChars = Env->GetStringUTFChars(JavaString, 0);
			Ret = FString(UTF8_TO_TCHAR(JavaChars));
			Env->ReleaseStringUTFChars(JavaString, JavaChars);
			Env->DeleteLocalRef(JavaString);
		}
	}
	return Ret;
}
int OnlineSubsystemYvrWrapper::GetDailySummaryPlanDurationInSeconds(jobject Obj)
{
	int Ret = 0;
	if(JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "getDailySummaryPlanDurationInSeconds", "(Ljava/lang/Object;)I", false);
		Ret = FJavaWrapper::CallIntMethod(Env, FJavaWrapper::GameActivityThis, Method, Obj);
	}
	return Ret;
}
long OnlineSubsystemYvrWrapper::GetDailySummaryDurationInSeconds(jobject Obj)
{
	long Ret = 0;
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "getDailySummaryDurationInSeconds", "(Ljava/lang/Object;)J", false);
		Ret = FJavaWrapper::CallLongMethod(Env, FJavaWrapper::GameActivityThis, Method, Obj);
	}
	return Ret;
}
double OnlineSubsystemYvrWrapper::GetDailySummaryCalorie(jobject Obj)
{
	double Ret = 0.0;
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "getDailySummaryCalorie", "(Ljava/lang/Object;)D", false);
		Ret = FJavaWrapper::CallDoubleMethod(Env, FJavaWrapper::GameActivityThis, Method, Obj);
	}
	return Ret;
}
double OnlineSubsystemYvrWrapper::GetDailySummaryPlanCalorie(jobject Obj)
{
	double Ret = 0.0;
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "getDailySummaryPlanCalorie", "(Ljava/lang/Object;)D", false);
		Ret = FJavaWrapper::CallDoubleMethod(Env, FJavaWrapper::GameActivityThis, Method, Obj);
	}
	return Ret;
}
#endif