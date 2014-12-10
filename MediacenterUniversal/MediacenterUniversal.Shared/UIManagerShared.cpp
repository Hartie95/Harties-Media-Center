#include "pch.h"
#include "UIManagerShared.h"
#include "Constands.h"
#include "Settings.h"

#include <ppltasks.h>
#include <collection.h>

using namespace UIManager;
using namespace ControlleSymbols;
using namespace Windows::Data::Json;

using namespace Concurrency;
using namespace UIEllementNames;


UIManagerShared::UIManagerShared(Grid^ mainGrid, Windows::UI::Xaml::Window^ mainWindow)
{
	this->mainWindow = mainWindow;
	this->mainGrid = mainGrid;
	//Ad^ mainAd


	this->mediaPlayerUIElements = ref new Platform::Collections::Map<Platform::String^, Windows::UI::Xaml::FrameworkElement^>();

	//Player
	//change to grids?
	this->headerAppBar = ref new AppBar();
	this->bottomAppBar = ref new AppBar();
	//make ist global??
	this->playlist=ref new ListView();
	this->playerGrid = ref new Grid();
	this->mediaPlayerUIElements->Insert(MEDIAPLAYERGRID, playerGrid);

	this->playerControlesGrid = ref new Grid();
	this->mediaPlayerUIElements->Insert(MEDIAPLAYERHEADERBARGRID, ref new Grid());
	this->mediaPlayerUIElements->Insert(MEDIAPLAYERBOTTOMBARGRID, playerControlesGrid);
	this->mediaPlayerUIElements->Insert(PLAYPAUSEBUTTON, ref new Button());
	this->mediaPlayerUIElements->Insert(NEXTBUTTON, ref new Button());
	this->mediaPlayerUIElements->Insert(PREVBUTTON, ref new Button());
	this->mediaPlayerUIElements->Insert(MODEBUTTON, ref new Button());
	this->mediaPlayerUIElements->Insert(PLAYEDTIME, ref new TextBlock());
	this->mediaPlayerUIElements->Insert(TOTALTIME, ref new TextBlock());
	this->mediaPlayerUIElements->Insert(VOLUMESLIDER, ref new Slider());
	this->mediaPlayerUIElements->Insert(TIMESLIDER, ref new Slider());
	this->mediaPlayerUIElements->Insert(MEDIAPLAYER, ref new MediaElement());
	this->mediaPlayerUIElements->Insert(BACKBUTTON, ref new Button());
	this->mediaPlayerUIElements->Insert(PLAYLISTBUTTON, ref new Button());
	this->mediaPlayerUIElements->Insert(MEDIATITLE, ref new TextBlock());
	this->mediaPlayerUIElements->Insert(MEDIACOVER, ref new Image());
	//Button^ OpenButton;
	
	
	MediaElement^ player = (MediaElement^)this->getElement(MEDIAPLAYER);
	player->AudioCategory = Windows::UI::Xaml::Media::AudioCategory::BackgroundCapableMedia;
	player->AutoPlay = false;
	player->IsRightTapEnabled = false; 
	Windows::UI::Xaml::Thickness marg(0, 0, 0, 0);	
	player->Margin = marg;
	Windows::UI::Xaml::Controls::Canvas::SetZIndex(player,-1);

	this->mainGrid->Children->Append(this->getElement(MEDIAPLAYERGRID));

	this->getElement(PLAYLISTBUTTON)->Tapped += ref new Windows::UI::Xaml::Input::TappedEventHandler(this, &UIManagerShared::playlistButtonTapped);

	this->mainGrid->UpdateLayout();
	mainWindow->SizeChanged += ref new Windows::UI::Xaml::WindowSizeChangedEventHandler(this, &UIManager::UIManagerShared::OnSizeChanged);

}


Mediaplayer^ UIManagerShared::createMediaPlayer(Windows::System::Display::DisplayRequest^ DisplayNeed)
{
	Mediaplayer^ mediaplayer = ref new Mediaplayer((MediaElement^)this->getElement(MEDIAPLAYER), DisplayNeed);
	mediaplayer->setCover((Image^)this->getElement(MEDIACOVER));
	mediaplayer->setModeButton((Button^)this->getElement(MODEBUTTON));
	mediaplayer->setNextButton((Button^)this->getElement(NEXTBUTTON));
	mediaplayer->setPlaylist(this->playlist);
	mediaplayer->setPlayPauseButton((Button^)this->getElement(PLAYPAUSEBUTTON));
	mediaplayer->setPrevButton((Button^)this->getElement(PREVBUTTON));
	mediaplayer->setTimeSlider((Slider^)this->getElement(TIMESLIDER));
	mediaplayer->setTimeText((TextBlock^)this->getElement(PLAYEDTIME), (TextBlock^)this->getElement(TOTALTIME));
	mediaplayer->setTitelBox((TextBlock^)this->getElement(MEDIATITLE));
	mediaplayer->setVolumeSlider((Slider^)this->getElement(VOLUMESLIDER));
	//todo remove
	this->getElement(BACKBUTTON)->Tapped += ref new Windows::UI::Xaml::Input::TappedEventHandler(mediaplayer, &Mediaplayer::opentapped);
	return mediaplayer;
}

void UIManagerShared::changeMode(int mode)
{
	if (mode == Modes::Player)
		this->showPlayer();
}

void UIManagerShared::showPlayer()
{
	Windows::UI::Xaml::Thickness marg(0, 0, 0, 0);
	this->getElement(MEDIAPLAYERGRID)->Margin = marg;
	this->getElement(MEDIAPLAYERGRID)->Visibility = Windows::UI::Xaml::Visibility::Visible;
	this->getElement(MEDIAPLAYER)->Visibility = Windows::UI::Xaml::Visibility::Visible;
	this->mainGrid->UpdateLayout();
}

void UIManagerShared::appendTheme()
{	
	create_task(Windows::Storage::FileIO::ReadTextAsync(this->themeFile)).then([this](String^ JsonString)
	{
		this->themeObject = JsonObject::Parse(JsonString);
		this->partsVertical = (int)this->themeObject->GetNamedNumber("partsVertical");
		this->partsHorizontal = (int)this->themeObject->GetNamedNumber("partsHorizontal");

		Windows::Foundation::Collections::IMapView<String^, IJsonValue^>^ test = this->themeObject->GetView();
		std::for_each(begin(test), end(test), [this](IKeyValuePair<Platform::String^, IJsonValue^>^ pair)
		{
			// do stuff
			auto key = pair->Key;
			IJsonValue^ value = pair->Value;
			if (key == "styles")
				this->appendStyles(value);
			if (key == "Content")
				this->appendGrids(value);
				
		});
		create_task(Windows::Storage::FileIO::ReadTextAsync(this->themeNamesFile)).then([this](String^ JsonString)
		{
			JsonObject^ jsonObject = JsonObject::Parse(JsonString);
			jsonObject->GetNamedString("PLAY", L"");
			((Button^)this->getElement(PLAYPAUSEBUTTON))->Content = jsonObject->GetNamedString("PLAY", L"");
			((Button^)this->getElement(NEXTBUTTON))->Content = jsonObject->GetNamedString("NEXT", L"");
			((Button^)this->getElement(PREVBUTTON))->Content = jsonObject->GetNamedString("PREVIOUS", L"");
			((Button^)this->getElement(MODEBUTTON))->Content = jsonObject->GetNamedString("REPEATNONE", L"");

			((Button^)this->getElement(BACKBUTTON))->Content = jsonObject->GetNamedString("BACK", L"");
			((Button^)this->getElement(PLAYLISTBUTTON))->Content = jsonObject->GetNamedString("PLAYLIST", L"");
			
			Windows::UI::Xaml::Automation::AutomationProperties::SetName((Button^)this->getElement(PLAYPAUSEBUTTON), jsonObject->GetNamedString("NAMEPLAY", L""));
			Windows::UI::Xaml::Automation::AutomationProperties::SetName((Button^)this->getElement(NEXTBUTTON), jsonObject->GetNamedString("NAMENEXT", L""));
			Windows::UI::Xaml::Automation::AutomationProperties::SetName((Button^)this->getElement(PREVBUTTON), jsonObject->GetNamedString("NAMEPREVIOUS", L""));
			Windows::UI::Xaml::Automation::AutomationProperties::SetName((Button^)this->getElement(MODEBUTTON), jsonObject->GetNamedString("NAMEREPEATNONE", L""));
			Windows::UI::Xaml::Automation::AutomationProperties::SetName((Button^)this->getElement(BACKBUTTON), jsonObject->GetNamedString("NAMEBACK", L""));
			Windows::UI::Xaml::Automation::AutomationProperties::SetName((Button^)this->getElement(PLAYLISTBUTTON), jsonObject->GetNamedString("NAMEPLAYLIST", L""));
		});
	});
}

void  UIManagerShared::appendStyles(Windows::Data::Json::IJsonValue^ value)
{
	JsonObject^ jsonObject = value->GetObject();
	Windows::Foundation::Collections::IMapView<String^, IJsonValue^>^ jsonMap = jsonObject->GetView();

	std::for_each(begin(jsonMap), end(jsonMap), [this](IKeyValuePair<Platform::String^, IJsonValue^>^ pair)
	{
		auto key = pair->Key;
		IJsonValue^ value = pair->Value;
		if (this->getElement(key) != nullptr)
		{
			
			int width = 0;
			int height = 0;
			String^ valign = "center";
			String^ halign = "left";
			String^ fColor = "white";
			int zIndex = 0;

			JsonObject^ jsonObject = value->GetObject();
			if (jsonObject->HasKey("width"))
			{
				width = (int)jsonObject->GetNamedNumber("width");
			}
			if (jsonObject->HasKey("height"))
			{
				height = (int)jsonObject->GetNamedNumber("height");
			}
			if (jsonObject->HasKey("verticalAlign"))
			{
				valign = jsonObject->GetNamedString("verticalAlign");
			}
			if (jsonObject->HasKey("horizontalAlign"))
			{
				halign = jsonObject->GetNamedString("horizontalAlign");
			}
			if (jsonObject->HasKey("foregroundColor"))
			{
				fColor = jsonObject->GetNamedString("foregroundColor");
			}
			if (jsonObject->HasKey("z-index"))
			{
				zIndex = (int)jsonObject->GetNamedNumber("z-index");
			}
			isType(key);
			Windows::UI::Xaml::FrameworkElement^ element = this->getElement(key);
			if (this->isType(key)=="Button")
				this->setDefaultValues((Button^)element);
			
			this->setSize(element, width, height);
			this->setForegroundColor(element, fColor);
			
			if (jsonObject->HasKey("margin"))
			{
				this->setMargin(element, jsonObject->GetNamedObject("margin"));
			}
			if (jsonObject->HasKey("background"))
			{
				this->setBackground(key, jsonObject->GetNamedObject("background"));
			}
			else
			{
				this->setBackground(key);
			}
			this->setVertikalAlign(element, valign);
			this->setHorizontalAlign(element, halign);
			this->setZIndex(element, zIndex);
		}
	});
}

void  UIManagerShared::appendGrids(Windows::Data::Json::IJsonValue^ value)
{
	JsonObject^ jsonObject = value->GetObject();
	Windows::Foundation::Collections::IMapView<String^, IJsonValue^>^ jsonMap = jsonObject->GetView();
	
	std::for_each(begin(jsonMap), end(jsonMap), [this](IKeyValuePair<Platform::String^, IJsonValue^>^ pair)
	{
		auto key = pair->Key;
		IJsonValue^ value = pair->Value;
		/*if (isdigit(key->Begin()[0]))
		{
			->Children->Append(this->getElement(pair->Value->GetString()));
		}*/
		if(this->getElement(key)!=nullptr)
			this->appendGrids(value->GetObject(), key);

	});
}

void  UIManagerShared::appendGrids(Windows::Data::Json::JsonObject^ jsonObject,String^ gridKey)
{
	Grid^ grid = (Grid^)this->getElement(gridKey);
	Windows::Foundation::Collections::IMapView<String^, IJsonValue^>^ jsonMap = jsonObject->GetView();
	grid->Children->Clear();
	std::for_each(begin(jsonMap), end(jsonMap), [this, grid](IKeyValuePair<Platform::String^, IJsonValue^>^ pair)
	{
		auto key = pair->Key;
		IJsonValue^ value = pair->Value;
		if (isdigit(key->Begin()[0]))
		{
			grid->Children->Append(this->getElement(pair->Value->GetString()));
		}
		else
		{
			grid->Children->Append(this->getElement(key));
			this->appendGrids(value->GetObject(), key);
		}

	});
}

void UIManagerShared::setMargin(Windows::UI::Xaml::FrameworkElement^ element, JsonObject^ margin)
{
	int left = 0;
	int right = 0;
	int top = 0;
	int bottom = 0;
	if (margin->HasKey("vertikal"))
	{
		left = (int) margin->GetNamedNumber("vertikal");
		right = left;
	}
	else
	{
		if (margin->HasKey("left"))
		{
			left = (int)margin->GetNamedNumber("left");
		}
		if (margin->HasKey("right"))
		{
			right = (int) margin->GetNamedNumber("right");
		}
	}
	if (margin->HasKey("horizontal"))
	{
		top = (int) margin->GetNamedNumber("horizontal");
		bottom = top;
	}
	else
	{
		if (margin->HasKey("top"))
		{
			top = (int) margin->GetNamedNumber("top");
		}
		if (margin->HasKey("bottom"))
		{
			bottom = (int) margin->GetNamedNumber("bottom");
		}
	}
	this->setMargin(element, left, top, right, bottom);
}

void UIManagerShared::setMargin(Windows::UI::Xaml::FrameworkElement^ element, int margin)
{
	this->setMargin(element, margin, margin, margin, margin);
}

void UIManagerShared::setMargin(Windows::UI::Xaml::FrameworkElement^ element, int vertikal, int horizontal)
{
	this->setMargin(element, vertikal, horizontal, vertikal, horizontal);
}

void UIManagerShared::setMargin(Windows::UI::Xaml::FrameworkElement^ element, int left, int top, int right, int bottom)
{
	double blockHeight;
	double blockWidth;
	double windowsHeight = mainWindow->Bounds.Height;
	double windowsWidth = mainWindow->Bounds.Width;
	//if (windowsHeight < windowsWidth)
	blockHeight = windowsHeight;
	//else
	blockWidth = windowsWidth;

	blockHeight /= this->partsVertical;
	blockWidth /= this->partsHorizontal;

	Windows::UI::Xaml::Thickness margin(blockWidth*left, blockHeight*top, blockWidth*right, blockHeight*bottom);
	element->Margin = margin;
}

void UIManagerShared::setSize(Windows::UI::Xaml::FrameworkElement^ element, int width, int height)
{
	double blockheight;
	double blockwidth;
	double windowsWidth = mainWindow->Bounds.Width;
	double windowsHeight = mainWindow->Bounds.Height;
	blockwidth = windowsWidth;
	blockheight = windowsHeight;
	if (width > 0)
	{
		blockwidth /= this->partsHorizontal;
		blockwidth *= width;
	}
	if (height > 0)
	{
		blockheight /= this->partsVertical;
		blockheight *= height;
	}
	
	element->Width = blockwidth;
	element->Height = blockheight;
}

void UIManagerShared::setTheme()
{
	String^ theme = Settings::getValue("CurrentTheme");
	if (theme == "Default")
		this->setDefaultTheme();
	else
		this->setTheme(theme);
}

void UIManagerShared::setDefaultTheme()
{
	this->themeFolder = nullptr;
	create_task(StorageFile::GetFileFromApplicationUriAsync(ref new Windows::Foundation::Uri("ms-appx:///Resources/DefaultTheme/theme.json"))).then([this](StorageFile^ File)
	{
		this->themeFile = File;
		create_task(StorageFile::GetFileFromApplicationUriAsync(ref new Windows::Foundation::Uri("ms-appx:///Resources/DefaultTheme/texts.json"))).then([this](StorageFile^ File)
		{
			this->themeNamesFile = File;
			appendTheme();
		});
	});
}
void UIManagerShared::setTheme(String^ Name)
{
	StorageFolder^ localFolder = Windows::Storage::ApplicationData::Current->LocalFolder;
	create_task(localFolder->CreateFolderAsync("Themes", Windows::Storage::CreationCollisionOption::OpenIfExists)).then([this, Name](StorageFolder^ themesFolder)
	{
		if (themesFolder != nullptr)
		{
			auto getThemeFileTask = create_task(themesFolder->GetFolderAsync(Name));
			getThemeFileTask.then([this](StorageFolder^ themeFolder)
			{
				if (themeFolder != nullptr)
				{
					this->themeFolder = themeFolder;

					auto getThemeFileTask = create_task(this->themeFolder->GetFileAsync("theme.json"));
					getThemeFileTask.then([this](StorageFile^ themeFile)
					{
						if (themeFile != nullptr)
						{
							this->themeFile = themeFile;
							auto getTextsFileTask = create_task(this->themeFolder->GetFileAsync("texts.json"));
							getTextsFileTask.then([this](StorageFile^ themeNamesFile)
							{
								if (themeNamesFile != nullptr)
								{
									this->themeNamesFile = themeNamesFile;
									this->appendTheme();
								}
							})
							.then([this](task<void> t)
							{
								try
								{
									t.get();
								}
								catch (Platform::COMException^ e)
								{
									this->setDefaultTheme();
								}
							});
						}
					})
						.then([this](task<void> t)
					{
						try
						{
							t.get();
						}
						catch (Platform::COMException^ e)
						{
							this->setDefaultTheme();
						}
					});
				}
			})
			.then([this](task<void> t)
			{
				try
				{
					t.get();
				}
				catch (Platform::COMException^ e)
				{
					this->setDefaultTheme();
				}
			});
		}
	});
}

void UIManagerShared::setDefaultValues(Windows::UI::Xaml::Controls::Button^ element)
{
	//Todo Abhängig von der Orientierung machen
	element->Style = (Windows::UI::Xaml::Style^)MediacenterUniversal::App::Current->Resources->Lookup("AppBarButtonStyle");

	Windows::UI::Xaml::Media::SolidColorBrush^ Temp_Color;
	Temp_Color = ref new Windows::UI::Xaml::Media::SolidColorBrush();
	Temp_Color->Color = Windows::UI::Colors::White;
	element->Foreground = Temp_Color;

	element->Visibility = Windows::UI::Xaml::Visibility::Visible;
}
void UIManagerShared::setForegroundColor(Windows::UI::Xaml::FrameworkElement^ element, String^ color)
{
	Windows::UI::Xaml::Media::SolidColorBrush^ Temp_Color;
	Temp_Color = ref new Windows::UI::Xaml::Media::SolidColorBrush();
	Temp_Color->Color = getColorFromString(color);
	try
	{
		((Button^)element)->Foreground = Temp_Color;
	}
	catch (Exception^ ex)
	{

	}
}

void UIManagerShared::setVertikalAlign(Windows::UI::Xaml::FrameworkElement^ element, String^ align)
{
	if (align == "center" || align == "Center")
		element->VerticalAlignment = Windows::UI::Xaml::VerticalAlignment::Center;
	else if (align == "bottom" || align == "Bottom")
		element->VerticalAlignment = Windows::UI::Xaml::VerticalAlignment::Bottom;
	else if (align == "strech" || align == "Strech")
		element->VerticalAlignment = Windows::UI::Xaml::VerticalAlignment::Stretch;
	else
		element->VerticalAlignment = Windows::UI::Xaml::VerticalAlignment::Top;
}

void UIManagerShared::setHorizontalAlign(Windows::UI::Xaml::FrameworkElement^ element, String^ align)
{
	if (align == "center" || align == "Center")
		element->HorizontalAlignment = Windows::UI::Xaml::HorizontalAlignment::Center;
	else if (align == "right" || align == "Right")
		element->HorizontalAlignment = Windows::UI::Xaml::HorizontalAlignment::Right;
	else if (align == "strech" || align == "Strech")
		element->HorizontalAlignment = Windows::UI::Xaml::HorizontalAlignment::Stretch;
	else
		element->HorizontalAlignment = Windows::UI::Xaml::HorizontalAlignment::Left;
}

void UIManagerShared::setZIndex(Windows::UI::Xaml::FrameworkElement^ element, int layer)
{
	Windows::UI::Xaml::Controls::Canvas::SetZIndex(element, layer);
}

void UIManagerShared::setBackground(String^ key)
{
		Windows::UI::Xaml::Media::SolidColorBrush^ Brush = ref new Windows::UI::Xaml::Media::SolidColorBrush();
		Brush->Color = getColorFromString("Black");
		setBrush(Brush, key);
}

void UIManagerShared::setBackground(String^ key, Windows::Data::Json::JsonObject^ jsonObject)
{
	
	if (jsonObject->HasKey("type"))
	{

		String^ type = jsonObject->GetNamedString("type");
		if (type == "color")
		{
			if (jsonObject->HasKey("color"))
			{
				String^ color = jsonObject->GetNamedString("color");
				Windows::UI::Xaml::Media::SolidColorBrush^ Brush = ref new Windows::UI::Xaml::Media::SolidColorBrush();
				Brush->Color = getColorFromString(color);
				setBrush(Brush, key);
			}
		}
		else if (type == "image")
		{
			if (jsonObject->HasKey("source"))
			{
				if (this->themeFolder == nullptr)
				{
					create_task(StorageFile::GetFileFromApplicationUriAsync(ref new Windows::Foundation::Uri("ms-appx:///Resources/DefaultTheme/Background.jpg"))).then([this, key](StorageFile^ File)
					{
						setBackgroundImage(key, File);
					});
				}
				else
				{
					String^ Filename=jsonObject->GetNamedString("source");
					auto getTextsFileTask = create_task(this->themeFolder->GetFileAsync(Filename));
					getTextsFileTask.then([this,key](StorageFile^ BackgroundImage)
					{
						if (BackgroundImage != nullptr)
						{
							setBackgroundImage(key, BackgroundImage);
						}
					})
						.then([this](task<void> t)
					{
						try
						{
							t.get();
						}
						catch (Platform::COMException^ e)
						{
							
						}
					});
				}
			}
		}
		else if (type == "gradient")
		{
			//todo Add more gradient types
			if (jsonObject->HasKey("gradientType"))
			{
				String^ type = jsonObject->GetNamedString("gradientType");
				if (type == "linear" || type == "Linear")
				{
					Windows::UI::Xaml::Media::LinearGradientBrush^ Brush = ref new Windows::UI::Xaml::Media::LinearGradientBrush();
					Windows::UI::Xaml::Media::GradientStopCollection^ gradients = ref new Windows::UI::Xaml::Media::GradientStopCollection();
					if (jsonObject->HasKey("gradientStops"))
					{
						Windows::Foundation::Collections::IMapView<String^, IJsonValue^>^ test = jsonObject->GetNamedObject("gradientStops")->GetView();
						std::for_each(begin(test), end(test), [this, gradients](IKeyValuePair<Platform::String^, IJsonValue^>^ pair)
						{
							// do stuff
							auto key = pair->Key;
							JsonObject^ gradientStopObject = pair->Value->GetObject();
							Windows::UI::Xaml::Media::GradientStop^ gradientStop = ref new Windows::UI::Xaml::Media::GradientStop();
							if (gradientStopObject->HasKey("color"))
								gradientStop->Color = getColorFromString(gradientStopObject->GetNamedString("color"));
							if (gradientStopObject->HasKey("offset"))
								gradientStop->Offset = gradientStopObject->GetNamedNumber("offset");
							gradients->Append(gradientStop);
						});
					} 

					Windows::Foundation::Point startPoint(0, 0);
					Windows::Foundation::Point endPoint(0, 1);
					
					if (jsonObject->HasKey("startPointX"))
					{
						double startPointX = jsonObject->GetNamedNumber("startPointX");
						startPoint.X = (float)startPointX;
					}
					if (jsonObject->HasKey("startPointY"))
					{
						double startPointY = jsonObject->GetNamedNumber("startPointY");
						startPoint.Y = (float)startPointY;
					}
					if (jsonObject->HasKey("endPointX"))
					{
						double endPointX = jsonObject->GetNamedNumber("endPointX");
						endPoint.X = (float)endPointX;
					}
					if (jsonObject->HasKey("endPointY"))
					{
						double endPointY = jsonObject->GetNamedNumber("endPointY");
						endPoint.Y = (float)endPointY;
					}
					
					Brush->StartPoint = startPoint;
					Brush->EndPoint = endPoint;

					Brush->GradientStops = gradients;
					setBrush(Brush,key);
				}
			}
		}

	}
}

void UIManagerShared::setBackgroundImage(String^ key, StorageFile^ BackgroundImage)
{
	create_task(BackgroundImage->OpenAsync(Windows::Storage::FileAccessMode::Read)).then([this, key](Streams::IRandomAccessStream^ fileStream)
	{
		auto image = ref new Windows::UI::Xaml::Media::Imaging::BitmapImage();
		image->SetSource(fileStream);

		Windows::UI::Xaml::Media::ImageBrush^ Brush = ref new Windows::UI::Xaml::Media::ImageBrush();
		Brush->ImageSource = image;
		Brush->Stretch = Windows::UI::Xaml::Media::Stretch::UniformToFill;
		setBrush(Brush, key);
	});
}

void UIManagerShared::setBrush(Windows::UI::Xaml::Media::Brush^ Brush, String^ elementKey)
{
	Windows::UI::Xaml::FrameworkElement^ element = this->getElement(elementKey);
	String^ type = isType(elementKey);
	if (type == "Grid")
		((Grid^)element)->Background = Brush;
	else if (type == "Button")
		((Button^)element)->Background = Brush;
	else if (type == "Slider")
		((Slider^)element)->Background = Brush;
}

void UIManager::UIManagerShared::OnSizeChanged(Platform::Object ^sender, Windows::UI::Core::WindowSizeChangedEventArgs ^e)
{
	appendStyles(this->themeObject->GetNamedValue("styles"));
}

Windows::UI::Xaml::FrameworkElement^ UIManager::UIManagerShared::getElement(String^ name)
{
	if (this->mediaPlayerUIElements->HasKey(name))
		return this->mediaPlayerUIElements->Lookup(name);
	else
		return nullptr;
}

String^ UIManagerShared::isType(String^ key)
{
	const wchar_t* name = key->Begin();
	if (name[0] == 'b' ||name[0] == 'B')
	{
		if (name[1] == 'u' && name[2] == 't' && name[3] == 't' && name[4] == 'o' && name[5] == 'n')
		{
			return "Button";
		}
	}
	else if (name[0] == 'g' || name[0] == 'G')
	{
		if (name[1] == 'r' && name[2] == 'i' && name[3] == 'd')
		{
			return "Grid";
		}
	}
	else if (name[0] == 's' || name[0] == 'S')
	{
		if (name[1] == 'l' && name[2] == 'i' && name[3] == 'd' && name[4] == 'e' && name[5] == 'r')
		{
			return "Slider";
		}
	}
	else if (name[0] == 'm' || name[0] == 'M')
	{
		if (name[1] == 'e' && name[2] == 'd' && name[3] == 'i' && name[4] == 'a')
		{
			return "MediaElement";
		}
	}
	else if (name[0] == 't' || name[0] == 'T')
	{
		if (name[1] == 'e' && name[2] == 'x' && name[3] == 't')
		{
			return "TextBlock";
		}
	}
	else if (name[0] == 'i' || name[0] == 'I')
	{
		if (name[1] == 'm' && name[2] == 'a' && name[3] == 'g' && name[4] == 'e')
		{
			return "Image";
		}
	}
	return "Unknown";
}

Windows::UI::Color UIManagerShared::getColorFromString(String^ colorString)
{
	if (colorString == "aliceBlue" || colorString == "AliceBlue")
		return Windows::UI::Colors::AliceBlue;
	else if (colorString == "antiqueWhite" || colorString == "AntiqueWhite")
		return Windows::UI::Colors::AntiqueWhite;
	else if (colorString == "aqua" || colorString == "Aqua")
		return Windows::UI::Colors::Aqua;
	else if (colorString == "aquamarine" || colorString == "Aquamarine")
		return Windows::UI::Colors::Aquamarine;
	else if (colorString == "azure" || colorString == "Azure")
		return Windows::UI::Colors::Azure;

	else if (colorString == "beige" || colorString == "Beige")
		return Windows::UI::Colors::Beige;
	else if (colorString == "bisque" || colorString == "Bisque")
		return Windows::UI::Colors::Bisque;
	else if (colorString == "black" || colorString == "Black")
		return Windows::UI::Colors::Black;
	else if (colorString == "blanchedAlmond" || colorString == "BlanchedAlmond")
		return Windows::UI::Colors::BlanchedAlmond;
	else if (colorString == "blue" || colorString == "Blue")
		return Windows::UI::Colors::Blue;
	else if (colorString == "blueViolet" || colorString == "BlueViolet")
		return Windows::UI::Colors::BlueViolet;
	else if (colorString == "brown" || colorString == "Brown")
		return Windows::UI::Colors::Brown;
	else if (colorString == "burlyWood" || colorString == "BurlyWood")
		return Windows::UI::Colors::BurlyWood;

	else if (colorString == "cadetBlue" || colorString == "CadetBlue")
		return Windows::UI::Colors::CadetBlue;
	else if (colorString == "chartreuse" || colorString == "Chartreuse")
		return Windows::UI::Colors::Chartreuse;
	else if (colorString == "chocolate" || colorString == "Chocolate")
		return Windows::UI::Colors::Chocolate;
	else if (colorString == "coral" || colorString == "Coral")
		return Windows::UI::Colors::Coral;
	else if (colorString == "cornflowerBlue" || colorString == "CornflowerBlue")
		return Windows::UI::Colors::CornflowerBlue;
	else if (colorString == "cornsilk" || colorString == "Cornsilk")
		return Windows::UI::Colors::Cornsilk;
	else if (colorString == "crimson" || colorString == "Crimson")
		return Windows::UI::Colors::Crimson;
	else if (colorString == "cyan" || colorString == "Cyan")
		return Windows::UI::Colors::Cyan;

	else if (colorString == "darkBlue" || colorString == "DarkBlue")
		return Windows::UI::Colors::DarkBlue;
	else if (colorString == "darkCyan" || colorString == "DarkCyan")
		return Windows::UI::Colors::DarkCyan;
	else if (colorString == "darkGoldenrod" || colorString == "DarkGoldenrod")
		return Windows::UI::Colors::DarkGoldenrod;
	else if (colorString == "darkGray" || colorString == "DarkGray")
		return Windows::UI::Colors::DarkGray;
	else if (colorString == "darkGreen" || colorString == "DarkGreen")
		return Windows::UI::Colors::DarkGreen;
	else if (colorString == "darkKhaki" || colorString == "DarkKhaki")
		return Windows::UI::Colors::DarkKhaki;
	else if (colorString == "darkMagenta" || colorString == "DarkMagenta")
		return Windows::UI::Colors::DarkMagenta;
	else if (colorString == "darkOliveGreen" || colorString == "DarkOliveGreen")
		return Windows::UI::Colors::DarkOliveGreen;
	else if (colorString == "darkOrange" || colorString == "DarkOrange")
		return Windows::UI::Colors::DarkOrange;
	else if (colorString == "darkOrchid" || colorString == "DarkOrchid")
		return Windows::UI::Colors::DarkOrchid;
	else if (colorString == "darkRed" || colorString == "DarkRed")
		return Windows::UI::Colors::DarkRed;
	else if (colorString == "darkSalmon" || colorString == "DarkSalmon")
		return Windows::UI::Colors::DarkSalmon;
	else if (colorString == "darkSeaGreen" || colorString == "DarkSeaGreen")
		return Windows::UI::Colors::DarkSeaGreen;
	else if (colorString == "darkSlateBlue" || colorString == "DarkSlateBlue")
		return Windows::UI::Colors::DarkSlateBlue;
	else if (colorString == "darkSlateGray" || colorString == "DarkSlateGray")
		return Windows::UI::Colors::DarkSlateGray;
	else if (colorString == "darkTurquoise" || colorString == "DarkTurquoise")
		return Windows::UI::Colors::DarkTurquoise;
	else if (colorString == "darkViolet" || colorString == "DarkViolet")
		return Windows::UI::Colors::DarkViolet;
	else if (colorString == "deepPink" || colorString == "DeepPink")
		return Windows::UI::Colors::DeepPink;
	else if (colorString == "deepSkyBlue" || colorString == "DeepSkyBlue")
		return Windows::UI::Colors::DeepSkyBlue;
	else if (colorString == "dimGray" || colorString == "DimGray")
		return Windows::UI::Colors::DimGray;
	else if (colorString == "dodgerBlue" || colorString == "DodgerBlue")
		return Windows::UI::Colors::DodgerBlue;

	else if (colorString == "firebrick" || colorString == "Firebrick")
		return Windows::UI::Colors::Firebrick;
	else if (colorString == "floralWhite" || colorString == "FloralWhite")
		return Windows::UI::Colors::FloralWhite;
	else if (colorString == "forestGreen" || colorString == "ForestGreen")
		return Windows::UI::Colors::ForestGreen;
	else if (colorString == "fuchsia" || colorString == "Fuchsia")
		return Windows::UI::Colors::Fuchsia;

	else if (colorString == "gainsboro" || colorString == "Gainsboro")
		return Windows::UI::Colors::Gainsboro;
	else if (colorString == "ghostWhite" || colorString == "GhostWhite")
		return Windows::UI::Colors::GhostWhite;
	else if (colorString == "gold" || colorString == "Gold")
		return Windows::UI::Colors::Gold;
	else if (colorString == "goldenrod" || colorString == "Goldenrod")
		return Windows::UI::Colors::Goldenrod;
	else if (colorString == "gray" || colorString == "Gray")
		return Windows::UI::Colors::Gray;
	else if (colorString == "green" || colorString == "Green")
		return Windows::UI::Colors::Green;
	else if (colorString == "greenYellow" || colorString == "GreenYellow")
		return Windows::UI::Colors::GreenYellow;

	else if (colorString == "honeydew" || colorString == "Honeydew")
		return Windows::UI::Colors::Honeydew;
	else if (colorString == "hotPink" || colorString == "HotPink")
		return Windows::UI::Colors::HotPink;

	else if (colorString == "indianRed" || colorString == "IndianRed")
		return Windows::UI::Colors::IndianRed;
	else if (colorString == "indigo" || colorString == "Indigo")
		return Windows::UI::Colors::Indigo;
	else if (colorString == "ivory" || colorString == "Ivory")
		return Windows::UI::Colors::Ivory;

	else if (colorString == "khaki" || colorString == "Khaki")
		return Windows::UI::Colors::Khaki;

	else if (colorString == "lavender" || colorString == "Lavender")
		return Windows::UI::Colors::Lavender;
	else if (colorString == "lavenderBlush" || colorString == "LavenderBlush")
		return Windows::UI::Colors::LavenderBlush;
	else if (colorString == "lawnGreen" || colorString == "LawnGreen")
		return Windows::UI::Colors::LawnGreen;
	else if (colorString == "lemonChiffon" || colorString == "LemonChiffon")
		return Windows::UI::Colors::LemonChiffon;
	else if (colorString == "lightBlue" || colorString == "LightBlue")
		return Windows::UI::Colors::LightBlue;
	else if (colorString == "lightCoral" || colorString == "LightCoral")
		return Windows::UI::Colors::LightCoral;
	else if (colorString == "lightCyan" || colorString == "LightCyan")
		return Windows::UI::Colors::LightCyan;
	else if (colorString == "lightGoldenrodYellow" || colorString == "LightGoldenrodYellow")
		return Windows::UI::Colors::LightGoldenrodYellow;
	else if (colorString == "lightGray" || colorString == "LightGray")
		return Windows::UI::Colors::LightGray;
	else if (colorString == "lightGreen" || colorString == "LightGreen")
		return Windows::UI::Colors::LightGreen;
	else if (colorString == "lightPink" || colorString == "LightPink")
		return Windows::UI::Colors::LightPink;
	else if (colorString == "lightSalmon" || colorString == "LightSalmon")
		return Windows::UI::Colors::LightSalmon;
	else if (colorString == "lightSeaGreen" || colorString == "LightSeaGreen")
		return Windows::UI::Colors::LightSeaGreen;
	else if (colorString == "lightSkyBlue" || colorString == "LightSkyBlue")
		return Windows::UI::Colors::LightSkyBlue;
	else if (colorString == "LightSlateGray" || colorString == "LightSlateGray")
		return Windows::UI::Colors::LightSlateGray;
	else if (colorString == "lightSteelBlue" || colorString == "LightSteelBlue")
		return Windows::UI::Colors::LightSteelBlue;
	else if (colorString == "lightYellow" || colorString == "LightYellow")
		return Windows::UI::Colors::LightYellow;
	else if (colorString == "lime" || colorString == "Lime")
		return Windows::UI::Colors::Lime;
	else if (colorString == "limeGreen" || colorString == "LimeGreen")
		return Windows::UI::Colors::LimeGreen;
	else if (colorString == "linen" || colorString == "Linen")
		return Windows::UI::Colors::Linen;

	else if (colorString == "magenta" || colorString == "Magenta")
		return Windows::UI::Colors::Magenta;
	else if (colorString == "maroon" || colorString == "Maroon")
		return Windows::UI::Colors::Maroon;
	else if (colorString == "mediumAquamarine" || colorString == "MediumAquamarine")
		return Windows::UI::Colors::MediumAquamarine;
	else if (colorString == "mediumBlue" || colorString == "MediumBlue")
		return Windows::UI::Colors::MediumBlue;
	else if (colorString == "mediumOrchid" || colorString == "MediumOrchid")
		return Windows::UI::Colors::MediumOrchid;
	else if (colorString == "mediumPurple" || colorString == "MediumPurple")
		return Windows::UI::Colors::MediumPurple;
	else if (colorString == "mediumSeaGreen" || colorString == "MediumSeaGreen")
		return Windows::UI::Colors::MediumSeaGreen;
	else if (colorString == "mediumSlateBlue" || colorString == "MediumSlateBlue")
		return Windows::UI::Colors::MediumSlateBlue;
	else if (colorString == "mediumSpringGreen" || colorString == "MediumSpringGreen")
		return Windows::UI::Colors::MediumSpringGreen;
	else if (colorString == "mediumTurquoise" || colorString == "MediumTurquoise")
		return Windows::UI::Colors::MediumTurquoise;
	else if (colorString == "mediumVioletRed" || colorString == "MediumVioletRed")
		return Windows::UI::Colors::MediumVioletRed;
	else if (colorString == "midnightBlue" || colorString == "MidnightBlue")
		return Windows::UI::Colors::MidnightBlue;
	else if (colorString == "mintCream" || colorString == "MintCream")
		return Windows::UI::Colors::MintCream;
	else if (colorString == "mistyRose" || colorString == "MistyRose")
		return Windows::UI::Colors::MistyRose;
	else if (colorString == "moccasin" || colorString == "Moccasin")
		return Windows::UI::Colors::Moccasin;

	else if (colorString == "navajoWhite" || colorString == "NavajoWhite")
		return Windows::UI::Colors::NavajoWhite;
	else if (colorString == "navy" || colorString == "Navy")
		return Windows::UI::Colors::Navy;

	else if (colorString == "oldLace" || colorString == "OldLace")
		return Windows::UI::Colors::OldLace;
	else if (colorString == "olive" || colorString == "Olive")
		return Windows::UI::Colors::Olive;
	else if (colorString == "oliveDrab" || colorString == "OliveDrab")
		return Windows::UI::Colors::OliveDrab;
	else if (colorString == "orange" || colorString == "Orange")
		return Windows::UI::Colors::Orange;
	else if (colorString == "orangeRed" || colorString == "OrangeRed")
		return Windows::UI::Colors::OrangeRed;
	else if (colorString == "orchid" || colorString == "Orchid")
		return Windows::UI::Colors::Orchid;

	else if (colorString == "paleGoldenrod" || colorString == "PaleGoldenrod")
		return Windows::UI::Colors::PaleGoldenrod;
	else if (colorString == "paleGreen" || colorString == "PaleGreen")
		return Windows::UI::Colors::PaleGreen;
	else if (colorString == "paleTurquoise" || colorString == "PaleTurquoise")
		return Windows::UI::Colors::PaleTurquoise;
	else if (colorString == "paleVioletRed" || colorString == "PaleVioletRed")
		return Windows::UI::Colors::PaleVioletRed;
	else if (colorString == "papayaWhip" || colorString == "PapayaWhip")
		return Windows::UI::Colors::PapayaWhip;
	else if (colorString == "peachPuff" || colorString == "PeachPuff")
		return Windows::UI::Colors::PeachPuff;
	else if (colorString == "peru" || colorString == "Peru")
		return Windows::UI::Colors::Peru;
	else if (colorString == "pink" || colorString == "Pink")
		return Windows::UI::Colors::Pink;
	else if (colorString == "plum" || colorString == "Plum")
		return Windows::UI::Colors::Plum;
	else if (colorString == "powderBlue" || colorString == "PowderBlue")
		return Windows::UI::Colors::PowderBlue;
	else if (colorString == "purple" || colorString == "Purple")
		return Windows::UI::Colors::Purple;

	else if (colorString == "red" || colorString == "Red")
		return Windows::UI::Colors::Red;
	else if (colorString == "rosyBrown" || colorString == "RosyBrown")
		return Windows::UI::Colors::RosyBrown;
	else if (colorString == "royalBlue" || colorString == "RoyalBlue")
		return Windows::UI::Colors::RoyalBlue;

	//Break because of to many else ifs
	if (colorString == "saddleBrown" || colorString == "SaddleBrown")
		return Windows::UI::Colors::SaddleBrown;
	else if (colorString == "salmon" || colorString == "Salmon")
		return Windows::UI::Colors::Salmon;
	else if (colorString == "sandyBrown" || colorString == "SandyBrown")
		return Windows::UI::Colors::SandyBrown;
	else if (colorString == "seaGreen" || colorString == "SeaGreen")
		return Windows::UI::Colors::SeaGreen;
	else if (colorString == "seaShell" || colorString == "SeaShell")
		return Windows::UI::Colors::SeaShell;
	else if (colorString == "sienna" || colorString == "Sienna")
		return Windows::UI::Colors::Sienna;
	else if (colorString == "silver" || colorString == "Silver")
		return Windows::UI::Colors::Silver;
	else if (colorString == "skyBlue" || colorString == "SkyBlue")
		return Windows::UI::Colors::SkyBlue;
	else if (colorString == "slateBlue" || colorString == "SlateBlue")
		return Windows::UI::Colors::SlateBlue;
	else if (colorString == "slateGray" || colorString == "SlateGray")
		return Windows::UI::Colors::SlateGray;
	else if (colorString == "snow" || colorString == "Snow")
		return Windows::UI::Colors::Snow;
	else if (colorString == "springGreen" || colorString == "SpringGreen")
		return Windows::UI::Colors::SpringGreen;
	else if (colorString == "steelBlue" || colorString == "SteelBlue")
		return Windows::UI::Colors::SteelBlue;

	else if (colorString == "tan" || colorString == "Tan")
		return Windows::UI::Colors::Tan;
	else if (colorString == "teal" || colorString == "Teal")
		return Windows::UI::Colors::Teal;
	else if (colorString == "thistle" || colorString == "Thistle")
		return Windows::UI::Colors::Thistle;
	else if (colorString == "tomato" || colorString == "Tomato")
		return Windows::UI::Colors::Tomato;
	else if (colorString == "transparent" || colorString == "Transparent")
		return Windows::UI::Colors::Transparent;
	else if (colorString == "turquoise" || colorString == "Turquoise")
		return Windows::UI::Colors::Turquoise;

	else if (colorString == "violet" || colorString == "Violet")
		return Windows::UI::Colors::Violet;

	else if (colorString == "wheat" || colorString == "Wheat")
		return Windows::UI::Colors::Wheat;
	else if (colorString == "white" || colorString == "White")
		return Windows::UI::Colors::White;
	else if (colorString == "whiteSmoke" || colorString == "WhiteSmoke")
		return Windows::UI::Colors::WhiteSmoke;

	else if (colorString == "yellow" || colorString == "Yellow")
		return Windows::UI::Colors::Yellow;
	else if (colorString == "yellowGreen" || colorString == "YellowGreen")
		return Windows::UI::Colors::YellowGreen;
	else
		return Windows::UI::Colors::White;

}


void UIManagerShared::playlistButtonTapped(Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e)
{
	//this->setTheme("theme2");
	
}


/*Todo
window size changed only do needed changes
themes:
	implement background(other gradient)
	implement colors better
implement landscape and ****
more error handlich(json for example)
*/