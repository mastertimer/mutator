CMAKE_MINIMUM_REQUIRED(VERSION 3.7)
project(mutator)

# C++17, самый стандартный из возможных (без дополнительных фишек компиляторов)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Юникодовая кодировка, которую нужно включить для Windows
set(EXECT )
if(WIN32)
    add_compile_definitions(UNICODE _UNICODE)
    set(EXECT WIN32)
endif()

set(all_source )

macro(source_from_directory dir)
    aux_source_directory(${dir} tmpv)
    foreach(x IN LISTS tmpv)
        list(APPEND all_source ${x})
    endforeach()
    include_directories(${dir})
endmacro()

source_from_directory("library")
source_from_directory("graphics")
source_from_directory("mut")
source_from_directory("mut/basic_go")
source_from_directory("mut/go")
source_from_directory("set")
source_from_directory("vs")

add_executable(${PROJECT_NAME} ${EXECT} vs/main.cpp vs/mutator.rc ${all_source})

include_directories(${CMAKE_SOURCE_DIR})

if(WIN32)
    set(target_system WINDOWS)
elseif(UNIX)
    if(NOT APPLE)
        set(target_system LINUX)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -fext-numeric-literals")
    else()
        set(target_system MACOS)
    endif()
else()
    set(target_system UNKNOWN)
endif()

if(MSVC) # "визуалка, отстань ты от enum'ов"
    add_definitions("/wd26812")
endif()
add_compile_definitions("_SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING")

add_compile_definitions("TARGET_SYSTEM_${target_system}=1")

if(target_system STREQUAL LINUX)
  find_package(X11 REQUIRED)
	include_directories(${PROJECT_NAME} ${X11_INCLUDE_DIR})
	target_link_libraries(${PROJECT_NAME} ${X11_LIBRARIES})
endif()
