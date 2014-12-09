#include "pch.h"
#include "Settings.h"

using namespace Platform;

bool Settings::initSettings()
{
	Array<String^>^ keys = ref new Array<String^>{ "OpenWith", "DefaultAudioLanguage", "OpenpickerMode", "LibraryMode","CurrentTheme" };
	Array<String^>^ DefaultValues = ref new Array<String^>{ "Open", "German", "List", "Coverflow","Default" };
	bool success = true;
	int SettingsLength = keys->Length;
	Windows::Foundation::Collections::IPropertySet^ sValues = Windows::Storage::ApplicationData::Current->RoamingSettings->Values;
	for (int i = 0; i < SettingsLength; i++)
	{
		String^key = keys->get(i);
		if (!sValues->HasKey(key))
		{
			String^ dValue = DefaultValues->get(i);
			if (sValues->Insert(key, dValue))
				OutputDebugStringW(L"created settings \n");
			else
			{
				OutputDebugStringW(L"Error\n");
				success = false;
			}
		}
	}
	return success;
}

String^ Settings::getValue(String^ key)
{
	String^ Value = nullptr;
	Windows::Foundation::Collections::IPropertySet^ sValues = Windows::Storage::ApplicationData::Current->RoamingSettings->Values;
	if (sValues->HasKey(key))
		Value = (String^)sValues->Lookup(key)->ToString();
	return Value;
}

int Settings::setValue(String^ key, String^ Value)
{
	int state = 1;
	Windows::Foundation::Collections::IPropertySet^ sValues = Windows::Storage::ApplicationData::Current->RoamingSettings->Values;
	if (sValues->HasKey(key))
	{
		Windows::Storage::ApplicationData::Current->RoamingSettings->Values->Remove(key);
		Windows::Storage::ApplicationData::Current->RoamingSettings->Values->Insert(key, Value);
		state = 0;
	}
	return state;
}

bool Settings::reset()
{
	Windows::Storage::ApplicationData::Current->RoamingSettings->Values->Clear();
	return initSettings();
}