#pragma once

using namespace Windows::UI::Xaml::Controls;

class UIManagerShared
{
public:
	UIManagerShared();
	~UIManagerShared();

private:
	//Main
	Grid^ mainGrid;
	//Ad^ mainAd


	//Player
	Grid^	playerGrid;
	AppBar^	headerAppBar;
	AppBar^ bottomAppBar;
	ListView^ playlist;
	Button^ playPauseButton;
	Button^ nextButton;
	Button^ prevButton;
	Button^ modeButton;
	Button^ showPlaylistButton;


	//Library
	Grid^ playerGrid;
	AppBar^	headerAppBar;
	AppBar^ bottomAppBar;
	//Array<Covers^>^ Covers
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


