#pragma once
using namespace Platform;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::Storage;
using namespace Windows::Foundation::Collections;

#include "Hilfsfunctionen.h"

namespace MediaPlayer
{

	ref class Mediaplayer sealed
 	{
	public:
		Mediaplayer(MediaElement^ Player, Button^ PPButton, Button^ ModeButton, IVectorView<StorageFile^>^ MediaFiles, ListView^ Playlist, TextBlock^ MediaTitle, Image^ Cover, Border^ StateBox, Windows::System::Display::DisplayRequest^ DisplayNeed);
		//~Mediaplayer(void);

		Mediaplayer(MediaElement^ Player, Windows::System::Display::DisplayRequest^ DisplayNeed);

		//Setter
		void setMediaElement(MediaElement^ Player);
		//Buttons
		void setPlayPauseButton(Button^ ppButton);
		void setNextButton(Button^ nextButton);
		void setPrevButton(Button^ PrevButton);
		void setModeButton(Button^ ModeButton);
		void setTitelBox(TextBlock^ MediaTitle);
		void setCover(Image^ Cover);
		void setActionOutput(TextBlock^ State, Border^ StateBox);
		void setPlaylist(ListView^ Playlist);
		void setTimeSlider(Slider^ Slider);
		void setTimeText(TextBlock^ CurrentTimeText, TextBlock^ TotalTimeText);
		void setVolumeSlider(Slider^ VolumeSlider);

		void PlayPause();
		void Play();
		void Pause();
		void Stop();
		void Next();
		void Next(bool manual);
		void Previous();
		void AutoNext();
		void FastForward();
		void Backward();
		void PlayTrack(unsigned int Track);
		unsigned int GetMode();
		String^ GetModeString();
		void SwitchMode();
		void SetMode(String^ Mode);
		void SelectAudio(int);
		void SelectAudio();
		void JumpTo(__int64 Time);
		__int64 GetActTime();
		__int64 GetTotTime();
		double GetVolume();
		void SetVolume(double Volume);
		void SelectCurrentTitelAtPlaylist();
		Windows::Media::MediaPlaybackType GetPlaybackType();
		void OpenFiles();
		void OpenFiles(IVectorView<StorageFile^>^ toAdd);
		void AddFiles();
		void AddFiles(IVectorView<StorageFile^>^ toAdd);
		void openContainingFolder();
		void MediaOpend();
		StorageFile^ GetCurrentFile();
		void openFolderSub(StorageFolder^);

		void test();
		//EventHandler//
		void MediaOpened(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void MediaPlaybackFailed(Platform::Object^ sender, Windows::UI::Xaml::ExceptionRoutedEventArgs^ e);
		void MediaEnded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void TimeSliderTimerHandler(Platform::Object^ sender, Platform::Object^ e);
		//ButtonHandler
		void opentapped(Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e);
		void moreTapped(Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e);
		void PlayPauseTapped(Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e);
		void PrevTapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e);
		void NextTapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e);
		void NextHolding(Platform::Object^ sender, Windows::UI::Xaml::Input::HoldingRoutedEventArgs^ e);
		void PrevHolding(Platform::Object^ sender, Windows::UI::Xaml::Input::HoldingRoutedEventArgs^ e);
		void ModeButtonTapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e);

		void TimeSliderManipulationStarted(Platform::Object^ sender, Windows::UI::Xaml::Input::ManipulationStartedRoutedEventArgs^ e);
		void TimeSliderManipulationCompleted(Platform::Object^ sender, Windows::UI::Xaml::Input::ManipulationCompletedRoutedEventArgs^ e);
		void TimeSliderValueChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs^ e);
		
		void PlaylistSelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e);
		void VolumeChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs^ e);
		void VolumePointerWheelChanged(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e);


	private:
		void init(MediaElement^ Player, Windows::System::Display::DisplayRequest^ DisplayNeed);
		void initSystemMediaControls();
		void initTimers();
		void openstream();;
		void SetModeButtonValue();
		void setStateBox(Border^ StateBox);
		void RefreshPlaylist();
		void SelectTrack();
		void SetTitles(Platform::Object^ sender, Platform::Object^ e);
		void getTitle();
		void HideInfo(Platform::Object^ sender, Platform::Object^ e);
		void MediaButtonPressed(Windows::Media::SystemMediaTransportControls^ sender, Windows::Media::SystemMediaTransportControlsButtonPressedEventArgs^ args);
		void playerCurrentStateChanged(Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		Windows::Media::MediaPlaybackType GetMediaTypeFromFileContentType(StorageFile^ file);
		void showState(String^ infotext);
		void setBitmapAsCover(StorageFile^ CoverFile);

		void addSubfolder(StorageFolder^ folder);

		bool autoplay;
		unsigned int acttrack;
		MediaElement^ Player;
		Button^ PPButton;
		Button^ NextButton;
		Button^ PrevButton;
		Button^ ModeButton;
		ListView^ Playlist;
		TextBlock^ MediaTitle;
		Image^ Cover;
		TextBlock^ State;
		Border^ StateBox;
		IVectorView<StorageFile^>^ MediaFiles;
		Windows::System::Display::DisplayRequest^ DisplayNeed;
		Slider^ TimeSlider;
		TextBlock^ CurrentTimeText;
		TextBlock^ TotalTimeText;
		Slider^ VolumeSlider;

		short int mode;
		bool DisplayNeedState;
		bool ready;
		unsigned int globalcounter;
		Windows::UI::Xaml::DispatcherTimer^ playlisttimer;
		Windows::UI::Xaml::DispatcherTimer^ Infotimer;
		Windows::UI::Xaml::DispatcherTimer^ TimeSliderTimer;

		String^ Title;
		Platform::Agile<Windows::UI::Core::CoreWindow> cw;

		Windows::Media::SystemMediaTransportControls^ smtc;

		

		//Cover Reading
		Windows::Storage::Streams::IInputStream^ inputstream;
		Windows::Storage::Streams::DataReader^ reader;
		__int64 thumbSize;
		Array<unsigned char>^ buffer;
		String^thumbnailname;

		//Playlist animation
		void Item_PointerEntered(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e);
		void Item_PointerExited(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e);
		Windows::UI::Xaml::Media::Animation::Storyboard^ PLAniS;
		Windows::UI::Xaml::Media::Animation::DoubleAnimation^ PLAni;



		int lastmrpos = 0;
		
	};
}

