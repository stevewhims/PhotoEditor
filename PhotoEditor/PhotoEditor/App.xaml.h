#pragma once

#include "App.xaml.g.h"

namespace winrt::PhotoEditor::implementation
{
    struct App : AppT<App>
    {
        App();
        Microsoft::UI::Xaml::Controls::Frame CreateRootFrame();

        void OnLaunched(Microsoft::UI::Xaml::LaunchActivatedEventArgs const&);
        void OnNavigationFailed(IInspectable const&, Microsoft::UI::Xaml::Navigation::NavigationFailedEventArgs const&);
        void OnSuspending(IInspectable const&, Windows::ApplicationModel::SuspendingEventArgs const&);

        static HWND WindowHandle() { return m_hWnd; }
        static winrt::Microsoft::UI::Xaml::Window Window() { return window; };

    private:
        static HWND m_hWnd;
        static winrt::Microsoft::UI::Xaml::Window window;
    };
}
