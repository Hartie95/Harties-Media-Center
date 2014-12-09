#pragma once

#include "Mediaplayer\Mediaplayer.h"

using namespace Windows::UI::Xaml::Controls;
using namespace Windows::Storage;
using namespace MediaPlayer;

namespace UIManager
{
	ref class UIManagerShared sealed
	{
	public:
		UIManagerShared(Grid^ mainGrid, Windows::UI::Xaml::Window^ mainWindow);

		void loadTheme();
		void resizeUI();
		void changeMode(int mode);
		void setTheme();
		

		Mediaplayer^ createMediaPlayer(Windows::System::Display::DisplayRequest^ DisplayNeed);

	private:
		void showPlayer();
		void showLibrary();
		void showConfiguration();
		void appendStyles(Windows::Data::Json::IJsonValue^ value);
		void appendGrids(Windows::Data::Json::IJsonValue^ value);
		void appendGrids(Windows::Data::Json::JsonObject^ jsonObject, String^ gridKey);
		void setMargin(Windows::UI::Xaml::FrameworkElement^ element, Windows::Data::Json::JsonObject^ margin);
		void setMargin(Windows::UI::Xaml::FrameworkElement^ element, int margin);
		void setMargin(Windows::UI::Xaml::FrameworkElement^ element, int vertikal, int horizontal);
		void setMargin(Windows::UI::Xaml::FrameworkElement^ element, int left, int top, int right, int bottom);
		void setSize(Windows::UI::Xaml::FrameworkElement^ element, int width, int height);
		void setForegroundColor(Windows::UI::Xaml::FrameworkElement^ element, String^ color);
		void setVertikalAlign(Windows::UI::Xaml::FrameworkElement^ element, String^ Align);
		void setHorizontalAlign(Windows::UI::Xaml::FrameworkElement^ element, String^ Align);
		void setZIndex(Windows::UI::Xaml::FrameworkElement^ element, int layer);
		void setBackground(String^ key, Windows::Data::Json::JsonObject^ jsonObject);
		void setBrush(Windows::UI::Xaml::Media::Brush^ Brush, String^ elementKey);
		void setBackgroundImage(String^ key, StorageFile^ BackgroundImage);
		void appendTheme();
		void setTheme(String^ Name);
		void setDefaultTheme();
		void setDefaultValues(Windows::UI::Xaml::Controls::Button^ element);

		String^ isType(String^ key);

		Windows::UI::Color getColorFromString(String^ colorString);
		Windows::UI::Xaml::FrameworkElement^ getElement(String^ name);

		void UIManagerShared::playlistButtonTapped(Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e);

		//Main
		Windows::UI::Xaml::Window^ mainWindow;
		StorageFolder^ themeFolder;
		StorageFile^ themeFile;
		Windows::Data::Json::JsonObject^ themeObject;
		StorageFile^ themeNamesFile;
		int partsVertical = 10; 
		int partsHorizontal = 10;

		Grid^ mainGrid;
		//Ad^ mainAd

	   Platform::Collections::Map<String^, Windows::UI::Xaml::FrameworkElement^>^ mediaPlayerUIElements;
	   Platform::Collections::Map<String^, Windows::UI::Xaml::FrameworkElement^>^ libraryUIElements;
		//Player
		Grid^	playerGrid;
		//change to grids?
		AppBar^	headerAppBar;
		AppBar^ bottomAppBar;
		ListView^ playlist;
		Grid^ playerControlesGrid;
		Grid^ headerGrid;
		Button^ playPauseButton;
		Button^ nextButton;
		Button^ prevButton;
		Button^ modeButton;
		TextBlock^ currentPlaybackTime;
		TextBlock^ totalPlaybackTime;
		Slider^	volumeSlider;
		Slider^ timeSlider;
		MediaElement^ player;
		Button^ backButton;
		Button^ showPlaylistButton;
		Button^ openButton;
		TextBlock^ mediaTitle;
		Image^	cover;


		//Library
		Grid^ libraryGrid;
		AppBar^	libHeaderAppBar;
		AppBar^ libHottomAppBar;
		//Array<Covers^>^ Covers

		void OnSizeChanged(Platform::Object ^sender, Windows::UI::Core::WindowSizeChangedEventArgs ^e);
	};
	enum Modes
	{
		Library,
		Player,
		Configuration
	};
};



/*
Frames:
Maingrid
Playergrid
Librarygrid

all:
Events
MediaplayerControlls?
Librarycontrolls?


Maingrid:
Settingsflyout
Ad


Playergrid:
Topbar
Botbar
Player
Playlist
Buttons

Librarygrid:
Botbar
Topbar
Covers[]


other:
themefile


Lists with name as index for Buttons?
Hide Playlist on player in Library
*/


