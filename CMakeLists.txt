cmake_minimum_required(VERSION 3.23)

project(QL6502 LANGUAGES C)

add_executable(ql6502)

target_include_directories(ql6502 PRIVATE "${PROJECT_SOURCE_DIR}/src" )

target_sources(ql6502 PRIVATE   
                "src/main.c" 
                "src/Build.c" 
                "src/XD6502.c" 
                "src/Pack.c")

# target_link_libraries(ql6502 PRIVATE "${USB_LIBRARY_PATH}/libusb-1.0.a")

add_custom_command(TARGET ql6502 POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ql6502.exe "${PROJECT_SOURCE_DIR}/example")