add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/libyuv)

# default CMakeLists.txt in libyuv doesn't provide includes path property,
# so let's set it manually here, because everything required is known already
target_include_directories(yuv PUBLIC ${CMAKE_CURRENT_LIST_DIR}/libyuv/include)
