add_library(magaxat)

target_include_directories(magaxat
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/../lib>
        $<BUILD_INTERFACE:${MAGAXAT_METADATA_INCLUDE_DIR}>
        $<INSTALL_INTERFACE:include>
)

target_compile_features(magaxat PUBLIC cxx_std_17)

if(WIN32)
    add_subdirectory(windows)
else()
    add_subdirectory(linux)
endif()

add_executable(magaxat-host
    main.cpp
)

target_link_libraries(magaxat-host PRIVATE magaxat)
