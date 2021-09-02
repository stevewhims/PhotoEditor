# A Windows App SDK 1.0 [Experimental] port of the UWP Photo Editor sample app (C++/WinRT)

This repo contains the result of taking the [UWP Photo Editor sample app](https://docs.microsoft.com/samples/microsoft/windows-appsample-photo-editor/photo-editor-cwinrt-sample-application/), and porting it to the Windows App SDK 1.0 [Experimental].

Begin by [cloning the UWP sample app's repo](https://github.com/microsoft/windows-appsample-photo-editor/tree/master/), and opening the solution in [Visual Studio](https://visualstudio.microsoft.com/downloads/).

## Install the Windows App SDK 1.0 [Experimental] VSIX

Download the VSIX installer from [Experimental release channel for the Windows App SDK](https://docs.microsoft.com/windows/apps/windows-app-sdk/experimental-channel#version-10-experimental-100-experimental1), and run to install it.

## Create a new project

In Visual Studio, create a new project from the **[Experimental] Blank App, Packaged with WAP (WinUI 3 in Desktop)** project template. You can find that project template in the **Create a new project** dialog by choosing language: *C++*; platform: *Windows App SDK*; project type: *WinUI*. Name the project *PhotoEditor*, uncheck **Place solution and project in the same directory**, and target the most recent release (not preview) of the client operating system (19041 at time of writing).

You'll now see two projects in **Solution Explorer**&mdash;one is qualified as **(Desktop)**, and the other as **(Package)**.

## The overall porting strategy

### Port code with the least dependencies first

One perhaps obvious approach might be to begin by porting **MainPage**; since that's such an important and prominent piece of the app. But we'd soon realize that **MainPage** has a dependency on the **DetailPage** view; and then that **DetailPage** has a dependency on the **Photo** model. If we were to follow that path, then we might be constantly interrupting ourselves (switching over to work on each newly discovered dependency). Certainly we wouldn't expect to get a clean *build* until we'd chased down every dependency, and essentially ported the whole project in one giant leap.

If on the other hand we begin with the piece of the project that doesn't depend on anything else, and work out from there (from least- to most-dependent piece), then we'll be able to focus on each piece one at a time. And we should also be able to achieve a clean build after porting each piece, to confirm that the porting process is staying on track. So for this walkthrough we'll take this second approach.

* We'll begin by copying over the asset files.
* Then we'll port the **Photo** model.
* Next we'll port the **App** class (since that needs some members adding to it that **DetailPage** and **MainPage** will depend on).
* Then we'll begin porting the views, starting with **DetailPage** first.
* And we'll finish up by porting the **MainPage** view.

> [!NOTE]
> I'll be referring to the UWP version of the sample project (the one that you cloned from its [repo](https://github.com/microsoft/windows-appsample-photo-editor/tree/master/)) as the *source* solution/project. I'll be referring to the Windows App SDK version as the *target* solution. In addition, to avoid any ambiguity, in the *target* solution I'll be referring to the target *Desktop* project, and to the target *Package* project.

I'll be using the naming just described because those projects represent the *source* and the *target*, respectively, of the porting process.

### Copy files, or copy file contents?

Naturally, we'll be copying over asset *files*; and not asset file *contents*. But what about source code files? Let's take the **Photo** runtime class as an example. **Photo** is made up of the source code files `Photo.idl`, `Photo.h`, and `Photo.cpp`.

* So should we use Visual Studio to create new `Photo.idl`, `Photo.h`, and `Photo.cpp` files in the target project, and then copy the *contents* of those three files over from the source project to the target project?
* Or should we copy over *the files themselves* (using **File Explorer**), and then add the copies to the target project?

It's your choice. In this walkthrough we'll go with the option of copying over *the source code files themselves* using **File Explorer**. But if you prefer to copy file *contents* over, then see [Appendix: copying the contents of the **Photo** model's files](#appendix-copying-the-contents-of-the-photo-models-files) section for an example of how you could do that for **Photo** (and you could then apply a similar procedure to other types in the project). That option does involve a lot more steps, though.

## Copy asset files

In your clone of the source project, in **File Explorer**, locate the folder **Windows-appsample-photo-editor** > **PhotoEditor** > **Assets**. You'll find eight asset files in that folder. Select those eight files, and copy them to the clipboard.

Also in **File Explorer**, now locate the corresponding folder in the target *Package* project that you created. The path to that folder is **PhotoEditor** > **PhotoEditor** > **PhotoEditor (Package)** > **Images**. Paste into that folder the asset files that you just copied, and accept the prompt to replace the seven files that already exist in the destination.

In your target *Package* project in Visual Studio, in **Solution Explorer**, expand the **Images** folder. Add to that folder the existing `bg1.png` asset file that you just pasted. You can hover the mouse pointer over the asset files. A thumbnail preview will appear for each, confirming that you've replaced/added the asset files correctly.

## Port the **Photo** model

**Photo** is a runtime class that represents a photo. It's a *model* (in the sense of models, views, and viewmodels).

### Copy **Photo** source code files

In your clone of the source project, in **File Explorer**, locate the folder **Windows-appsample-photo-editor** > **PhotoEditor**. In that folder you'll find the three source code files `Photo.idl`, `Photo.h`, and `Photo.cpp`; those files together implement the **Photo** runtime class. Select those three files, and copy them to the clipboard.

In Visual Studio, right-click the target *Desktop* project node, and click **Open Folder in File Explorer**. This opens the target *Desktop* project folder in **File Explorer**. Paste into that folder the three files that you just copied.

Back in **Solution Explorer**, with the target *Desktop* project node selected, make sure that **Show All Files** is toggled on. Right-click the three files that you just pasted, and click **Include In Project**. Toggle **Show All Files** off.

In the source project, in **Solution Explorer**, `Photo.h` and `.cpp` are nested under `Photo.idl` to indicate that they're generated from (dependent upon) it. If you like that arrangement, then you can do the same thing in the target *Desktop* project by manually editing `\PhotoEditor\PhotoEditor\PhotoEditor\PhotoEditor.vcxproj`. Find the following:

```xml
<ClInclude Include="Photo.h" />
```

And replace it with this:

```xml
<ClInclude Include="Photo.h">
  <DependentUpon>Photo.idl</DependentUpon>
</ClInclude>
```

Repeat that for `Photo.cpp`.

### Port **Photo** source code

In `Photo.idl`, search for `Windows.UI.Xaml` (which is the namespace for UWP XAML), and change that to `Microsoft.UI.Xaml` (which is the namespace for WinUI XAML).

In `Photo.cpp`, add `#include "Photo.g.cpp"` to the existing include directives, immediately after `#include "Photo.h"`.

Make the following find/replacement (match case and whole word) in the contents of all of the source code in the files that you just copied and pasted.

* `Windows::UI::Xaml` => `Microsoft::UI::Xaml`

From `pch.h` in the source project, copy the following includes, and paste them into `pch.h` in the target *Desktop* project.

```cppwinrt
#include <winrt/Microsoft.UI.Xaml.Media.Imaging.h>
#include <winrt/Windows.Storage.h>
#include <winrt/Windows.Storage.FileProperties.h>
#include <winrt/Windows.Storage.Streams.h>
```

Confirm that you can build the target solution (but don't run yet).

## Port the **App** class

No changes are necessary to `App.idl` and `App.xaml`. But we do need to edit `App.xaml.h` and `.xaml.cpp` to add some new members to the **App** class. We'll do so in a way that lets us build after each section.

### Making the main window object available

**App** stores the main window object in its private data member *window*. It'll be more convenient later in the porting process if we make that static, and also make its value available via an accessor function. Since we're making *window* static, we'll need to initialize it in `App.xaml.cpp` instead of via the default member initializer that the code is currently using. Here are what those changes look like in `App.xaml.h` and `App.xaml.cpp`.

```cppwinrt
// App.xaml.h
...
struct App : AppT<App>
{
    ...
    static winrt::Microsoft::UI::Xaml::Window Window(){ return window; };

private:
    static winrt::Microsoft::UI::Xaml::Window window;
};
...

// App.xaml.cpp
...
winrt::Microsoft::UI::Xaml::Window App::window{ nullptr };
...
```

### Making the main window handle available

Later in the porting process, we'll need access to the main window's handle (a [**HWND**](/windows/win32/winprog/windows-data-types)). So let's add a data member and an accessor function for that. Add the following code to `App.xaml.h` and `App.xaml.cpp`.

```cppwinrt
// App.xaml.h
...
struct App : AppT<App>
{
    ...
    static HWND WindowHandle() { return m_hWnd; }

private:
    static HWND m_hWnd;
};
...

// App.xaml.cpp
...
HWND App::m_hWnd{ 0 };
...
```

### App::OnNavigationFailed

The members we'll port in the next few sections all exist to support navigation within the app. Let's begin by porting the **OnNavigationFailed** event handler. Just copy the declaration and the definition of that member function from the source project and paste it into the target *Desktop* project (in `App.xaml.h` and `App.xaml.cpp`).

In the code you pasted, change `Windows::UI::Xaml` to `Microsoft::UI::Xaml`.

### App::CreateRootFrame

The source project contains a helper function named **CreateRootFrame**. Copy the declaration and the definition of that helper function from the source project, and paste it into the target *Desktop* project (in `App.xaml.h` and `App.xaml.cpp`).

In the code you pasted, change `Windows::UI::Xaml` to `Microsoft::UI::Xaml`. And change the two occurrences of `Window::Current()` to `window` (which is the name of the data member of the **App** class that we saw earlier).

### App::OnLaunched

The target *Desktop* project already contains an implementation of the **OnLaunched** event handler. Its parameter is a constant reference to a **Microsoft::UI::Xaml::LaunchActivatedEventArgs**, which is correct for the Windows App SDK (contrast that to the source project, which uses **Windows::ApplicationModel::Activation::LaunchActivatedEventArgs**, which is correct for UWP).

We just need to merge the two definitions of **OnLaunched** so that **App::OnLaunched** in `App.xaml.cpp` in the target *Desktop* project looks like this (using `window` instead of `Window::Current()`).

```cppwinrt
void App::OnLaunched(LaunchActivatedEventArgs const&)
{
    window = make<MainWindow>();

    Frame rootFrame = CreateRootFrame();
    if (!rootFrame.Content())
    {
        rootFrame.Navigate(xaml_typename<PhotoEditor::MainPage>());
    }

    window.Activate();
}
```

At the end of that same function, immediately after the line that activates the window, add the following few lines of code to obtain the main window handle, and set the value of `m_hWnd`.

```cppwinrt
auto windowNative{ window.as<::IWindowNative>() };
HWND hWnd{ 0 };
windowNative->get_WindowHandle(&hWnd);
App::m_hWnd = hWnd;
```

The **IWindowNative** interface is defined in `microsoft.ui.xaml.window.h`. So add the following include to `pch.h`.

```cppwinrt
#include <microsoft.ui.xaml.window.h>
```

## Port the **DetailPage** view

**DetailPage** is the class that represents the photo editor page, where Win2D effects are toggled, set, and chained together. You get to the photo editor page by selecting a photo thumbnail on **MainPage**. **DetailPage** is a *model* (in the sense of models, views, and viewmodels).

### Reference the Win2D NuGet package

To support code in **DetailPage**, the source project has a dependency on [Win2D](https://microsoft.github.io/Win2D/WinUI3/html/Introduction.htm). So we'll also need a dependency on Win2D in our target project. But Win2D itself has a dependency on [DWriteCore](https://docs.microsoft.com/windows/win32/directwrite/dwritecore-overview). So let's add the NuGet packages in the following order.

* In the target solution in Visual Studio, click **Tools** > **NuGet Package Manager** > **Manage NuGet Packages for Solution...** > **Browse**, check **Include prerelease**, and type or paste *Microsoft.WindowsAppSDK.DWrite*. Select the *1.0.0-experimental1* item in search results, check the PhotoEditor *Desktop* project, and click **Install** to install the package into that project.
* Repeat those steps for the *Microsoft.Graphics.Win2D* NuGet package. You won't need to check **Include prerelease**, and its version will be *0.8.2.23*, or something similar to that.

### Copy **DetailPage** source code files

In your clone of the source project, in **File Explorer**, locate the folder **Windows-appsample-photo-editor** > **PhotoEditor**. In that folder you'll find the four source code files `DetailPage.idl`, `DetailPage.xaml`, `DetailPage.h`, and `DetailPage.cpp`; those files together implement the **DetailPage** view. Select those four files, and copy them to the clipboard.

In Visual Studio, right-click the target *Desktop* project node, and click **Open Folder in File Explorer**. This opens the target *Desktop* project folder in **File Explorer**. Paste into that folder the four files that you just copied.

In **File Explorer** change the names of `DetailPage.h` and `DetailPage.cpp` to `DetailPage.xaml.h` and `DetailPage.xaml.cpp`, respectively.

Back in **Solution Explorer**, with the target *Desktop* project node selected, make sure that **Show All Files** is toggled on. Right-click the four files that you just pasted (and renamed), and click **Include In Project**. Toggle **Show All Files** off.

In the source project, in **Solution Explorer**, `DetailPage.idl` is nested under `DetailPage.xaml`. If you like that arrangement, then you can do the same thing in the target *Desktop* project by manually editing `\PhotoEditor\PhotoEditor\PhotoEditor\PhotoEditor.vcxproj`. Find the following:

```xml
<Midl Include="DetailPage.idl" />
```

And replace it with this:

```xml
<Midl Include="DetailPage.idl">
  <DependentUpon>DetailPage.xaml</DependentUpon>
</Midl>
```

### Port **DetailPage** source code

In `DetailPage.idl`, search for `Windows.UI.Xaml`, and change that to `Microsoft.UI.Xaml`.

In `DetailPage.xaml.cpp`, change `#include "DetailPage.h"` to `#include "DetailPage.xaml.h"`.

Immediately below that, add `#include "DetailPage.g.cpp"`.

Make the following find/replacements (match case and whole word) in the contents of all of the source code in the files that you just copied and pasted.

* `Windows::UI::Composition` => `Microsoft::UI::Composition`
* `Windows::UI::Xaml` => `Microsoft::UI::Xaml`
* `Window::Current()` => `App::Window()`

For the call to the static **App::Window** method to compile, you'll need to edit `DetailPage.xaml.cpp`, and add `#include "App.xaml.h"` immediately before `#include "Photo.h"`.

From `pch.h` in the source project, copy the following includes, and paste them into `pch.h` in the target *Desktop* project.

```cppwinrt
#include <winrt/Microsoft.Graphics.Canvas.Effects.h>
#include <winrt/Microsoft.UI.Composition.h>
```

Also, at the top of `pch.h`, immediately after `#pragma once`, add this:

```cppwinrt
// This is required because we are using std::min and std::max, otherwise 
// we have a collision with min and max macros being defined elsewhere.
#define NOMINMAX
```

Confirm that you can build the target solution (but don't run yet).

## Port the **MainPage** view

The main page of the app represents the view that you see first when you run the app. It's the page that loads the photos from the **Pictures Library**, and displays a tiled thumbnail view.

### Copy **MainPage** source code files

Just like you did with **DetailPage**, copy over `MainPage.idl`, `MainPage.xaml`, `MainPage.h`, and `MainPage.cpp`. Rename the `.h` and `.cpp` files to `.xaml.h` and `.xaml.cpp`, respectively. Include all four files in the target *Desktop* project.

You can optionally open `\PhotoEditor\PhotoEditor\PhotoEditor\PhotoEditor.vcxproj`, and replace:

```xml
<Midl Include="MainPage.idl" />
```

With:

```xml
<Midl Include="MainPage.idl">
  <DependentUpon>MainPage.xaml</DependentUpon>
</Midl>
```

### Port **MainPage** source code

In `MainPage.idl`, search for `Windows.UI.Xaml`, and change both occurrences to `Microsoft.UI.Xaml`.

In `MainPage.xaml`, immediately after the `<Page.Resources>` element is an opening **RelativePanel** tag. To that tag, add the attribute `x:Name="layoutRoot"`, because we'll be referring to that element by name.

Also in `MainPage.xaml`, find the **Image** element with the `x:Name` of *BackgroundImage*. Change the value of its **Source** attribute to `"Images/bg1.png"`.

In `MainPage.xaml.cpp`, change `#include "MainPage.h"` to `#include "MainPage.xaml.h"`.

Immediately below that, add `#include "MainPage.g.cpp"`.

Also in `MainPage.xaml.cpp`, find the string literal `L"Assets/StoreLogo.png"`, and change that to `L"Images/StoreLogo.png"`.

Still in `MainPage.xaml.cpp`, in the **MainPage::GetItemsAsync** method, immediately after the line `ContentDialog unsupportedFilesDialog{};`, add this line of code.

```cppwinrt
unsupportedFilesDialog.XamlRoot(layoutRoot().XamlRoot());
```

Make the following find/replacements (match case and whole word) in the contents of all of the source code in the files that you just copied and pasted.

* `Windows::UI::Composition` => `Microsoft::UI::Composition`
* `Windows::UI::Xaml` => `Microsoft::UI::Xaml`
* `Window::Current()` => `App::Window()`

For the call to the static **App::Window** method to compile, you'll need to edit `MainPage.xaml.cpp`, and add `#include "App.xaml.h"` immediately before `#include "Photo.h"`.

From `pch.h` in the source project, copy the following includes, and paste them into `pch.h` in the target *Desktop* project.

```cppwinrt
#include <winrt/Microsoft.UI.Xaml.Hosting.h>
#include <winrt/Windows.Storage.Search.h>
```

Confirm that you can build the target solution (but don't run yet).

## Update **MainWindow**

In `MainWindow.xaml`, delete the **StackPanel** and its contents. That leaves only the empty **Window** element.

In `MainWindow.idl`, delete `Int32 MyProperty;`, leaving only the constructor.

In `MainWindow.xaml.h` and `MainWindow.xaml.cpp`, delete the declarations and definitions of **MyProperty** and **myButton_Click**, leaving only the constructor.

Confirm that you can build the target solution (but don't run yet).

## Known issues

### ImageProperties

See GitHub issue [StorageItemContentProperties.GetImagePropertiesAsync causes an access violation when the same code works fine in the UWP version](https://github.com/microsoft/WindowsAppSDK/issues/1141). From that issue, https://microsoft.visualstudio.com/OS/_workitems/edit/35008809 was created.

To get the app to run without crashing, it's necessary to make the following changes. These are not an expected part of the port, so I'm hoping either a) to understand why they're necessary, or b) for them to be fixed in a future release, and so we can remove this section.

MainPage.xaml.cpp:
    // Creates a Photo from Storage file for adding to Photo collection.
    IAsyncOperation<PhotoEditor::Photo> MainPage::LoadImageInfoAsync(StorageFile file)
    {
        //auto properties = co_await file.Properties().GetImagePropertiesAsync();
        auto info = winrt::make<Photo>(nullptr, file, file.DisplayName(), file.DisplayType());
        co_return info;
    }

Photo.cpp:
    hstring Photo::ImageDimensions() const
    {
        return L"Not implemented";

        //wstringstream stringStream;
        //stringStream << m_imageProperties.Width() << " x " << m_imageProperties.Height();
        //wstring str = stringStream.str();
        //return static_cast<hstring>(str);
    }

    void Photo::ImageTitle(hstring const& value)
    {
        //if (m_imageProperties.Title() != value)
        //{
        //    m_imageProperties.Title(value);
        //    auto ignoreResult = m_imageProperties.SavePropertiesAsync();
        //    RaisePropertyChanged(L"ImageTitle");
        //}
    }

Photo.h:
    hstring ImageTitle() const
    {
        return m_imageName;
        // return m_imageProperties.Title() == L"" ? m_imageName : m_imageProperties.Title();
    }

DetailPage.xaml.cpp (and change return type in .idl and .h)
    IAsyncAction DetailPage::FitToScreen()
    {
        auto properties = co_await Item().ImageFile().Properties().GetImagePropertiesAsync();
        auto a = MainImageScroller().ActualWidth() / properties.Width();
        auto b = MainImageScroller().ActualHeight() / properties.Height();
        auto ZoomFactor = static_cast<float>(std::min(a, b));
        MainImageScroller().ChangeView(nullptr, nullptr, ZoomFactor);
    }

## Test the ported app

Confirm that you can build the target solution. Now build and test the app. Select an image, set a zoom level, choose effects, and configure them.

## Appendix: copying the contents of the **Photo** model's files

As we discussed earlier, you have the option to copy over source code *files* themselves, or the *contents* of source code files. We've already shown how to copy over source code *files* themselves. So this section gives an example of copying file *contents*.

In the source project in Visual Studio, locate the folder **PhotoEditor (Universal Windows)** > **Models**. That folder contains the files `Photo.idl`, `Photo.h`, and `Photo.cpp`, which together implement the **Photo** runtime class.

### Add the IDL, and generate stubs

In your target *Desktop* project in Visual Studio, add a new **Midl File (.idl)** item to the project. Name the new item `Photo.idl`. Delete the default contents of `Photo.idl`.

From the source project in Visual Studio, copy the contents of **Models** > `Photo.idl`, and paste them into the `Photo.idl` file that you just added to your target *Desktop* project. In the code you pasted, search for `Windows.UI.Xaml`, and change that to `Microsoft.UI.Xaml`.

Save the file.

> [!IMPORTANT]
> We're about to perform a build of your target solution. The build won't run to completion at this point, but it will get far enough to do necessary work for us.

Now build the target solution. Even though it won't complete, building is necessary now because it'll generate the source code files (stubs) that we need to get started implementing the **Photo** model.

In Visual Studio, right-click the target *Desktop* project node, and click **Open Folder in File Explorer**. This opens the target *Desktop* project folder in **File Explorer**. There, navigate into the `Generated Files\sources` folder (so you'll be in `\PhotoEditor\PhotoEditor\PhotoEditor\Generated Files\sources`). Copy the stub files `Photo.h` and `.cpp`, and paste them into the project folder, which is now up two folder levels in `\PhotoEditor\PhotoEditor\PhotoEditor`.

Back in **Solution Explorer**, with the target *Desktop* project node selected, make sure that **Show All Files** is toggled on. Right-click the stub files that you just pasted (`Photo.h` and `.cpp`), and click **Include In Project**. Toggle **Show All Files** off.

You'll see a `static_assert` at the top of the contents of `Photo.h` and `.cpp`, which you'll need to delete.

Confirm that you can build again (but don't run yet).

### Port code into the stubs

Copy the contents of `Photo.h` and `.cpp` from the source project into the target project.

From here, the remaining steps to port the code that you copied are the same as those given in the [Port **Photo** source code](#port-photo-source-code) section.
