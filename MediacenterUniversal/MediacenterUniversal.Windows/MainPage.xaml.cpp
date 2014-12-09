//
// MainPage.xaml.cpp
// Implementierung der MainPage-Klasse
//

#include "pch.h"
#include "MainPage.xaml.h"

#include "Settings.h"

#include <ppltasks.h>
#include <collection.h>

#include "SettingsFlyout.xaml.h"
using namespace Windows::UI::ApplicationSettings;
using namespace Windows::Foundation;
using namespace Windows::UI::Popups;

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

using namespace Concurrency;


// Die Elementvorlage "Leere Seite" ist unter http://go.microsoft.com/fwlink/?LinkId=234238 dokumentiert.

MainPage::MainPage()
{
	InitializeComponent();
	/*StorageFolder^ localFolder = Windows::Storage::ApplicationData::Current->LocalFolder;
	create_task(localFolder->CreateFolderAsync("Themes", Windows::Storage::CreationCollisionOption::OpenIfExists)).then([this, localFolder](StorageFolder^ themesFolder)
	{
		if (themesFolder != nullptr)
		{
			checkDefaulThemeFolder(themesFolder);
		}
	});*/
	Settings::initSettings();
	this->commandsRequestedEventRegistrationToken = SettingsPane::GetForCurrentView()->CommandsRequested += ref new TypedEventHandler<SettingsPane^, SettingsPaneCommandsRequestedEventArgs^>(this, &MainPage::onCommandsRequested);
	this->uIManager = ref new UIManagerShared(mainGrid, Window::Current);
	this->mediaPlayer = uIManager->createMediaPlayer(ref new Windows::System::Display::DisplayRequest());
	this->uIManager->changeMode(Modes::Player);
	this->uIManager->setTheme();
	this->mediaPlayer->OpenFiles();
}

void MainPage::checkDefaulThemeFolder(StorageFolder^ themesFolder)
{
	create_task(themesFolder->CreateFolderAsync("Default", Windows::Storage::CreationCollisionOption::OpenIfExists)).then([this, themesFolder](StorageFolder^ defaultThemefolder)
	{
		checkDefaulThemeFiles(defaultThemefolder);
	});
}

void MainPage::checkDefaulThemeFiles(StorageFolder^ defaultThemesFolder)
{
	auto getBGFileTask = create_task(defaultThemesFolder->GetFileAsync("Background.jpg"));
	getBGFileTask.then([this, defaultThemesFolder](StorageFile^ BackgroundImage)
	{
		if (BackgroundImage == nullptr)
			{
				copyToThemefolder("Background.jpg", defaultThemesFolder);
			}
	})
		.then([this, defaultThemesFolder](task<void> t)
	{

		try
		{
			t.get();
		}
		catch (Platform::COMException^ e)
		{
			copyToThemefolder("Background.jpg", defaultThemesFolder);
		}

	});
	auto getNoCoverFileTask = create_task(defaultThemesFolder->GetFileAsync("noCover.png"));
	getNoCoverFileTask.then([this, defaultThemesFolder](StorageFile^ BackgroundImage)
	{
		if (BackgroundImage == nullptr)
		{
			copyToThemefolder("noCover.png", defaultThemesFolder);
		}
	})
		.then([this, defaultThemesFolder](task<void> t)
	{
		try
		{
			t.get();
		}
		catch (Platform::COMException^ e)
		{
			copyToThemefolder("noCover.png", defaultThemesFolder);
		}
	});
	auto getThemeFileTask = create_task(defaultThemesFolder->GetFileAsync("theme.json"));
	getThemeFileTask.then([this, defaultThemesFolder](StorageFile^ BackgroundImage)
	{
		if (BackgroundImage == nullptr)
		{
			copyToThemefolder("theme.json", defaultThemesFolder);
		}
	})
		.then([this, defaultThemesFolder](task<void> t)
	{
		try
		{
			t.get();
		}
		catch (Platform::COMException^ e)
		{
			copyToThemefolder("theme.json", defaultThemesFolder);
		}
	});
	auto getTextsFileTask = create_task(defaultThemesFolder->GetFileAsync("texts.json"));
	getTextsFileTask.then([this, defaultThemesFolder](StorageFile^ BackgroundImage)
	{
		if (BackgroundImage == nullptr)
		{
			copyToThemefolder("texts.json", defaultThemesFolder);
		}
	})
		.then([this, defaultThemesFolder](task<void> t)
	{
		try
		{
			t.get();
		}
		catch (Platform::COMException^ e)
		{
			copyToThemefolder("texts.json", defaultThemesFolder);
		}
	});
}

void MainPage::copyToThemefolder(String^ fileName, StorageFolder^ themesFolder)
{
	create_task(StorageFile::GetFileFromApplicationUriAsync(ref new Windows::Foundation::Uri("ms-appx:///Resources/DefaultTheme/"+fileName))).then([this,themesFolder](StorageFile^ File )
	{
		create_task(File->CopyAsync(themesFolder));
	});
}



void MainPage::OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e)
{
	
}

void MainPage::onSettingsCommand(Windows::UI::Popups::IUICommand^ command)
{

	auto mySettings = ref new SettingsFlyout();
	mySettings->Show();
	mySettings->Unloaded += ref new Windows::UI::Xaml::RoutedEventHandler(this, &MediacenterUniversal::MainPage::OnUnloaded);
	
}

void MainPage::onCommandsRequested(SettingsPane^ settingsPane, SettingsPaneCommandsRequestedEventArgs^ e)
{
	UICommandInvokedHandler^ handler = ref new UICommandInvokedHandler(this, &MainPage::onSettingsCommand);

	SettingsCommand^ generalCommand = ref new SettingsCommand("Themes", "Themes", handler);
	e->Request->ApplicationCommands->Append(generalCommand);
}

void MediacenterUniversal::MainPage::OnUnloaded(Platform::Object ^sender, Windows::UI::Xaml::RoutedEventArgs ^e)
{
	this->uIManager->setTheme();
}
