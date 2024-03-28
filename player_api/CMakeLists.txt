include(${CMAKE_CURRENT_LIST_DIR}/CMakePrivateSources.txt)
include(${CMAKE_CURRENT_LIST_DIR}/CMakePublicHeaders.txt)

add_library(bnb_player_api STATIC ${BNB_PLAYER_API_PRIVATE_SOURCES} ${BNB_PLAYER_API_PUBLIC_HEADERS})
target_link_libraries(bnb_player_api yuv bnb_effect_player glad)
target_include_directories(bnb_player_api PUBLIC ${CMAKE_CURRENT_LIST_DIR})
