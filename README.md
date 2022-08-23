# Quick start examples for integrating [Banuba SDK on Desktop](https://docs.banuba.com/face-ar-sdk/core/effect_player/) in C++ apps

> **Important**
>
> [master](../../tree/master) branch is always compatible with latest SDK version. Please use [v0.x](../../tree/v0.x) branch for SDK version 0.x (e.g. v0.38).

## Getting Started

1. Get the latest Banuba SDK archive for macOS/Windows and the client token. Please fill out our form at [form at banuba.com](https://www.banuba.com/face-filters-sdk) website, or contact us via [info@banuba.com](mailto:info@banuba.com).
2. Copy `bnb_viewer_standalone/bnb_sdk/` into the `OEP-desktop/bnb_sdk` dir:
    `bnb_viewer_standalone/bnb_sdk/` => `OEP-desktop/bnb_sdk`
3. Copy `bnb_viewer_standalone/resources/` files into the `OEP-desktop/resources` dir:
    `bnb_viewer_standalone/resources/` => `OEP-desktop/resources`
4. Copy `bnb_viewer_standalone/third` files into the `OEP-desktop/third` dir:
    `bnb_viewer_standalone/third/` => `OEP-desktop/third`

    NOTE: The macOS sample only depends on `asyncplusplus` and `glfw` third-party libraries.

    NOTE: The Windows sample depends on the following third-party libraries:
      - `glfw` - provides OGL context and application window
      - `asyncplusplus` - used in the class implementing window (can be discarded)
      - `openal` - sound library used by the BanubaSDK
      - `ffmpeg` - used by video player to play video as a [background replacement](https://docs.banuba.com/face-ar-sdk-v1/overview/technical_specification#video-formats-support). 
5. Copy and Paste your client token into the appropriate section of `OEP-desktop/main.cpp`
6. Configure effect in the appropriate section of `OEP-desktop/main.cpp`, e.g. `effects/test_BG`
    The `effects` folder should be located in the `OEP-desktop/resources` folder.
    More effects can be found [here](https://docs.banuba.com/face-ar-sdk-v1/overview/demo_face_filters)
7. Generate project files by executing the following commands:

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

8. The previous step will generate a Xcode project for macOS and a Visual Studio project for Windows. Open the viewer_standalone project in an appropriate IDE on your platform.
9. Select target `example` in Xcode or set the `example` project as Startup in Visual Studio.
10. Run build.

## Contributing

Contributions are what make the open source community such an amazing place to learn, inspire, and create. Any contributions you make are **greatly appreciated**.

1. Fork the Project
2. Create your Feature Branch (`git checkout -b feature/AmazingFeature`)
3. Commit your Changes (`git commit -m 'Add some AmazingFeature`)
4. Push to the Branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

## Sample structure

- **oep** - is a submodule of the offscreen effect player
- **libraries**
  - **renderer** - used only to demonstrate how to work with offscreen_effect_player. Draws received frames to the specified GLFW window
  - **utils**
    - **glfw_utils** - contains helper classes to work with GLFW
    - **utils** - contains common helper classes such as thread_pool
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
