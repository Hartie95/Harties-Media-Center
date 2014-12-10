//
// SettingsFlyout.xaml.cpp
// Implementierung der SettingsFlyout-Klasse
//

#include "pch.h"
#include "SettingsFlyout.xaml.h"
#include "Settings.h"

using namespace MediacenterUniversal;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::UI::Popups;

#include <ppltasks.h>
#include <collection.h>

using namespace Concurrency;
using namespace Windows::Storage;
// Die Elementvorlage "Einstellungs-Flyout" ist unter http://go.microsoft.com/fwlink/?LinkId=273769 dokumentiert.

MediacenterUniversal::SettingsFlyout::SettingsFlyout()
{
	InitializeComponent();
	String^ currentTheme = Settings::getValue("CurrentTheme");
	StorageFolder^ localFolder = Windows::Storage::ApplicationData::Current->LocalFolder;
	create_task(localFolder->CreateFolderAsync("Themes", Windows::Storage::CreationCollisionOption::OpenIfExists)).then([this, currentTheme](StorageFolder^ themesFolder)
	{
		if (themesFolder != nullptr)
		{
			create_task(themesFolder->GetFoldersAsync()).then([this, currentTheme](Windows::Foundation::Collections::IVectorView<StorageFolder^>^ Folders)
			{
				std::for_each(begin(Folders), end(Folders), [this, currentTheme](StorageFolder^ folder)
				{
					// do stuff
					String^ Name = folder->Name;
					this->themeBox->Items->Append(Name);
					
					if (Name == currentTheme)
						themeBox->SelectedValue = Name;

				});
				this->themeBox->SelectionChanged += ref new Windows::UI::Xaml::Controls::SelectionChangedEventHandler(this, &MediacenterUniversal::SettingsFlyout::themeBox_SelectionChanged);
			});
		}
	});
}


void MediacenterUniversal::SettingsFlyout::themeBox_SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e)
{
	if (init == 0)
	{
		String^ test = themeBox->SelectedValue->ToString();
		Settings::setValue("CurrentTheme", test);
	}
}
