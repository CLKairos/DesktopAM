cmake_minimum_required(VERSION 3.16)
project(DesktopAM LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# SFML
set(SFML_DIR ${CMAKE_SOURCE_DIR}/lib/SFML/lib/cmake/SFML)
find_package(SFML 3 REQUIRED COMPONENTS Graphics Window System)

# Build PortAudio manually
set(PA_DIR ${CMAKE_SOURCE_DIR}/lib/portaudio)

add_library(portaudio_static STATIC
        ${PA_DIR}/src/common/pa_allocation.c
        ${PA_DIR}/src/common/pa_converters.c
        ${PA_DIR}/src/common/pa_cpuload.c
        ${PA_DIR}/src/common/pa_debugprint.c
        ${PA_DIR}/src/common/pa_dither.c
        ${PA_DIR}/src/common/pa_front.c
        ${PA_DIR}/src/common/pa_process.c
        ${PA_DIR}/src/common/pa_ringbuffer.c
        ${PA_DIR}/src/common/pa_stream.c
        ${PA_DIR}/src/common/pa_trace.c
        ${PA_DIR}/src/os/unix/pa_unix_hostapis.c
        ${PA_DIR}/src/os/unix/pa_unix_util.c
        ${PA_DIR}/src/hostapi/alsa/pa_linux_alsa.c
)

target_include_directories(portaudio_static PUBLIC
        ${PA_DIR}/include
        ${PA_DIR}/src/common
        ${PA_DIR}/src/os/unix
        /usr/include/alsa
)

target_compile_definitions(portaudio_static PRIVATE
        PA_USE_ALSA
        PA_LITTLE_ENDIAN
)

target_link_libraries(portaudio_static PUBLIC
        asound
        m
        pthread
)

# DesktopAM executable
add_executable(DesktopAM
        main.cpp
        audiomixer.cpp
        uimanager.cpp
)

target_include_directories(DesktopAM PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/lib/SFML/include
        ${PA_DIR}/include
)

target_link_libraries(DesktopAM PRIVATE
        SFML::Graphics
        SFML::Window
        SFML::System
        portaudio_static
)

add_custom_command(TARGET DesktopAM POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/res
        $<TARGET_FILE_DIR:DesktopAM>/res
)