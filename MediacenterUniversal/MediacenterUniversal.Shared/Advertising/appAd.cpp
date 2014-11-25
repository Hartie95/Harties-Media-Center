#include "pch.h"
#include "appAd.h"


appAd::appAd()
{
	this->ad = ref new AdControl();
	this->ad->ApplicationId = "eed88a0f-a18e-4106-a56c-d4b2891d8bb4";
	this->ad->AdUnitId = "10730525";
	this->ad->Width = 728;
	this->ad->Height = 90;
	this->moveAd(0, 0, 20, 20, Windows::UI::Xaml::HorizontalAlignment::Center, Windows::UI::Xaml::VerticalAlignment::Bottom);
	this->hideAd();
}
void appAd::showAd()
{
	this->ad->Visibility = Windows::UI::Xaml::Visibility::Visible;
	this->ad->IsEnabled = true;
	/*if (this->ad->IsAutoRefreshEnabled != true)
		this->ad->IsAutoRefreshEnabled = true;*/
	this->ad->Resume();
	
}
void appAd::hideAd()
{
	this->ad->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
	/*if(this->ad->IsAutoRefreshEnabled == true)
		this->ad->IsAutoRefreshEnabled = false;*/
	this->ad->IsEnabled = false;
}
AdControl^ appAd::getAd()
{
	return this->ad;
}
void appAd::moveAd(Windows::UI::Xaml::Thickness margin)
{
	this->ad->Margin = margin;
}
void appAd::moveAd(Windows::UI::Xaml::HorizontalAlignment horizontalAlignment, Windows::UI::Xaml::VerticalAlignment verticalAlignment)
{
	this->ad->VerticalAlignment = verticalAlignment;
	this->ad->HorizontalAlignment = horizontalAlignment;
}
void appAd::moveAd(int margintop, int marginleft, int marginbottom, int marginright, Windows::UI::Xaml::HorizontalAlignment horizontalAlignment, Windows::UI::Xaml::VerticalAlignment verticalAlignment)
{
	Windows::UI::Xaml::Thickness* margin = new Windows::UI::Xaml::Thickness(marginleft,margintop,marginright,marginbottom);
	this->moveAd(*margin);
	moveAd(horizontalAlignment, verticalAlignment);
}
void appAd::moveAd(int margintop, int marginleft, int marginbottom, int marginright)
{
	Windows::UI::Xaml::Thickness* margin = new Windows::UI::Xaml::Thickness(marginleft, margintop, marginright, marginbottom);
	this->moveAd(*margin);
}
void appAd::moveAd(int marginvalue, Windows::UI::Xaml::HorizontalAlignment horizontalAlignment, Windows::UI::Xaml::VerticalAlignment verticalAlignment)
{
	Windows::UI::Xaml::Thickness* margin = new Windows::UI::Xaml::Thickness(marginvalue);
	this->moveAd(*margin);
	moveAd(horizontalAlignment, verticalAlignment);
}
void appAd::moveAd(int marginvalue)
{
	Windows::UI::Xaml::Thickness* margin = new Windows::UI::Xaml::Thickness(marginvalue);
	this->moveAd(*margin);
}
void appAd::moveAd(Windows::UI::Xaml::Thickness margin, Windows::UI::Xaml::HorizontalAlignment horizontalAlignment, Windows::UI::Xaml::VerticalAlignment verticalAlignment)
{
	this->moveAd(margin);
	moveAd(horizontalAlignment, verticalAlignment);
}
