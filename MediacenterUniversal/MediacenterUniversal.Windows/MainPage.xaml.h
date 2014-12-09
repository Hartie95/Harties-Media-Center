//
// MainPage.xaml.h
// Deklaration der MainPage-Klasse
//

#pragma once

#include "MainPage.g.h"
#include "UIManagerShared.h"

using namespace UIManager;

using namespace Windows::UI::ApplicationSettings;

namespace MediacenterUniversal
{
	/// <summary>
	/// Eine leere Seite, die eigenständig verwendet werden kann oder auf die innerhalb eines Rahmens navigiert werden kann.
	/// </summary>
	public ref class MainPage sealed
	{
	public:
		MainPage();

	protected:
		//Settingsflyout
		virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
		
	private:
		UIManagerShared^ uIManager;
		Mediaplayer^ mediaPlayer;
		void copyToThemefolder(String^ fileName, StorageFolder^ themesFolder);
		void checkDefaulThemeFiles(StorageFolder^ defaultThemesFolder);
		void checkDefaulThemeFolder(StorageFolder^ themesFolder);
	
		//Settingsflyout
		void onCommandsRequested(Windows::UI::ApplicationSettings::SettingsPane^ settingsPane, Windows::UI::ApplicationSettings::SettingsPaneCommandsRequestedEventArgs^ e);
		void onSettingsCommand(Windows::UI::Popups::IUICommand^ command);
		Windows::Foundation::EventRegistrationToken commandsRequestedEventRegistrationToken;
		void OnUnloaded(Platform::Object ^sender, Windows::UI::Xaml::RoutedEventArgs ^e);
	};
}
