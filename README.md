# Quick start examples for integrating [Banuba SDK on Desktop](https://docs.banuba.com/face-ar-sdk/core/effect_player/) in C++ apps

## Getting Started

1. Get the latest Banuba SDK archive for MacOS/Windows and the client token. Please fill out our form at [form at banuba.com](https://www.banuba.com/face-filters-sdk) website, or contact us via [info@banuba.com](mailto:info@banuba.com).
2. Copy `bnb_sdk.tar.gz/mac` (for Mac OS) or `bnb_sdk.zip/bin` with `bnb_sdk.zip/include` (for Windows) into `quickstart-desktop-cpp/bnb_sdk` dir.
3. Windows only: copy **contents** of `bnb_sdk.zip/resources` into `quickstart-desktop-cpp/resources`.
4. Copy and Paste your client token into the appropriate section of [`OEP-desktop/main.cpp`][main.cpp#L15]
5. Copy `effects` from release archive into `OEP-desktop/resources`. 
Configure effect in the appropriate section of [`OEP-desktop/main.cpp`](main.cpp#L69), e.g. `effects/test_BG`
    The `effects` folder should be located in the `OEP-desktop/resources` folder.
    More effects can be found [here](https://docs.banuba.com/face-ar-sdk-v1/overview/demo_face_filters)
6. Generate project files by executing the following commands:

    ##### Windows x86 build:

    ```bat
        cd %path_to_repository%
        git submodule update --init --recursive
        md build
        cd build
        cmake -A Win32 ..
    ```

    ##### Windows x64 build:

    ```bat
        cd %path_to_repository%
        git submodule update --init --recursive
        md build
        cd build
        cmake -A x64 ..
    ```

    ##### macOS build:

    ```sh
        cd %path_to_repository%
        git submodule update --init --recursive
        mkdir build
        cd build
        cmake -G Xcode ..
    ```

7. The previous step will generate a Xcode project for macOS and a Visual Studio project for Windows. Open the viewer_standalone project in an appropriate IDE on your platform.
8. Select target `example` in Xcode or set the `example` project as Startup in Visual Studio.
9. Run build.


## Sample structure

- **oep** - is a submodule of the offscreen effect player
- **libraries**
  - **glad** -  OpenGL loader
  - **renderer** - used only to demonstrate how to work with offscreen_effect_player. Draws received frames to the specified GLFW window
  - **utils** - wrapper for GLFW
- **main.cpp** - contains the main function implementation, demonstrating basic pipeline for frame processing to apply effect offscreen
- **effect_player.cpp, effect_player.hpp** - contains the custom implementation of the effect_player interface with using cpp api
- **render_context.cpp, render_context.hpp** - contains the custom implementation of the render_context interface with using GLFW
- **camera_utils.cpp, camera_utils.hpp** - contains a method that helps convert bnb::full_image_t type to OEP pixel_buffer type

## How to change an effect

1. Open `OEP-desktop/main.cpp`
2. On line 45 find:

   ```c++
    oep->load_effect("effects/Afro");
   ```

3. Write the effect name that you want to run. For example: ("effects/your_effect_name")

*Note:* The effect must be in `OEP-desktop/resources/effect`.

## Integration note

For the integration of the Offscreen Effect player into your application, it is necessary to copy the OEP folder and implement interfaces for effect_player and render_context, but if your application is based on the GLFW library and using bnb_effect_player CPP API, you can just reuse the current implementation.
