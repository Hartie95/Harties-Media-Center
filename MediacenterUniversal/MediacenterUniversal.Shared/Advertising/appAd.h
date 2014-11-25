#pragma once
using namespace Microsoft::Advertising::WinRT::UI;
ref class appAd sealed
{
public:
	appAd();
	void showAd();
	void hideAd();
	AdControl^ getAd();
	void moveAd(Windows::UI::Xaml::Thickness margin);
	void moveAd(Windows::UI::Xaml::HorizontalAlignment horizontalAlignment, Windows::UI::Xaml::VerticalAlignment verticalAlignment);
	void moveAd(int margintop, int marginleft, int marginbottom, int marginright, Windows::UI::Xaml::HorizontalAlignment horizontalAlignment, Windows::UI::Xaml::VerticalAlignment verticalAlignment);
	void moveAd(int margintop, int marginleft, int marginbottom, int marginright);
	void moveAd(int marginvalue, Windows::UI::Xaml::HorizontalAlignment horizontalAlignment, Windows::UI::Xaml::VerticalAlignment verticalAlignment);
	void moveAd(int marginvalue);
	void moveAd(Windows::UI::Xaml::Thickness margin, Windows::UI::Xaml::HorizontalAlignment horizontalAlignment, Windows::UI::Xaml::VerticalAlignment verticalAlignment);


private:
	AdControl^ ad;
};

