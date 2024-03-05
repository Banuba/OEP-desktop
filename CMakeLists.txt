cmake_minimum_required(VERSION 3.9)

project(playerapi-desktop-cpp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(BNB_RESOURCES_FOLDER ${CMAKE_CURRENT_LIST_DIR}/resources)
set(BNB_THIRD_FOLDER ${CMAKE_CURRENT_LIST_DIR}/third)

if(WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /D_ENABLE_EXTENDED_ALIGNED_STORAGE")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /D_ENABLE_EXTENDED_ALIGNED_STORAGE")

    # Set to OFF to disable ffmpeg dependency (SDK should be built with disabled video_player also)
    set(BNB_VIDEO_PLAYER ON)

    add_definitions(
        -DBNB_RESOURCES_FOLDER="${BNB_RESOURCES_FOLDER}"
        -DBNB_VIDEO_PLAYER=$<BOOL:${BNB_VIDEO_PLAYER}>
    )
endif()

#
# Main
#

include(${CMAKE_CURRENT_LIST_DIR}/copy_libs.cmake)

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/bnb_sdk)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/third/glfw)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/third/glad)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/player_api)

option(DEPLOY_BUILD "Build for deployment" OFF)

###########
# Targets #
###########

set(APP_SOURCES
    glfw_window.hpp
    glfw_window.cpp
    main.cpp
    opengl_context.hpp
)

if (APPLE)
    # some magic to add these files to copy phase
    set(SDK_FRAMEWORK_PATH "${CMAKE_CURRENT_LIST_DIR}/bnb_sdk/mac")
    set(FullEPFrameworkPath "${SDK_FRAMEWORK_PATH}/BanubaEffectPlayer.framework")
    add_custom_command(OUTPUT "${FullEPFrameworkPath}" COMMAND "")

    set(EXAMPLE_RESOURCES ${BNB_RESOURCES_FOLDER}/effects)

    add_executable(example ${APP_SOURCES} ${FullEPFrameworkPath} ${EXAMPLE_RESOURCES})

    foreach(assetItem ${EXAMPLE_RESOURCES})
      set_source_files_properties(${assetItem} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)   
    endforeach()
    set_source_files_properties(${FullEPFrameworkPath} PROPERTIES MACOSX_PACKAGE_LOCATION Frameworks)
else (APPLE)
    add_executable(example ${APP_SOURCES})
endif (APPLE)

target_link_libraries(example
    glfw
    bnb_player_api
)

if (APPLE)
    set(CMAKE_OSX_DEPLOYMENT_TARGET "10.13")

    set(EXAMPLE_BUNDLE_NAME "Banuba Player API Example") # This name can contain up to 15 characters according to Apple's documentation
    set(EXAMPLE_COPYRIGHT "© 2024 Banuba. All rights reserved.")
    set(EXAMPLE_INFO "Banuba Player API Example.")

    set_target_properties(example PROPERTIES
        OUTPUT_NAME ${EXAMPLE_BUNDLE_NAME}
        RESOURCE "${EXAMPLE_RESOURCES}"
        MACOSX_BUNDLE TRUE

        # The same as stadrard MacOSXBundleInfo.plist.in from CMake but with camera permissions added
        MACOSX_BUNDLE_INFO_PLIST "${BNB_RESOURCES_FOLDER}/mac/Info.plist.in"

        MACOSX_BUNDLE_BUNDLE_NAME           ${EXAMPLE_BUNDLE_NAME}            # Sets CFBundleName
        MACOSX_BUNDLE_COPYRIGHT             ${EXAMPLE_COPYRIGHT}              # Sets NSHumanReadableCopyright
        MACOSX_BUNDLE_GUI_IDENTIFIER        "com.banuba.sdk.playerapi.sample" # Sets CFBundleIdentifier
        MACOSX_BUNDLE_INFO_STRING           ${EXAMPLE_INFO}                   # Sets CFBundleGetInfoString

        # Explicitly skip code signing (CMake tries to turn it on for application bundles)
        XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY ""
    )
endif()

copy_sdk(example)
copy_third(example)
