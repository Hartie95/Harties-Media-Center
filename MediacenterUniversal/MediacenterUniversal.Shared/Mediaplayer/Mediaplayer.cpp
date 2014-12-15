/*
Todo:
Add Event Handler
	Timestring
	maingrid

	Media Failed Handling
	Cleaning of Code
	SelectPlaylist undefined value possible?
Bugs:	
	Thumbnail not right
	First song after open wont be marked in playlist as cur. playing
*/
#include "pch.h"
#include "Mediaplayer.h"

#include <ppltasks.h>
#include <collection.h>


using namespace MediaPlayer;

using namespace Platform;
using namespace Concurrency;
using namespace Windows::Foundation;
using namespace Windows::Storage::FileProperties;
using namespace Windows::Storage::Streams;
using namespace Windows::Media;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml::Media::Animation;


Mediaplayer::Mediaplayer(MediaElement^ Player, Windows::System::Display::DisplayRequest^ DisplayNeed)
{
	init(Player, DisplayNeed);
}

Mediaplayer::Mediaplayer(MediaElement^ Player, Button^ PPButton, Button^ ModeButton, IVectorView<StorageFile^>^ MediaFiles, ListView^ Playlist, TextBlock^ MediaTitle, Image^ Cover, Border^ StateBox, Windows::System::Display::DisplayRequest^ DisplayNeed)
{
	//Todo 
	init(Player, DisplayNeed);
	this->setPlayPauseButton(PPButton);
	this->setModeButton(ModeButton);
	this->setPlaylist(Playlist);
	this->setCover(Cover);
	setStateBox(StateBox);
	//create Setter
	//this->MediaFiles=nu;		
	this-> MediaTitle=MediaTitle;		
	
}

void Mediaplayer::setMediaElement(MediaElement^ Player)
{
	if (Player != nullptr)
	{
		this->Player=Player;
		this->Player->MediaOpened += ref new Windows::UI::Xaml::RoutedEventHandler(this,&Mediaplayer::MediaOpened);
		this->Player->CurrentStateChanged += ref new Windows::UI::Xaml::RoutedEventHandler(this, &Mediaplayer::playerCurrentStateChanged);
		this->Player->MediaFailed += ref new Windows::UI::Xaml::ExceptionRoutedEventHandler(this, &Mediaplayer::MediaPlaybackFailed);
		this->Player->MediaEnded += ref new Windows::UI::Xaml::RoutedEventHandler(this, &Mediaplayer::MediaEnded);
	}
}
void Mediaplayer::setStateBox(Border^ StateBox)
{
	if (StateBox != nullptr)
	{
		this->StateBox = StateBox;
		if (this->StateBox->Child != nullptr)
			this->State = (TextBlock^)StateBox->Child;
		// Todo create TextBlock
	}
}

void Mediaplayer::init(MediaElement^ Player, Windows::System::Display::DisplayRequest^ DisplayNeed)
{
	this->setMediaElement(Player);
	this->DisplayNeed = DisplayNeed;

	this->PPButton = nullptr;
	this->ModeButton = nullptr;
	this->MediaFiles = nullptr;
	this->Playlist = nullptr;
	this->MediaTitle = nullptr;
	this->State = nullptr;
	this->StateBox = nullptr;
	this->Cover = nullptr;
	this->TimeSlider = nullptr;

	this->acttrack = 0;
	this->mode = 0;
	this->autoplay = false;
	this->DisplayNeedState = false;

	cw = Windows::UI::Xaml::Window::Current->CoreWindow;
	

	//this->Player->RightTapped += ref new Windows::UI::Xaml::Input::RightTappedEventHandler(this, &Mediaplayer::player_righttapped);

	this->initSystemMediaControls();
	this->initTimers();
}
void Mediaplayer::initSystemMediaControls()
{
	this->smtc = nullptr;
	smtc = Windows::Media::SystemMediaTransportControls::GetForCurrentView();
	smtc->PlaybackStatus = Windows::Media::MediaPlaybackStatus::Closed;
	smtc->ButtonPressed += ref new TypedEventHandler<SystemMediaTransportControls^, SystemMediaTransportControlsButtonPressedEventArgs^>(this, &Mediaplayer::MediaButtonPressed);
	smtc->IsPlayEnabled = false;
	smtc->IsPauseEnabled = false;
	smtc->IsStopEnabled = false;
}

void Mediaplayer::initTimers()
{
	TimeSpan ts;

	ts.Duration = 100000 * 1;
	this->playlisttimer = ref new Windows::UI::Xaml::DispatcherTimer();
	this->playlisttimer->Tick += ref new EventHandler<Object^>(this, &Mediaplayer::SetTitles);
	this->playlisttimer->Interval = ts;
	
	ts.Duration = 1000000 * 5;
	this->Infotimer = ref new Windows::UI::Xaml::DispatcherTimer();
	this->Infotimer->Tick += ref new EventHandler<Object^>(this, &Mediaplayer::HideInfo);
	this->Infotimer->Interval = ts;

	ts.Duration = (long long)(1000000 * 1.5);
	this->PLAni = ref new DoubleAnimation();
	this->PLAni->From = 0.0;
	this->PLAni->To = 1.0;
	this->PLAni->Duration = ts;

	ts.Duration = 10000 * 250;
	this->TimeSliderTimer = ref new Windows::UI::Xaml::DispatcherTimer;
	this->TimeSliderTimer->Tick += ref new EventHandler<Object^>(this, &Mediaplayer::TimeSliderTimerHandler);
	this->TimeSliderTimer->Interval = ts;
}



//Buttons
void Mediaplayer::setPlayPauseButton(Button^ ppButton)
{
	this->PPButton = ppButton;
	this->PPButton->Tapped += ref new Windows::UI::Xaml::Input::TappedEventHandler(this, &Mediaplayer::PlayPauseTapped);
}
void Mediaplayer::setNextButton(Button^ NextButton)
{
	if (NextButton != nullptr)
	{
		this->NextButton = NextButton;
		this->NextButton->Tapped += ref new Windows::UI::Xaml::Input::TappedEventHandler(this, &Mediaplayer::NextTapped);
		this->NextButton->Holding += ref new Windows::UI::Xaml::Input::HoldingEventHandler(this, &Mediaplayer::NextHolding);
	}
}
void Mediaplayer::setPrevButton(Button^ PrevButton)
{
	if (PrevButton != nullptr)
	{
		this->PrevButton = PrevButton;
		this->PrevButton->Tapped += ref new Windows::UI::Xaml::Input::TappedEventHandler(this, &Mediaplayer::PrevTapped);
		this->PrevButton->Holding += ref new Windows::UI::Xaml::Input::HoldingEventHandler(this, &Mediaplayer::PrevHolding);
	}
}
void Mediaplayer::setModeButton(Button^ ModeButton)
{
	this->ModeButton = ModeButton;
	this->ModeButton->Tapped += ref new Windows::UI::Xaml::Input::TappedEventHandler(this, &Mediaplayer::ModeButtonTapped);
}
void Mediaplayer::setTitelBox(TextBlock^ MediaTitle)
{
	this->MediaTitle = MediaTitle;
}
void Mediaplayer::setCover(Image^ Cover)
{
	this->Cover = Cover;
}
void Mediaplayer::setActionOutput(TextBlock^ State, Border^ StateBox)
{
	this->State = State;
	this->StateBox = StateBox;
}
void Mediaplayer::setPlaylist(ListView^ Playlist)
{
	if (Playlist != nullptr)
	{
		this->Playlist = Playlist;
		this->Playlist->SelectionChanged += ref new Windows::UI::Xaml::Controls::SelectionChangedEventHandler(this, &Mediaplayer::PlaylistSelectionChanged);
	}
}

void Mediaplayer::setTimeSlider(Slider^ Slider)
{
	if (Slider != nullptr)
	{
		this->TimeSlider = Slider;
		this->TimeSlider->ManipulationStarted += ref new Windows::UI::Xaml::Input::ManipulationStartedEventHandler(this, &Mediaplayer::TimeSliderManipulationStarted);
		this->TimeSlider->ManipulationCompleted += ref new Windows::UI::Xaml::Input::ManipulationCompletedEventHandler(this, &Mediaplayer::TimeSliderManipulationCompleted);
		this->TimeSlider->ValueChanged += ref new Windows::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventHandler(this, &Mediaplayer::TimeSliderValueChanged);
	}
}

void Mediaplayer::setTimeText(TextBlock^ CurrentTimeText, TextBlock^ TotalTimeText)
{
	if (CurrentTimeText != nullptr)
		this->CurrentTimeText = CurrentTimeText;
	if (TotalTimeText != nullptr)
		this->TotalTimeText = TotalTimeText;
}

void Mediaplayer::setVolumeSlider(Slider^ VolumeSlider)
{
	if (VolumeSlider != nullptr)
	{
		this->VolumeSlider = VolumeSlider;
		this->VolumeSlider->ValueChanged += ref new Windows::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventHandler(this, &Mediaplayer::VolumeChanged);
		this->VolumeSlider->PointerWheelChanged += ref new Windows::UI::Xaml::Input::PointerEventHandler(this, &MediaPlayer::Mediaplayer::VolumePointerWheelChanged);
	}
}

/*Mediaplayer::~Mediaplayer(void)
{

}*/

void Mediaplayer::PlayPause()
{
	if(Player->CurrentState.ToString()=="Playing")
	{
		this->Pause();
	}
	else
	{
		this->Play();
	}
}

void Mediaplayer::Play()
{
	Player->Play();
	//smtc->IsPauseEnabled = true;
	if (PPButton != nullptr)
	{
		PPButton->Content = L"\ue103";
		Windows::UI::Xaml::Automation::AutomationProperties::SetName(PPButton, "Pause");
	}
	if (this->State != nullptr)
	{
		if (Player->CurrentState.ToString() != "Playing")
		{
			this->State->Text = "Play";
			this->StateBox->Visibility = Windows::UI::Xaml::Visibility::Visible;
			this->Infotimer->Start();
		}
	}

	this->Player->PlaybackRate = 1;

	if(DisplayNeedState==false)
	{
		DisplayNeedState=true;
		DisplayNeed->RequestActive();
	}
}

void Mediaplayer::Pause()
{
	Player->Pause();
	//smtc->IsPlayEnabled = true;
	if (PPButton != nullptr)
	{
		PPButton->Content = L"\xe102";
		Windows::UI::Xaml::Automation::AutomationProperties::SetName(PPButton, "Play");
	}
	if (this->State != nullptr)
	{
		if (Player->CurrentState.ToString() == "Playing")
		{
			this->State->Text = "Pause";
			this->StateBox->Visibility = Windows::UI::Xaml::Visibility::Visible;
			this->Infotimer->Start();
		}
	}

	if(DisplayNeedState==true)
	{
		DisplayNeedState=false;
		DisplayNeed->RequestRelease();
	}
}

void Mediaplayer::Stop()
{
	this->Pause();
	this->Player->Stop();
	this->acttrack=0;
	autoplay=false;
	openstream();
}

void Mediaplayer::Next()
{
	this->Next(false);
}

void Mediaplayer::Next(bool manual)
{
	/*
		0->Normal
		1->Repeat-all
		2->Repeat-one
		3->Shuffel
	*/
	if (this->MediaFiles != nullptr)
	{
		this->Pause();
		this->JumpTo(0);
		bool more = true;
		this->autoplay = true;
		if (this->MediaFiles->Size <= this->acttrack + 1)
		{
			more = false;
		}
		switch (this->mode)
		{

		case 2:
			if (manual == false)
			{
				this->Player->Play();  //Fixes Bug Not Playing at single repeat but only work around
				this->Play();
				break;
			}
		case 1:
		case 0:
			if (more == true)
			{
				this->acttrack++;
				this->openstream();
			}
			else
			{
				if (mode == 0)
					autoplay = false;
				this->acttrack = 0;
				this->openstream();
			}
			break;
		case 3:
			acttrack = rand() % (MediaFiles->Size);
			this->openstream();
			break;
		}
	}
}

void Mediaplayer::Previous()
{
	this->Pause();
	this->JumpTo(0);
	this->autoplay=true;
	if(this->acttrack>0 && this->MediaFiles->Size>0)
	{	
		this->acttrack--;
		this->autoplay=true;
		this->openstream();
	}
	else
	{
		this->acttrack=0;
	}
}

void Mediaplayer::FastForward()
{
	double playbackrate = Player->PlaybackRate;
	if (playbackrate < 32 && playbackrate > 0)
	{
		playbackrate = playbackrate * 2;
		
	}
	else if(playbackrate<1)
		playbackrate=1;
	else
		playbackrate=1;
	if (this->State != nullptr)
	{
		this->State->Text = "FastForward";
		this->StateBox->Visibility = Windows::UI::Xaml::Visibility::Visible;
		this->Infotimer->Start();
	}
	Player->PlaybackRate = playbackrate;
}

void Mediaplayer::Backward()
{
	double playbackrate = Player->PlaybackRate;

	if (playbackrate > -32 && playbackrate <= -1)
	{
		playbackrate = playbackrate * 2;
		if (this->State != nullptr)
		{
			this->State->Text = "Backward";
			this->StateBox->Visibility = Windows::UI::Xaml::Visibility::Visible;
			this->Infotimer->Start();
		}
	}
	/*else if(playbackrate>-1)
		playbackrate=playbackrate-1;*/
	else
		playbackrate=-1;

	Player->PlaybackRate = playbackrate;
}

void Mediaplayer::PlayTrack(unsigned int Track)
{
	if(Track<MediaFiles->Size && Track!=this->acttrack)
	{
		this->autoplay=true;
		this->acttrack=Track;
		this->openstream();
	}
}

unsigned int Mediaplayer::GetMode()
{
	return this->mode;
}

String^ Mediaplayer::GetModeString()
{
	switch(this->mode)
	{
	case 1:
		return "Repeat-All";
		break;
	case 2:
		return "Repeat-Once";
		break;
	case 3:
		return "Shuffle";
		break;
	default:
		return "normal";
		break;
	}
}

void Mediaplayer::SwitchMode()
{
	switch(this->mode)
	{
	case 0 :	
	case 1 :		
	case 2 :	
		mode++;
		break;
	default	:	
		mode=0;
		break;
	}
	this->SetModeButtonValue();
}

void Mediaplayer::SetMode(String^ ModeString)
{
	if(ModeString=="Repeat-All")
	{
		this->mode=1;
	}
	else if(ModeString=="Repeat-Once")
	{
		this->mode=2;
	}
	else if(ModeString=="Shuffle")
	{
		this->mode=3;
	}
	else
	{
		this->mode=0;
	}
	this->SetModeButtonValue();
}

void Mediaplayer::SelectAudio(int selected)
{
	if (this->Player->AudioStreamCount > 1 && this->Player->AudioStreamCount >selected)
	{
		IBox<int>^ temp = ref new Platform::Box<int>(selected);
		this->Player->AudioStreamIndex = temp;
	}
}
void Mediaplayer::SelectAudio()
{
	if (this->Player->AudioStreamCount > 1)
	{
		int actstream = 0;
		if (this->Player->AudioStreamIndex == nullptr)
		{
			actstream = 1;
		}
		else
		{
			actstream = this->Player->AudioStreamIndex->Value;
		}

		if (actstream + 1 < this->Player->AudioStreamCount)
		{
			actstream++;
		}
		else
		{
			actstream = 0;
		}
		SelectAudio(actstream);
	}
}

void Mediaplayer::JumpTo(__int64 time)
{
	TimeSpan ts;
	ts.Duration =(__int64) 10000 * time;
	Player->Position = ts;
}

__int64 Mediaplayer::GetActTime()
{
	__int64 time=0;
	time = Player->Position.Duration / 10000000;
	return time;
}

__int64 Mediaplayer::GetTotTime()
{
	__int64 time=0;
	time = Player->NaturalDuration.TimeSpan.Duration / 10000000;
	return time;
}

double Mediaplayer::GetVolume()
{
	return Player->Volume;
}

void Mediaplayer::SetVolume(double Volume)
{
	Player->Volume = Volume;
}


void Mediaplayer::SelectCurrentTitelAtPlaylist()
{
	if (this->Playlist != nullptr)
	{
		if (this->Playlist->Items->Size == this->MediaFiles->Size && this->Playlist->Items->Size >= this->acttrack)
		{
			this->Playlist->SelectedIndex = this->acttrack;
		}
	}
}

Windows::Media::MediaPlaybackType Mediaplayer::GetPlaybackType()
{
	return this->GetMediaTypeFromFileContentType(this->GetCurrentFile());
}

void Mediaplayer::setBitmapAsCover(StorageFile^ CoverFile)
{
	create_task(CoverFile->OpenAsync(Windows::Storage::FileAccessMode::Read)).then([this](Streams::IRandomAccessStream^ fileStream)
	{
		// Set the image source to the selected bitmap
		auto bitmapImage = ref new Windows::UI::Xaml::Media::Imaging::BitmapImage();
		bitmapImage->DecodePixelHeight = 100;
		bitmapImage->DecodePixelWidth = 100;

		bitmapImage->SetSource(fileStream);
		this->Cover->Source = bitmapImage;
		this->Cover->Visibility = Windows::UI::Xaml::Visibility::Visible;
		this->MediaTitle->HorizontalAlignment = Windows::UI::Xaml::HorizontalAlignment::Right;
	});
}
void Mediaplayer::OpenFiles()
{
	Windows::Storage::Pickers::FileOpenPicker^ openPicker = help::create_open_picker("Wiedergeben");
	create_task(openPicker->PickMultipleFilesAsync()).then([this] (IVectorView<StorageFile^>^ toOpen)
	{
		try
		{
			this->OpenFiles(toOpen);
		}
		catch (Exception^ ex)
		{
			OutputDebugStringW(L"exp = ");
			OutputDebugStringW(ex->Message->Data());
			OutputDebugStringW(L"\n\n");
		}
	}).then([](task<void> previousTask)
	{
		try{
			previousTask.get();		//Get the result of task by forcing it.
		}
		catch (Exception^ ex)
		{
			OutputDebugStringW(L"exp = ");
			OutputDebugStringW(ex->Message->Data());
			OutputDebugStringW(L"\n\n");
		}
	});
}

void Mediaplayer::OpenFiles(IVectorView<StorageFile^>^ toOpen)
{
	if (toOpen != nullptr)
	{
		if (toOpen->Size > 0)
		{
			this->MediaFiles = help::getSupportedFiles(toOpen);
			this->acttrack = 0;
			this->RefreshPlaylist();
			this->autoplay = true;
			this->openstream();
		}
	}
}

void Mediaplayer::AddFiles()
{
	Windows::Storage::Pickers::FileOpenPicker^ openPicker = help::create_open_picker("Hinzufügen");
	create_task(openPicker->PickMultipleFilesAsync()).then([this] (IVectorView<StorageFile^>^ toAdd)
	{
		if (toAdd->Size>0)
		{
			this->AddFiles(toAdd);
		}
	});
	
}

void Mediaplayer::AddFiles(IVectorView<StorageFile^>^ toAdd)
{
	if (toAdd != nullptr)
	{
		if (toAdd->Size > 0)
		{
			if (this->MediaFiles != nullptr)
			{
				this->MediaFiles = help::addivec(this->MediaFiles, toAdd);
				if (this->Playlist != nullptr)
				{
					this->RefreshPlaylist();
				}
			}
			else
			{
				this->OpenFiles(toAdd);
			}
		}
	}
}

void Mediaplayer::openContainingFolder()
{
	if (this->MediaFiles->GetAt(acttrack) != nullptr)
	{
		create_task(this->MediaFiles->GetAt(acttrack)->GetParentAsync()).then([this](StorageFolder^ folder)
		{
			if (folder != nullptr)
			{
				this->MediaFiles = nullptr;
				this->addSubfolder(folder);
			}
		});
	}
}
void Mediaplayer::openFolderSub(StorageFolder^ Folder)
{
	if (Folder != nullptr)
	{
		this->MediaFiles = nullptr;
		this->addSubfolder(Folder);
	}
}

Windows::Media::MediaPlaybackType Mediaplayer::GetMediaTypeFromFileContentType(StorageFile^ file)
{
	Windows::Media::MediaPlaybackType mediaPlaybackType = MediaPlaybackType::Unknown;
	if (file != nullptr)
	{
		Platform::String^ fileMimeType = file->ContentType;
		Platform::String^ mimeTypeAudio = "audio/";
		Platform::String^ mimeTypeVideo = "video/";
		Platform::String^ mimeTypeImage = "image/";

		if (-1 != FindStringOrdinal(FIND_STARTSWITH, fileMimeType->Data(), fileMimeType->Length(),
			mimeTypeAudio->Data(), mimeTypeAudio->Length(), TRUE))
		{
			mediaPlaybackType = MediaPlaybackType::Music;
		}
		else if (-1 != FindStringOrdinal(FIND_STARTSWITH, fileMimeType->Data(), fileMimeType->Length(),
			mimeTypeVideo->Data(), mimeTypeVideo->Length(), TRUE))
		{
			mediaPlaybackType = MediaPlaybackType::Video;
		}
		else if (-1 != FindStringOrdinal(FIND_STARTSWITH, fileMimeType->Data(), fileMimeType->Length(),
			mimeTypeImage->Data(), mimeTypeImage->Length(), TRUE))
		{
			mediaPlaybackType = MediaPlaybackType::Image;
		}
	}
	return mediaPlaybackType;
}


void Mediaplayer::MediaOpend()
{
	if(autoplay==true)
		this->Play();
	const wchar_t* mimes=MediaFiles->GetAt(acttrack)->ContentType->Begin();
	if(mimes[0]==L'v')
	{
		if (this->MediaTitle != nullptr)
		{
			create_task(MediaFiles->GetAt(acttrack)->Properties->GetVideoPropertiesAsync()).then([this](VideoProperties^ actprop)
			{
				if (actprop->Title != "")
				{
					MediaTitle->Text = actprop->Title;
				}
				else
				{
					MediaTitle->Text = MediaFiles->GetAt(acttrack)->DisplayName;
				}
			});
		}
		if (this->Cover != nullptr)
		{
			LPCWSTR a = L"Get Thumb\n";
			OutputDebugString(a);
			create_task(MediaFiles->GetAt(acttrack)->GetParentAsync()).then([this](StorageFolder^ temp)
			{

				if (temp != nullptr)
				{
					//String^ FileName = "1111.txt";
					task<StorageFile^> GetFileTask(temp->GetFileAsync("Cover.jpg"));
					this->Cover->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
					this->MediaTitle->HorizontalAlignment = Windows::UI::Xaml::HorizontalAlignment::Center;
					GetFileTask.then([this](StorageFile^ CoverFile)
					{
						try
						{
							if (CoverFile != nullptr)
							{
								this->setBitmapAsCover(CoverFile);
							}
							else
							{
								Uri^ nocover = ref new Uri("ms-appx:///media_files/noCover.png");
								create_task(StorageFile::GetFileFromApplicationUriAsync(nocover)).then([this](StorageFile^ CoverFile)
								{
									if (CoverFile!=nullptr)
										this->setBitmapAsCover(CoverFile);
								});
							}
						}
						catch (Exception^ ex)
						{
							OutputDebugStringW(L"exp = ");
							OutputDebugStringW(ex->Message->Data());
							OutputDebugStringW(L"\n\n");
						}

					}).then([](task<void> previousTask)
					{
						try{
							previousTask.get();		//Get the result of task by forcing it.
						}
						catch (Exception^ ex)
						{
							OutputDebugStringW(L"exp = ");
							OutputDebugStringW(ex->Message->Data());
							OutputDebugStringW(L"\n\n");
						}
					});
				}
			});
		}
	}
	else if(mimes[0]==L'a')
	{
		if (this->MediaTitle != nullptr)
		{
			create_task(MediaFiles->GetAt(acttrack)->Properties->GetMusicPropertiesAsync()).then([this](MusicProperties^ actprop)
			{
				if (actprop->Title != "")
				{
					MediaTitle->Text = actprop->Title;
				}
				else
				{
					MediaTitle->Text = MediaFiles->GetAt(acttrack)->DisplayName;
				}
			});
		}
		if (this->Cover != nullptr)
		{
			//var thumbnaiMode
			Windows::Storage::FileProperties::ThumbnailMode::MusicView;
			//var thumbnailOptions
			Windows::Storage::FileProperties::ThumbnailOptions::ResizeThumbnail;
			LPCWSTR a = L"Get Thumb\n";
			//OutputDebugString(a);
			create_task(MediaFiles->GetAt(acttrack)->GetThumbnailAsync(Windows::Storage::FileProperties::ThumbnailMode::MusicView, 96, Windows::Storage::FileProperties::ThumbnailOptions::ResizeThumbnail)).then([this](StorageItemThumbnail^ thumbnail)
			{
				LPCWSTR a = L"Got Thumb\n";
				OutputDebugString(a);

				if (thumbnail != nullptr)
				{
					IRandomAccessStream^ thumbnailstream = (IRandomAccessStream^)thumbnail;
					auto bitmapImage = ref new Windows::UI::Xaml::Media::Imaging::BitmapImage();
					bitmapImage->DecodePixelHeight = 70;
					bitmapImage->DecodePixelWidth = 70;

					bitmapImage->SetSource(thumbnailstream);
					this->Cover->Source = bitmapImage;
					//Cover->Visibility = Windows::UI::Xaml::Visibility::Visible;
					this->MediaTitle->HorizontalAlignment = Windows::UI::Xaml::HorizontalAlignment::Right;
				}
			});
		}
	}
	if (TotalTimeText!=nullptr)
		this->TotalTimeText->Text = help::timetostring(this->GetTotTime());
	if (TimeSlider != nullptr)
	{
		this->TimeSlider->Maximum = (double)this->GetTotTime();
		this->TimeSliderTimer->Start();
	}
}


StorageFile^ Mediaplayer::GetCurrentFile()
{
	if (this->MediaFiles != nullptr)
		return this->MediaFiles->GetAt(this->acttrack);
	else
		return nullptr;
}



void Mediaplayer::openstream()
{
	cw->Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([=]()
	{
		if(this->MediaFiles != nullptr)
		{
			create_task(MediaFiles->GetAt(acttrack)->OpenAsync(FileAccessMode::Read)).then([this](Streams::IRandomAccessStream^ stream)
			{
				this->Player->SetSource(stream, MediaFiles->GetAt(acttrack)->ContentType);
				if (Playlist != nullptr)
				{
					if (this->Playlist->Items->Size == this->MediaFiles->Size && this->Playlist->Items->Size >= this->acttrack)
					{
						this->Playlist->SelectedIndex = this->acttrack;
					}
				}
				smtc->IsPlayEnabled = true;
				smtc->IsPauseEnabled = true;
				smtc->IsStopEnabled = true;
				smtc->IsFastForwardEnabled = true;
				smtc->IsRewindEnabled = true;
				if (this->MediaFiles->Size <= this->acttrack + 1 && (mode != 1 && mode != 3))
				{
					smtc->IsNextEnabled = false;
				}
				else
				{
					smtc->IsNextEnabled = true;
				}
				if (this->acttrack > 0)
				{
					smtc->IsPreviousEnabled = true;
				}

				try
				{

					create_task(smtc->DisplayUpdater->CopyFromFileAsync(GetMediaTypeFromFileContentType(MediaFiles->GetAt(acttrack)), MediaFiles->GetAt(acttrack))).then([this](bool success)
					{
						try
						{
							if (success)
							{
								// updates UI with the new values set in DisplayUpdater
								smtc->IsPlayEnabled = true;
								try
								{
									smtc->DisplayUpdater->Update();
								}
								catch (Exception^ ex)
								{
									OutputDebugStringW(L"exp = ");
									OutputDebugStringW(ex->Message->Data());
									OutputDebugStringW(L"\n\n");
								}
							}
							else
							{
								// ClearAll() updates the UI immediately
								smtc->DisplayUpdater->ClearAll();
							}
						}
						catch (Exception^ ex)
						{
							OutputDebugStringW(L"exp = ");
							OutputDebugStringW(ex->Message->Data());
							OutputDebugStringW(L"\n\n");
						}
					}).then([](task<void> previousTask)
					{
						try{
							previousTask.get();		//Get the result of task by forcing it.
						}
						catch (Exception^ ex)
						{
							OutputDebugStringW(L"exp = ");
							OutputDebugStringW(ex->Message->Data());
							OutputDebugStringW(L"\n\n");
						}
					});;
				}
				catch (Exception^ e)
				{
					cw->Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([=]()
					{
						//Harties_Media_Center::App::->NotifyUser(e->Message, NotifyType::ErrorMessage);
					}));
				}
				
			}).then([](task<void> previousTask)
			{
				try{
					previousTask.get();		//Get the result of task by forcing it.
				}
				catch (Exception^ ex)
				{
					OutputDebugStringW(L"exp = ");
					OutputDebugStringW(ex->Message->Data());
					OutputDebugStringW(L"\n\n");
				}
			});
		}
	}));
}

void Mediaplayer::SetModeButtonValue()
{
	if (this->ModeButton != nullptr)
	{
		Windows::UI::Xaml::Media::SolidColorBrush^ Temp_Color;
		Temp_Color = ref new Windows::UI::Xaml::Media::SolidColorBrush();
		switch (this->mode)
		{
		case 1:
			this->ModeButton->Content = L"\xe1CD";
			Windows::UI::Xaml::Automation::AutomationProperties::SetName(this->ModeButton, "Repeat-All");
			Temp_Color->Color = Windows::UI::Colors::White;
			this->ModeButton->Foreground = Temp_Color;
			break;
		case 2:
			this->ModeButton->Content = L"\xe1CC";
			Windows::UI::Xaml::Automation::AutomationProperties::SetName(this->ModeButton, "Repeat-Single");
			Temp_Color->Color = Windows::UI::Colors::White;
			this->ModeButton->Foreground = Temp_Color;
			break;
		case 3:
			this->ModeButton->Content = L"\xe14B";
			Windows::UI::Xaml::Automation::AutomationProperties::SetName(this->ModeButton, "Shuffle");
			Temp_Color->Color = Windows::UI::Colors::White;
			this->ModeButton->Foreground = Temp_Color;
			break;
		default:
			this->ModeButton->Content = L"\xe1CD";
			Windows::UI::Xaml::Automation::AutomationProperties::SetName(this->ModeButton, "Repeat-None");
			Temp_Color->Color = Windows::UI::Colors::Gray;
			this->ModeButton->Foreground = Temp_Color;
			break;
		}
	}
}
void Mediaplayer::RefreshPlaylist()
{
	if(this->MediaFiles!=nullptr && this->Playlist!= nullptr)
	{
		unsigned int globalcounter=0;
		this->Playlist->SelectedItem=nullptr;
		this->Playlist->Items->Clear();
		// Define work here.
		unsigned int size=this->MediaFiles->Size;
		while(globalcounter<size)
		{
			this->Playlist->Items->Append(this->MediaFiles->GetAt(globalcounter)->DisplayName);
			globalcounter++;
		}
		this->SelectCurrentTitelAtPlaylist();
		this->globalcounter=0;
		 ready=false;
		Title="\n";

		
		this->playlisttimer->Start();

	  /*PLAniS = ref new Storyboard();
	  PLAniS->SetTarget(PLAni, Playlist);
	  PLAniS->SetTargetProperty(PLAni, "Opacity");
	  PLAniS->Children->Append(PLAni);
	  PLAniS->Begin();*/
	  //((Windows::UI::Xaml::Controls::ListViewItem^) Playlist->Items->GetAt(0));// ->PointerEntered+= ref new Windows::UI::Xaml::Input::PointerEventHandler(this, &Mediaplayer::Item_PointerEntered);
	  //((Windows::UI::Xaml::Controls::ListViewItem^) Playlist->Items->GetAt(0))->PointerExited += ref new Windows::UI::Xaml::Input::PointerEventHandler(this, &Mediaplayer::Item_PointerExited);
			//SetTitles();
			//create_task(SetTitles()).then([this]{});
	}
}

void Mediaplayer::SetTitles(Platform::Object^ sender, Platform::Object^ e)
{
	if (this->MediaFiles != nullptr && this->Playlist != nullptr)
	{
		if (ready == true)
		{
			if (Title != "")
			{
				Playlist->Items->SetAt(this->globalcounter, Title);
				Title = "";
			}
			this->globalcounter++;
			if (this->globalcounter < MediaFiles->Size)
			{
				this->getTitle();
			}
			else
			{
				this->playlisttimer->Stop();
			}

		}
		else if (ready == false && Title == "\n")
		{
			Title = "";
			this->getTitle();
		}
	}
}

void Mediaplayer::getTitle()
{
		ready=false;
		const wchar_t* mimes=MediaFiles->GetAt(globalcounter)->ContentType->Begin();
		if(mimes[0]==L'v')
		{
			create_task(MediaFiles->GetAt(globalcounter)->Properties->GetVideoPropertiesAsync()).then([this]  (VideoProperties^ actprop)
			{
				if(actprop->Title!="")
				{
					Title=actprop->Title;
					__int64 temp=(__int64)(globalcounter+1);
					String^ number="";
					if(temp<10)
					{
						number=" "+L'0'+(__int64)(temp);
						number=number+" ";
					}
					else
					{
						number=" "+(__int64)(temp);
						number=number+" ";
					}
					Title=number+Title;
				}
				else
				{
					
				}

				ready=true;
			});
		}
		else if(mimes[0]==L'a')
		{
			create_task(MediaFiles->GetAt(globalcounter)->Properties->GetMusicPropertiesAsync()).then([this] (MusicProperties^ actprop)
			{
				
				if(actprop->Title!=""&&actprop->Title!=nullptr)
				{
					Title=actprop->Title;
					//this->Playlist->Items->Append(" "+this->MediaFiles->GetAt(globalcounter)->DisplayName);
					//Playlist->Items->SetAt(2,actprop->Title);
					__int64 temp=(__int64)(globalcounter+1);
					String^ number="";
					if(temp<10)
					{
						number=" "+L'0'+(__int64)(temp);
						number=number+" ";
					}
					else
					{
						number=" "+(__int64)(temp);
						number=number+" ";
					}
					Title=number+Title;
				}
				else
				{
				
				}

				ready=true;
			});
		}
}

void Mediaplayer::HideInfo(Platform::Object^ sender, Platform::Object^ e)
{
	this->StateBox->Visibility=Windows::UI::Xaml::Visibility::Collapsed;
	this->Infotimer->Stop();
}

void Mediaplayer::MediaButtonPressed(Windows::Media::SystemMediaTransportControls^ sender, Windows::Media::SystemMediaTransportControlsButtonPressedEventArgs^ args)
{
	switch (args->Button)
	{
	case SystemMediaTransportControlsButton::Next:
		/*DispatchNotifyUser("Next pressed", NotifyType::StatusMessage);
		currentIndex++;
		SetCurrentPlayingAsync(currentIndex);*/
		cw->Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([this]()
		{
			this->Next(true);
		}));
		break;

	case SystemMediaTransportControlsButton::Previous:
		/*DispatchNotifyUser("Previous pressed", NotifyType::StatusMessage);
		currentIndex--;
		SetCurrentPlayingAsync(currentIndex);*/
		cw->Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([this]()
		{
			this->Previous();
		}));
		break;

	case SystemMediaTransportControlsButton::Play:
		cw->Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([this]()
		{
			this->Play();
		}));
		break;

	case SystemMediaTransportControlsButton::Pause:
		cw->Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([this]()
		{
			this->Pause();
		}));
		break;

	case SystemMediaTransportControlsButton::Stop:
		cw->Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([this]()
		{
			this->Stop();
		}));
		break;
	case SystemMediaTransportControlsButton::FastForward :
		cw->Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([this]()
		{
			this->FastForward();
		}));
		break;
	case SystemMediaTransportControlsButton::Rewind:
		cw->Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([this]()
		{
			this->Backward();
		}));
		break;
	}
}

void Mediaplayer::playerCurrentStateChanged(Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	/*if (Player->CurrentState == MediaElementState::Playing)
	{
		isPlaying = true;
		PlayPauseButton->Content = "Pause";
	}
	else if (!isChangingMedia &&
		(MyMediaElement->CurrentState == MediaElementState::Stopped || MyMediaElement->CurrentState == MediaElementState::Paused))
	{
		isPlaying = false;
		PlayPauseButton->Content = "Play";
	}*/

	switch (Player->CurrentState)
	{
	case MediaElementState::Closed:
		smtc->PlaybackStatus = MediaPlaybackStatus::Closed;
		break;

	case MediaElementState::Opening:
		smtc->PlaybackStatus = MediaPlaybackStatus::Changing;
		break;

	case MediaElementState::Buffering:
		// no changes in MediaPlaybackStatus
		break;

	case MediaElementState::Paused:
		smtc->PlaybackStatus = MediaPlaybackStatus::Paused;
		break;

	case MediaElementState::Playing:
		smtc->PlaybackStatus = MediaPlaybackStatus::Playing;
		break;

	case MediaElementState::Stopped:
		smtc->PlaybackStatus = MediaPlaybackStatus::Stopped;
		break;
	}
}

//Playlist animation
void Mediaplayer::Item_PointerEntered(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e)
{
	PLAniS = ref new Storyboard();
	PLAniS->SetTarget(PLAni,(Windows::UI::Xaml::Controls::ListViewItem^)sender);
	PLAniS->SetTargetProperty(PLAni, "Opacity");
	PLAniS->Children->Append(PLAni);
	PLAniS->Begin();
	
}

void Mediaplayer::Item_PointerExited(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e)
{
	PLAniS->Stop();
}





void Mediaplayer::addSubfolder(StorageFolder^ folder)
{
	create_task(folder->GetFilesAsync()).then([this](IVectorView<StorageFile^>^ temp)
	{
		//this->MediaFiles = temp;
			this->AddFiles(temp);
	});
	create_task(folder->GetFoldersAsync()).then([this](IVectorView<StorageFolder^>^ temp)
	{
		unsigned int max = temp->Size;
		unsigned int i = 0;
		for (i = 0; i < max; i++)
		{
			this->addSubfolder(temp->GetAt(i));
		/*	create_task(temp->GetAt(i)->GetFilesAsync()).then([this](IVectorView<StorageFile^>^ temp)
			{
				//this->MediaFiles = temp;
				this->AddFiles(temp);
			});*/
		}
	});

}

void showState(String^ infotext)
{

}

void Mediaplayer::test()
{



}
using namespace Windows::UI::Popups;
using namespace Windows::UI::Xaml;
Rect GetElementRect(FrameworkElement^ element)
{
	GeneralTransform^ buttonTransform = element->TransformToVisual(nullptr);
	const Point pointOrig(0, 0);
	const Point pointTransformed = buttonTransform->TransformPoint(pointOrig);
	const Rect rect(pointTransformed.X, pointTransformed.Y, safe_cast<float>(element->ActualWidth), safe_cast<float>(element->ActualHeight));
	return rect;
}


void Mediaplayer::MediaOpened(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	this->MediaOpend();
}

void Mediaplayer::MediaPlaybackFailed(Platform::Object^ sender, Windows::UI::Xaml::ExceptionRoutedEventArgs^ e)
{
	/*Todo*/
	//Windows::UI::Popups::MessageDialog^ msbox = ref new Windows::UI::Popups::MessageDialog("Fehler Beim öffnen der Datei");

}


void Mediaplayer::MediaEnded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	this->Player->Stop();
	this->Next();
}

void Mediaplayer::opentapped(Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e)
{
	// Create a menu and add commands specifying a callback delegate for each.
	// Since command delegates are unique, no need to specify command Ids.
	OutputDebugStringW(L"Create Context Menue");
	auto menu = ref new PopupMenu();
	menu->Commands->Append(ref new UICommand("Open Files", ref new UICommandInvokedHandler([this](IUICommand^ command)
	{
			this->OpenFiles();
		
	})));

	menu->Commands->Append(ref new UICommand("AddFiles", ref new UICommandInvokedHandler([this](IUICommand^ command)
	{
		this->AddFiles();
	})));
	menu->Commands->Append(ref new UICommand("Open Containing Folder", ref new UICommandInvokedHandler([this](IUICommand^ command)
	{
		this->openContainingFolder();
	})));
	// We don't want to obscure content, so pass in a rectangle representing the sender of the context menu event.
	auto rect = GetElementRect(safe_cast<FrameworkElement^>(sender));
	create_task(menu->ShowForSelectionAsync(rect)).then([this](IUICommand^ command)
	{
		if (command == nullptr)
		{
			// The command is null if no command was invoked.
			//OutputTextBlock->Text = L"Context menu dismissed";
		}
	});
}

void Mediaplayer::moreTapped(Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e)
{
	// Create a menu and add commands specifying a callback delegate for each.
	// Since command delegates are unique, no need to specify command Ids.
	OutputDebugStringW(L"Create Context Menue");
	auto menu = ref new PopupMenu();
	menu->Commands->Append(ref new UICommand("Change Audio", ref new UICommandInvokedHandler([this](IUICommand^ command)
	{
		SelectAudio();
	})));

	int languages = Player->AudioStreamCount;
	for (int i = 0; i < languages; i++)
	{
		String^ language = Player->GetAudioStreamLanguage(i);
		menu->Commands->Append(ref new UICommand(language, ref new UICommandInvokedHandler([this,i](IUICommand^ command)
		{
			SelectAudio(i);
		})));
	}

	menu->Commands->Append(ref new UICommand("Clear Settings", ref new UICommandInvokedHandler([this](IUICommand^ command)
	{
		settings::reset();
	})));

	menu->Commands->Append(ref new UICommand(settings::getValue("OpenpickerMode"), ref new UICommandInvokedHandler([this](IUICommand^ command)
	{
		if (settings::getValue("OpenpickerMode") == "List")
			settings::setValue("OpenpickerMode", "Thumbnail");
		else
			settings::setValue("OpenpickerMode", "List");
	})));

	menu->Commands->Append(ref new UICommand(settings::getValue("OpenWith"), ref new UICommandInvokedHandler([this](IUICommand^ command)
	{
		if (settings::getValue("OpenWith") == "Open")
			settings::setValue("OpenWith", "Add");
		else
			settings::setValue("OpenWith", "Open");
	})));

	// We don't want to obscure content, so pass in a rectangle representing the sender of the context menu event.
	auto rect = GetElementRect(safe_cast<FrameworkElement^>(sender));
	create_task(menu->ShowForSelectionAsync(rect)).then([this](IUICommand^ command)
	{
		if (command == nullptr)
		{
			// The command is null if no command was invoked.
			//OutputTextBlock->Text = L"Context menu dismissed";
		}
	});
	//OutputTextBlock->Text = "Context menu shown";*/
}


void Mediaplayer::PlayPauseTapped(Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e)
{
	this->PlayPause();
}

void Mediaplayer::PrevTapped(Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e)
{
	this->Previous();
}
void Mediaplayer::NextTapped(Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e)
{
	this->Next(true);
}
void Mediaplayer::NextHolding(Platform::Object^ sender, Windows::UI::Xaml::Input::HoldingRoutedEventArgs^ e)
{
	this->FastForward();
}


void Mediaplayer::PrevHolding(Platform::Object^ sender, Windows::UI::Xaml::Input::HoldingRoutedEventArgs^ e)
{
	this->Backward();
}

void Mediaplayer::ModeButtonTapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e)
{
	this->SwitchMode();

	//shuffle &#xE14B;
	//repeat &#xE1CC;
	//repeat all  &#xE1CD;
}

void Mediaplayer::TimeSliderManipulationStarted(Platform::Object^ sender, Windows::UI::Xaml::Input::ManipulationStartedRoutedEventArgs^ e)
{
	this->Pause();
	this->TimeSliderTimer->Stop();
}


void Mediaplayer::TimeSliderValueChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs^ e)
{
	if (TimeSliderTimer->IsEnabled == false)
	{
		TimeSpan MediaTime = this->Player->Position;
		MediaTime.Duration = help::sectotime((long long)this->TimeSlider->Value);;
		this->Player->Position = MediaTime;
	}
}

void Mediaplayer::TimeSliderManipulationCompleted(Platform::Object^ sender, Windows::UI::Xaml::Input::ManipulationCompletedRoutedEventArgs^ e)
{
	this->Play();
	TimeSpan vtime = this->Player->Position;
	vtime.Duration = help::sectotime((int64)this->TimeSlider->Value);;
	this->Player->Position = vtime;
	TimeSliderTimer->Start();
}

void Mediaplayer::TimeSliderTimerHandler(Platform::Object^ sender, Platform::Object^ e)
{
	this->TimeSlider->Value = (double)this->GetActTime();
	if(this->CurrentTimeText!=nullptr)
		CurrentTimeText->Text = help::timetostring(this->GetActTime());
}

void Mediaplayer::PlaylistSelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e)
{
	if (this->Playlist->SelectedItem != nullptr)
	{
		this->PlayTrack(this->Playlist->SelectedIndex);
	}

}

void Mediaplayer::VolumePointerWheelChanged(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e)
{
	auto properties = e->GetCurrentPoint(VolumeSlider)->Properties;
	if (properties->MouseWheelDelta > lastmrpos)
		this->SetVolume(this->GetVolume() + 0.1);
	else if (properties->MouseWheelDelta < lastmrpos)
		this->SetVolume(this->GetVolume() - 0.1);
	VolumeSlider->Value = this->GetVolume() * 100;
}


void Mediaplayer::VolumeChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs^ e)
{
		this->SetVolume(this->VolumeSlider->Value / 100);
}
