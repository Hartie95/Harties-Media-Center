
#include "pch.h"
#include "Hilfsfunctionen.h"
IVectorView<StorageFile^>^ help::addivec(IVectorView<StorageFile^>^ adda,IVectorView<StorageFile^>^ addb)
{
	unsigned int sizea=adda->Size;
	unsigned int sizeb=addb->Size;
	unsigned int endsize=adda->Size+addb->Size;

	Array<StorageFile^>^ filearr=ref new Array<StorageFile^>(endsize);
	unsigned int y=0;
	for(unsigned int i=0;i<sizea;i++)
	{
		if (suported(adda->GetAt(i)->FileType))
		{
			filearr->set(y,adda->GetAt(i));
			y++;
		}
			
	}
	unsigned int a=0;
	for(unsigned int i=0;i<sizeb;i++)
	{
		
		if (suported(addb->GetAt(i)->FileType))
		{
				filearr->set(y,addb->GetAt(i));
				y++;
		}
		a++;
	}
	//////////////////////////////////////////////////////////////////////
	Array<StorageFile^>^ readyfilearr = ref new Array<StorageFile^>(y);
	for (unsigned int i = 0; i < y; i++)
	{
		readyfilearr->set(i, filearr->get(i));
	}
	////////////////////////////////////////////////////////////////////////
	Platform::Collections::VectorView<StorageFile^>^ vectviews =ref new Platform::Collections::VectorView<StorageFile^>(readyfilearr);
	return (IVectorView<StorageFile^>^)vectviews;
};

IVectorView<StorageFile^>^ help::addiIvec(IVectorView<IStorageItem^>^ adda,IVectorView<StorageFile^>^ addb)
{
	if (adda != nullptr&&addb != nullptr)
	{
		unsigned int sizea = adda->Size;
		unsigned int sizeb = addb->Size;
		unsigned int endsize = adda->Size + addb->Size;

		Array<StorageFile^>^ filearr = ref new Array<StorageFile^>(endsize);
		for (unsigned int i = 0; i < sizea; i++)
		{
			filearr->set(i, (StorageFile^)adda->GetAt(i));
		}
		unsigned int a = 0;
		for (unsigned int i = sizea; i < endsize; i++)
		{

			filearr->set(i, addb->GetAt(a));
			a++;
		}

		Platform::Collections::VectorView<StorageFile^>^ vectviews = ref new Platform::Collections::VectorView<StorageFile^>(filearr);
		return (IVectorView<StorageFile^>^)vectviews;
	}
	else if (adda == nullptr)
	{
		unsigned int sizeb = addb->Size;
		Array<StorageFile^>^ filearr = ref new Array<StorageFile^>(sizeb);
		for (unsigned int i = 0; i < sizeb; i++)
		{
			filearr->set(i, addb->GetAt(i));
		}
		Platform::Collections::VectorView<StorageFile^>^ vectviews = ref new Platform::Collections::VectorView<StorageFile^>(filearr);
		return (IVectorView<StorageFile^>^)vectviews;
	}
	else if (addb == nullptr)
	{
		unsigned int sizea = adda->Size;
		Array<StorageFile^>^ filearr = ref new Array<StorageFile^>(sizea);
		for (unsigned int i = 0; i < sizea; i++)
		{
			filearr->set(i,(StorageFile^) adda->GetAt(i));
		}
		Platform::Collections::VectorView<StorageFile^>^ vectviews = ref new Platform::Collections::VectorView<StorageFile^>(filearr);
		return (IVectorView<StorageFile^>^)vectviews;
	}
	else
		return nullptr;
};
IVectorView<StorageFile^>^ help::getSupportedFiles(IVectorView<StorageFile^>^ Files)
{
	unsigned int size = Files->Size;

	Array<StorageFile^>^ filearr = ref new Array<StorageFile^>(size);
	unsigned int y = 0;
	for (unsigned int i = 0; i < size; i++)
	{
		if (suported(Files->GetAt(i)->FileType))
		{
			filearr->set(y, Files->GetAt(i));
			y++;
		}

	}
	//////////////////////////////////////////////////////////////////////
	Array<StorageFile^>^ readyfilearr = ref new Array<StorageFile^>(y);
	for (unsigned int i = 0; i < y; i++)
	{
		readyfilearr->set(i, filearr->get(i));
	}
	////////////////////////////////////////////////////////////////////////
	Platform::Collections::VectorView<StorageFile^>^ vectviews = ref new Platform::Collections::VectorView<StorageFile^>(readyfilearr);
	return (IVectorView<StorageFile^>^)vectviews;

}
String^ help::timetostring(Windows::Foundation::TimeSpan time)
{
	long long dur=time.Duration;
	__int64 temp=dur/10000000;
	__int64 dursec=temp%60;
	__int64 durmin=(temp-dursec)/60;
	String^ dursecs;
	dursecs="";
	String^ durmins=" ";
	if(dursec<10)
		dursecs="0"+dursec;
	else
		dursecs=""+dursec;
	if(durmin<10)
		durmins="0"+durmin;
	else
		durmins=""+durmin;
	return ""+durmins+":"+dursecs;

}
String^ help::timetostring(__int64 time)
{
	long long dur=time;
	__int64 temp=dur;
	__int64 dursec=temp%60;
	__int64 durmin=(temp-dursec)/60;
	String^ dursecs;
	dursecs="";
	String^ durmins=" ";
	if(dursec<10)
		dursecs="0"+dursec;
	else
		dursecs=""+dursec;
	if(durmin<10)
		durmins="0"+durmin;
	else
		durmins=""+durmin;
	return ""+durmins+":"+dursecs;

}
__int64 help::timetosec(Windows::Foundation::TimeSpan time)
{
	long long dur=time.Duration;
	__int64 temp=dur/10000000;
	
	return temp;

}
int64 help::sectotime(__int64 time)
{
	int64 temp=((int64)time)*10000000;
	
	return temp;

}


FileOpenPicker^ help::create_open_picker()
{

	FileOpenPicker^ openPicker = ref new FileOpenPicker();
	if (settings::getValue("OpenpickerMode") == "List")
		openPicker->ViewMode = PickerViewMode::List;
	else
		openPicker->ViewMode = PickerViewMode::Thumbnail;
	openPicker->SuggestedStartLocation = PickerLocationId::VideosLibrary;
	Array<String^>^ supported_files = help::getSupportedFiles();

	for (unsigned int i = 0; i < supported_files->Length; i++)
		openPicker->FileTypeFilter->Append(supported_files->get(i));
	return openPicker;
}
FileOpenPicker^ help::create_open_picker(String^ Openname)
{
	FileOpenPicker^ openPicker = ref new FileOpenPicker();
	if (settings::getValue("OpenpickerMode") == "List")
		openPicker->ViewMode = PickerViewMode::List;
	else
		openPicker->ViewMode = PickerViewMode::Thumbnail;
	openPicker->SuggestedStartLocation = PickerLocationId::VideosLibrary;
	openPicker->CommitButtonText=Openname;
	Array<String^>^ supported_files = help::getSupportedFiles();
	
	for (unsigned int i = 0; i < supported_files->Length; i++)
		openPicker->FileTypeFilter->Append(supported_files->get(i));

	/*openPicker->FileTypeFilter->Append(".3g2");
	openPicker->FileTypeFilter->Append(".3gp2");
	openPicker->FileTypeFilter->Append(".3gp");
	openPicker->FileTypeFilter->Append(".3gpp");
	openPicker->FileTypeFilter->Append(".m4a");
	openPicker->FileTypeFilter->Append(".m4v");
	openPicker->FileTypeFilter->Append(".mp4v");
	openPicker->FileTypeFilter->Append(".mp4");
	openPicker->FileTypeFilter->Append(".mov");
	openPicker->FileTypeFilter->Append(".m2ts");
	openPicker->FileTypeFilter->Append(".asf");
	openPicker->FileTypeFilter->Append(".wm");
	openPicker->FileTypeFilter->Append(".wmv");
	openPicker->FileTypeFilter->Append(".wma");
	openPicker->FileTypeFilter->Append(".aac");
	openPicker->FileTypeFilter->Append(".adt");
	openPicker->FileTypeFilter->Append(".adts");
	openPicker->FileTypeFilter->Append(".mp3");
	openPicker->FileTypeFilter->Append(".wav");
	openPicker->FileTypeFilter->Append(".avi");
	openPicker->FileTypeFilter->Append(".ac3");
	openPicker->FileTypeFilter->Append(".ec3");*/
	
	return openPicker;
}


FolderPicker^ help::create_folder_picker(String^ Openname)
{
	FolderPicker^ FolderPickerP = ref new FolderPicker();
	FolderPickerP->CommitButtonText = Openname;
	FolderPickerP->ViewMode = PickerViewMode::List;
	/*if (settings::getValue("OpenpickerMode") == "List")
		openPicker->ViewMode = PickerViewMode::List;
	else
		openPicker->ViewMode = PickerViewMode::Thumbnail;*/
	FolderPickerP->SuggestedStartLocation = PickerLocationId::ComputerFolder;
	Array<String^>^ supported_files = help::getSupportedFiles();

	for (unsigned int i = 0; i < supported_files->Length; i++)
		FolderPickerP->FileTypeFilter->Append(supported_files->get(i));


	return FolderPickerP;
}

bool help::suported(String^ filetype)
{
	Boolean suported = false;
	//Array<String^>^ suported_audiofiles = ref new Array<String^>{ ".wma", ".aac", ".mp3", ".wav", ".ac3" };
	//Array<String^>^ suported_videofiles = ref new Array<String^>{ ".avi", ".wmv", ".mp4", ".wmv", ".mov", ".3gp", ".mp4v" };
	//Array<String^>^ other_suported_files = ref new Array<String^>{ ".3g2", ".3gp2", ".3gpp", ".m4a", ".m4v", ".m2ts", ".asf", ".wm", ".adt", ".adts", ".ec3" };
	Array<String^>^ supported_files = help::getSupportedFiles();

	for (unsigned int i = 0; i < supported_files->Length; i++)
		if(filetype == supported_files->get(i))
			suported = true;

	/*for (unsigned int i = 0; i < suported_audiofiles->Length; i++)
		if(filetype==suported_audiofiles->get(i))
			suported = true;
	for (unsigned int i = 0; i < suported_videofiles->Length; i++)
		if (filetype == suported_videofiles->get(i))
			suported = true;
	for (unsigned int i = 0; i < other_suported_files->Length; i++)
		if (filetype == other_suported_files->get(i))
			suported = true;
			*/
	return suported;
}

Array<String^>^ help::getSupportedAudio()
{
	return ref new Array<String^>{ ".wma", ".aac", ".mp3", ".wav", ".ac3" };


}

Array<String^>^ help::getSupportedVideo()
{
	return ref new Array<String^>{ ".avi", ".wmv", ".mp4", ".wmv", ".mov", ".3gp", ".mp4v" };


}
Array<String^>^ help::getOtherSupportedFiles()
{
	return ref new Array<String^>{ ".3g2", ".3gp2", ".3gpp", ".m4a", ".m4v", ".m2ts", ".asf", ".wm", ".adt", ".adts", ".ec3" };

}

Array<String^>^ help::getSupportedFiles()
{
	Array<String^>^ audio = help::getSupportedAudio();
	Array<String^>^ video = help::getSupportedVideo();
	Array<String^>^ other = help::getOtherSupportedFiles();
	unsigned int size = audio->Length+video->Length+other->Length;
	Array<String^>^ supportedFiles = ref new Array<String^>(size);
	unsigned int i = 0;
	for (; i < audio->Length; i++)
	{
		supportedFiles->set(i, audio->get(i));
	}
	for (unsigned int x = 0; x < video->Length; x++,i++)
	{
		supportedFiles->set(i, video->get(x));
	}
	for (unsigned int x = 0; x < other->Length; x++ , i++)
	{
		supportedFiles->set(i, other->get(x));
	}
	return supportedFiles;
}




		////////Settings/////////

bool settings::initSettings()
{
	Array<String^>^ keys = ref new Array<String^>{ "OpenWith", "DefaultAudioLanguage", "OpenpickerMode", "LibraryMode" };
	Array<String^>^ DefaultValues = ref new Array<String^>{ "Open", "German", "List", "Coverflow" };
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

String^ settings::getValue(String^ key)
{
	String^ Value = nullptr;
	Windows::Foundation::Collections::IPropertySet^ sValues = Windows::Storage::ApplicationData::Current->RoamingSettings->Values;
	if (sValues->HasKey(key))
		Value = (String^)sValues->Lookup(key)->ToString();
	return Value;
}

int settings::setValue(String^ key, String^ Value)
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

bool settings::reset()
{
	Windows::Storage::ApplicationData::Current->RoamingSettings->Values->Clear();
	return initSettings();
}