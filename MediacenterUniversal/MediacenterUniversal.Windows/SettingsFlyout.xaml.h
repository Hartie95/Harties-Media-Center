//
// SettingsFlyout.xaml.h
// Deklaration der SettingsFlyout-Klasse
//

#pragma once

#include "SettingsFlyout.g.h"

namespace MediacenterUniversal
{
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class SettingsFlyout sealed
	{
	public:
		SettingsFlyout();
	private:
		void themeBox_SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e);

		int init;
	};
}
