Quick start examples for integrating [Banuba SDK on Desktop](https://docs.banuba.com/face-ar-sdk/core/effect_player/) in C++ apps.

# Getting Started

1. Get the latest Banuba SDK archive for MacOS/Windows and the client token. Please fill out our form at [form at banuba.com](https://www.banuba.com/face-filters-sdk) website, or contact us via [info@banuba.com](mailto:info@banuba.com).
2. Copy `bnb_viewer_standalone/bnb_sdk/` into the `quickstart-desktop-cpp/bnb_sdk` dir:
    `bnb_viewer_standalone/bnb_sdk/` => `quickstart-desktop-cpp/bnb_sdk`
3. Copy `bnb_viewer_standalone/resources/` files into the `quickstart-desktop-cpp/resources` dir:
    `bnb_viewer_standalone/resources/` => `quickstart-desktop-cpp/resources`
4. Copy `bnb_viewer_standalone/third` files into the `quickstart-desktop-cpp/third` dir:
    `bnb_viewer_standalone/third/` => `quickstart-desktop-cpp/third`
5. Copy and Paste your client token into the appropriate section of `quickstart-desktop-cpp/main.cpp`
6. Load submodules and generate project files by executing the following commands:
    ##### Windows x86 build:	
    ```bat
        cd %path_to_repository%
        git submodule init
        git submodule update
        mkdir build
        cd build
        cmake -A Win32 ..
    ```
    ##### Windows x64 build:	
    ```bat
        cd %path_to_repository%
        git submodule init
        git submodule update
        mkdir build
        cd build
        cmake -A x64 ..
    ```
    ##### MacOS build:	
    ```sh
        cd %path_to_repository%
        git submodule init
        git submodule update
        mkdir build
        cd build
        cmake -G Xcode ..
    ```
7. The previous step will generate a Xcode project for MacOS and a Visual Studio project for Windows. Open the viewer_standalone project in an appropriate IDE on your platform.
8. Select target `example`.
9. Run build.

# Contributing

Contributions are what make the open source community such an amazing place to learn, inspire, and create. Any contributions you make are **greatly appreciated**.

1. Fork the Project
2. Create your Feature Branch (`git checkout -b feature/AmazingFeature`)
3. Commit your Changes (`git commit -m 'Add some AmazingFeature`)
4. Push to the Branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

# Sample structure

- **oep** - submodule with offscreen effect player
- **main.cpp** - contains the main function implementation, demonstrating basic pipeline for frame processing to apply effect offscreen

# Note

For integration of the Offscreen Effect player to your application it is necessary to copy the offscreen_effect_player folder and implement interfaces for offscreen_render_target, but if your application is based on the GLFW library, you can just reuse offscreen_render_target with the utility library.
