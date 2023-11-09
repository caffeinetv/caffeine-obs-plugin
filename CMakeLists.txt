cmake_minimum_required(VERSION 3.16...3.21)

project(caffeine-obs-plugin VERSION 1.0.6)
add_library(${CMAKE_PROJECT_NAME} MODULE)

set(PLUGIN_AUTHOR "Caffeine")

set(MACOS_BUNDLEID "tv.caffeine.${CMAKE_PROJECT_NAME}")

set(LINUX_MAINTAINER_EMAIL "obs-plugin@caffeine.tv")

set(PROJECT_INCLUDE_DIRS)

set(CMAKE_CXX_STANDARD 17)
option(BUILD_TESTING "Build unit tests." ON)

# Set new UUIDs when you start to create a new plugin.
set(MACOS_PACKAGE_UUID "6D335328-285D-44A1-AB9D-7BA9222130F5")
set(MACOS_INSTALLER_UUID "017640FD-544F-4CA5-858E-600AC2543367")
set(WINDOWS_APP_ID "26FCEE78-C1C3-4072-9E51-659186537B5E")

# Enable default inclusion of targets' source and binary directory
set(CMAKE_INCLUDE_CURRENT_DIR TRUE)
if(XCODE)
  include(cmake/xcode.cmake)
endif()

# include curl
find_package(CURL REQUIRED)
include_directories(${CURL_INCLUDE_DIR})
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE ${CURL_LIBRARIES} CURL::libcurl)

target_sources(
  ${CMAKE_PROJECT_NAME}
  PRIVATE src/plugin-main.cpp
          src/error-logging.hpp
          src/caffeine.hpp
          src/caffeine.cpp
          src/instance.cpp
          src/instance.hpp
          src/rest-api.hpp
          src/rest-api.cpp
          src/serialization.hpp
          src/serialization.cpp
          src/urls.cpp
          src/urls.hpp
          src/utils.hpp
          src/broadcast.cpp
          src/broadcast.hpp
          src/websocket-api.hpp
          src/websocket-api.cpp
          src/policy.hpp
          src/policy.cpp
          src/caffeine-dock-window.hpp
          src/caffeine-dock-window.cpp)

set_target_properties(
  ${CMAKE_PROJECT_NAME}
  PROPERTIES AUTOMOC ON
             AUTOUIC ON
             AUTORCC ON
             AUTOUIC_SEARCH_PATHS "src/forms")
target_sources(${CMAKE_PROJECT_NAME} PRIVATE src/forms/Caffeine.qrc)
target_sources(${CMAKE_PROJECT_NAME} PRIVATE src/forms/CaffeineDockWindow.ui)

list(
  APPEND
  PROJECT_INCLUDE_DIRS
  "${PROJECT_SOURCE_DIR}/src"
  "${PROJECT_SOURCE_DIR}/thirdparty/nlohmann_json/single_include"
  "${PROJECT_SOURCE_DIR}/src/generated"
  "${PROJECT_SOURCE_DIR}/thirdparty/websocketpp-0.8.2/include"
  "${PROJECT_SOURCE_DIR}/thirdparty/asio-1.12.1/include"
  "${CURL_INCLUDE_DIRS}")

# To build stand-alone asio without boost
add_definitions(-DASIO_STANDALONE)

list(APPEND PROJECT_DEFINITIONS "ASIO_STANDALONE")

set(OPENSSL_USE_STATIC_LIBS TRUE)
find_package(OpenSSL REQUIRED)
if(OPENSSL_FOUND)
  # Add the include directories for compiling
  include_directories(${OPENSSL_ROOT_DIR} ${OPENSSL_INCLUDE_DIR} ${OPENSSL_LIBRARIES})
  # Add the static lib for linking
  target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE OpenSSL::SSL OpenSSL::Crypto)
  message("Found OpenSSL ${OPENSSL_VERSION}")
else()
  message("OpenSSL Not Found")
endif()

include_directories("${PROJECT_INCLUDE_DIRS}")

# Tests
if(BUILD_TESTING)
  enable_testing()
  add_subdirectory(tests)
endif()

# Include CoreServices and Security for ssl certificate verification
if(${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
  find_library(SECURITY_LIBRARY Security)
  find_library(CORE_SERVICES_LIBRARY CoreServices)
  target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE ${CORE_SERVICES_LIBRARY} ${SECURITY_LIBRARY})
endif()

# Import libobs as main plugin dependency
find_package(libobs REQUIRED)
include(cmake/ObsPluginHelpers.cmake)

# To use the OBS Frontend API in plugin
find_package(obs-frontend-api REQUIRED)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE OBS::obs-frontend-api)

# To use Qt
find_qt(COMPONENTS Widgets Core)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE Qt::Core Qt::Widgets)
set_target_properties(
  ${CMAKE_PROJECT_NAME}
  PROPERTIES AUTOMOC ON
             AUTOUIC ON
             AUTORCC ON)

configure_file(src/plugin-macros.h.in ${CMAKE_SOURCE_DIR}/src/plugin-macros.generated.h)

target_sources(${CMAKE_PROJECT_NAME} PRIVATE src/plugin-macros.generated.h)

# /!\ TAKE NOTE: No need to edit things past this point /!\

# --- Platform-independent build settings ---

target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/src)

target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE OBS::libobs)

# --- End of section ---

# --- Windows-specific build settings and tasks ---
if(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
  configure_file(cmake/bundle/windows/installer-Windows.iss.in
                 ${CMAKE_BINARY_DIR}/installer-Windows.generated.iss)

  configure_file(cmake/bundle/windows/resource.rc.in ${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}.rc)
  target_sources(${CMAKE_PROJECT_NAME} PRIVATE ${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}.rc)

  if(MSVC)
    target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE /W4)
  endif()
  # --- End of section ---

  # -- macOS specific build settings and tasks --
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
  set(MACOSX_PLUGIN_GUI_IDENTIFIER "${MACOS_BUNDLEID}")
  set(MACOSX_PLUGIN_BUNDLE_VERSION "${CMAKE_PROJECT_VERSION}")
  set(MACOSX_PLUGIN_SHORT_VERSION_STRING "${CMAKE_PROJECT_VERSION}")

  target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE -Wall)
  # --- End of section ---

  # --- Linux-specific build settings and tasks ---
else()
  target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE -Wall)
endif()
# --- End of section ---

setup_plugin_target(${CMAKE_PROJECT_NAME})
