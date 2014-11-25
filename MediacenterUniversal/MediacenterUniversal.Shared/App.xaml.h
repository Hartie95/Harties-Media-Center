//
// App.xaml.h
// Deklaration der App-Klasse
//

#pragma once

#include "App.g.h"

namespace MediacenterUniversal
{
	/// <summary>
	/// Stellt das anwendungsspezifische Verhalten bereit, um die Standardanwendungsklasse zu ergänzen.
	/// </summary>
	ref class App sealed
	{
	public:
		App();

		virtual void OnLaunched(Windows::ApplicationModel::Activation::LaunchActivatedEventArgs^ e) override;

	private:
#if WINAPI_FAMILY==WINAPI_FAMILY_PHONE_APP
		Windows::UI::Xaml::Media::Animation::TransitionCollection^ _transitions;
		Windows::Foundation::EventRegistrationToken _firstNavigatedToken;

		void RootFrame_FirstNavigated(Platform::Object^ sender, Windows::UI::Xaml::Navigation::NavigationEventArgs^ e);
#endif

		void OnSuspending(Platform::Object^ sender, Windows::ApplicationModel::SuspendingEventArgs^ e);
	};
}
