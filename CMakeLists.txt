cmake_minimum_required(VERSION 3.20...4.0) # FetchContent is available in 3.11+

#set(CMAKE_C_STANDARD 17)
#set(CMAKE_CXX_STANDARD_REQUIRED True)

# https://www.programmersought.com/article/68945253024/

project(dx11Base VERSION 0.0.1)

set(WINDOWKIT_VERSION 10) # folder folder version
set(WINDOWKIT_VERSION_UPDATE 10.0.19041.0) # sub folder folder version
set(WINDOW_BIT x64) # arm, arm64, x64, x86
set(DIRECTX_VERSION 11) # ex. 9, 10, 11, 12

# statically link MSVC runtime in thrift to being fully self-contained.
# this increases .exe size but guarantees working on all windows versions
# see https://cmake.org/cmake/help/latest/variable/CMAKE_MSVC_RUNTIME_LIBRARY.html
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
#set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded")


# This section deal with the character sets UNICODE _UNICODE
# Directx win32 settings
# this deal with convert string
add_definitions(-DUNICODE -D_UNICODE)


# Directx win32 settings
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:CONSOLE")

# control where the static and shared libraries are built so that on windows
# we don't need to tinker with the path to run the executable
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")

set(DIRECTX_LIBS "") # ex. d3d11, d3dx11d, dxguid

option (BUILD_SHARED_LIBRARY OFF)
set(BUILD_SHARED_LIBS OFF)


if (WIN32)
  set(CMAKE_USE_SCHANNEL ON)
endif()

include(FetchContent) 


# Fetches the latest version of FMT
FetchContent_Declare(
    fmt
    URL	https://github.com/fmtlib/fmt/archive/refs/heads/master.zip
)
FetchContent_MakeAvailable(fmt)


if (NOT imgui_FOUND) # If there's none, fetch and build imgui
	include(FetchContent)
	FetchContent_Declare(
	  imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
	  GIT_TAG v1.92.1-docking
  )
    
	FetchContent_GetProperties(cimgui)
	
    if (NOT imgui_POPULATED) # Have we downloaded yet?
      set(FETCHCONTENT_QUIET NO)
      FetchContent_MakeAvailable(imgui)
      # utility to setup the downloaded library for use
      #FetchContent_MakeAvailable(imgui)

	  #INCLUDE
      list(APPEND CPP_INCLUDE_DIRS ${imgui_SOURCE_DIR})
      list(APPEND CPP_INCLUDE_DIRS ${imgui_SOURCE_DIR}/backends)
	  #SOURCE
      list(APPEND CPP_SOURCE_FILES ${imgui_SOURCE_DIR}/imgui.cpp)
      list(APPEND CPP_SOURCE_FILES ${imgui_SOURCE_DIR}/imgui_demo.cpp)
      list(APPEND CPP_SOURCE_FILES ${imgui_SOURCE_DIR}/imgui_draw.cpp)
      list(APPEND CPP_SOURCE_FILES ${imgui_SOURCE_DIR}/imgui_tables.cpp)
      list(APPEND CPP_SOURCE_FILES ${imgui_SOURCE_DIR}/imgui_widgets.cpp)
	  #HEADER
      list(APPEND CPP_HEADER_FILES ${imgui_SOURCE_DIR}/imgui.h)
      list(APPEND CPP_HEADER_FILES ${imgui_SOURCE_DIR}/imconfig.h)
      list(APPEND CPP_HEADER_FILES ${imgui_SOURCE_DIR}/imgui_internal.h)
	  #DIRECTX
	    list(APPEND CPP_SOURCE_FILES ${imgui_SOURCE_DIR}/backends/imgui_impl_win32.cpp)
	    list(APPEND CPP_HEADER_FILES ${imgui_SOURCE_DIR}/backends/imgui_impl_win32.h)
      list(APPEND CPP_SOURCE_FILES ${imgui_SOURCE_DIR}/backends/imgui_impl_dx11.cpp)
      list(APPEND CPP_HEADER_FILES ${imgui_SOURCE_DIR}/backends/imgui_impl_dx11.h)

    endif()
  endif()


	
	

  set_property(GLOBAL PROPERTY USE_FOLDERS ON)  #this is on by default



include_directories(${CPP_INCLUDE_DIRS}) #include file folder

list(APPEND DIRECTX_LIBS d3d11)


set(
    source_list

    hasMain/hasMainImGuiDX11.cpp
	hasMain/BGMainSupport.cpp
	hasMain/BGMainSupport.h
	hasMain/resource.rc
	hasMain/resource.h

    common/BGUtil.cpp
    common/BGUtil.h
    common/AppIni.h
	common/AppIni.cpp
	
    common/Fonts/font_awesome_5.h
	common/Fonts/FontAwesome5.cpp
	common/Fonts/FontAwesome5Brands.cpp
	common/Fonts/Fonts.h
	common/Fonts/IconsFontAwesome5.h
	common/Fonts/IconsFontAwesome5Brands.h
	common/Fonts/RobotoBold.cpp
	common/Fonts/RobotoItalic.cpp
	common/Fonts/RobotoMonoBold.cpp
	common/Fonts/RobotoMonoItalic.cpp
	common/Fonts/RobotoMonoRegular.cpp
	common/Fonts/RobotoRegular.cpp

	SQLSupport/sqlite3.h
	SQLSupport/sqlite3.c
	SQLSupport/SQL_Common.cpp
	SQLSupport/SQL_Common.h


	App/ImGuiAPP.cpp
	App/ImGuiAPP.h
	App/TemplateApp.cpp
	App/TemplateApp.h
	App/About.cpp 
	App/help.cpp 
	App/help.h
	App/ImBGUtil.cpp
	App/ImBGUtil.h
	App/AppOptions.cpp

	#SQLSupport/SQL_RouteList.h
	#SQLSupport/SQL_RouteList.cpp
    #SQLSupport/SQL_POIList.cpp
	#SQLSupport/SQL_ACList.cpp






)

#this organizes the VS project folder like the source tree folder
foreach(source IN LISTS source_list)
    get_filename_component(source_path "${source}" PATH)
    string(REPLACE "/" "\\" source_path_msvc "${source_path}")
    source_group("${source_path_msvc}" FILES "${source}")
endforeach()


# headers and paths
#set(CPP_INCLUDE_DIRS "")
#set(CPP_SOURCE_FILES "")
#set(CPP_HEADER_FILES "")

#================================================
# EXECUTE APP
#================================================
add_executable(${PROJECT_NAME} 
  ${CPP_HEADER_FILES}
  ${CPP_SOURCE_FILES}
  ${source_list}
)

set_property(TARGET ${PROJECT_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/RunFolder")

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME} )
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)


#target_compile_definitions(${PROJECT_NAME} PRIVATE "UNICODE" "_UNICODE")
# https://stackoverflow.com/questions/33137590/link-against-dx10-using-cmake

target_include_directories(${PROJECT_NAME}  PUBLIC App)
target_include_directories(${PROJECT_NAME}  PUBLIC common)
target_include_directories(${PROJECT_NAME}  PUBLIC hasMain)
target_include_directories(${PROJECT_NAME}  PUBLIC src)
target_include_directories(${PROJECT_NAME}  PUBLIC SQLSupport)


target_link_libraries(${PROJECT_NAME}   ${DIRECTX_LIBS} )
target_link_libraries(${PROJECT_NAME}   fmt)

