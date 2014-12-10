#pragma once

class Settings
{
public:
	static bool reset();
	static bool initSettings();
	static int setValue(Platform::String^ key, Platform::String^ Value);
	static Platform::String^ getValue(Platform::String^ key);
	
};

