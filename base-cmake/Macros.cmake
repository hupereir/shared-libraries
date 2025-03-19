# $Id$
###################### setup cmake policy #########################
macro(setup_cmake_settings)
    set(CMAKE_AUTOMOC TRUE)
    set(CMAKE_AUTORCC TRUE)
    if(COMMAND CMAKE_POLICY)
        cmake_policy(SET CMP0003 NEW)
        # cmake_policy(SET CMP0028 OLD)
        # cmake_policy(SET CMP0043 NEW)
    endif()
endmacro()

###################### setup compiler flags #########################
macro(setup_compiler_flags)

  if(USE_QT6)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
  elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
  elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel" AND NOT WIN32)
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
  endif()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -Wundef")

  if(CMAKE_GENERATOR STREQUAL "Ninja" AND
    ((CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS 4.9) OR
     (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS 3.5)))
    # Force colored warnings in Ninja's output, if the compiler has -fdiagnostics-color support.
    # Rationale in https://github.com/ninja-build/ninja/issues/814
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fdiagnostics-color=always")
  endif()

  add_compile_definitions(QT_DISABLE_DEPRECATED_BEFORE=0x050F00)

endmacro()

###################### add missing libraries for static Qt5 compilation #########################
macro(fix_win32_static_compilation)
  get_filename_component(qt5_install_prefix "${Qt5_DIR}/../../" ABSOLUTE)
  message( "fix_win32_static_compilation - qt5_install_prefix: ${qt5_install_prefix}" )
  set_property(TARGET Qt5::Core APPEND PROPERTY INTERFACE_LINK_LIBRARIES
    -lversion
    -lws2_32
    -lwinmm
    -luxtheme
    -luserenv
    -lnetapi32
    -limm32
    -ldwmapi
    -lwtsapi32
    -liphlpapi
    ${qt5_install_prefix}/libqtpcre2.a
    ${qt5_install_prefix}/libqtlibpng.a
    ${qt5_install_prefix}/libqtharfbuzz.a
    ${qt5_install_prefix}/libqtfreetype.a)
endmacro()

###################### Install Win32 application #########################
macro(add_win32_executable target version)

  ### executable
  add_executable(${target} WIN32 ${ARGN})
  set(BIN_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/bin")

  ### use static gcc linking
  if(${CMAKE_COMPILER_IS_GNUCXX})
    if(USE_SHARED_LIBS)
      set_target_properties(${target} PROPERTIES LINK_FLAGS " -static-libgcc -static-libstdc++ -lpthread -s")
    else()
      # set_target_properties(${target} PROPERTIES LINK_FLAGS " -static -static-libgcc -static-libstdc++ -lpthread -s")
      set_target_properties(${target} PROPERTIES LINK_FLAGS "-static -static-libgcc -static-libstdc++ -Wl,-Bstatic -lstdc++ -lpthread -s")
    endif()
  endif()

  ### get target location
  get_target_property(TARGET_PATH ${target} LOCATION)

  ### Compress target
  if(NOT USE_SHARED_LIBS)

    find_program(UPX upx)
    if(UPX)
      add_custom_command(TARGET ${target} POST_BUILD COMMAND ${UPX} --force ${TARGET_PATH})
    else()
      message("-- Program 'upx' not found")
    endif()

  endif()

  ### copy to version release
  if(RELEASE)

    set(TARGET_RELEASE ${target}-${version})
    add_custom_command(
      TARGET ${target} POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy ${TARGET_PATH} ${CMAKE_CURRENT_BINARY_DIR}/${TARGET_RELEASE}.exe
   )

    install(
      PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/${TARGET_RELEASE}.exe
      DESTINATION ${CMAKE_INSTALL_PREFIX}/release)

  endif()

endmacro()

###################### Install Apple application #########################
macro(add_apple_executable target version)

  ### version
  set(MACOSX_BUNDLE_SHORT_VERSION_STRING ${version})
  set(MACOSX_BUNDLE_VERSION ${version})
  set(MACOSX_BUNDLE_LONG_VERSION_STRING "Version ${version}")

  ### installation directory
  set(BIN_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}")
  add_executable(${target} MACOSX_BUNDLE ${ARGN})

endmacro()

###################### Install unix application #########################
macro(add_unix_executable target)

  ### executable
  add_executable(${target} ${ARGN})
  set(BIN_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/bin")

endmacro()

###################### Install application #########################
macro(add_platform_executable target version)

  if(WIN32)

    add_win32_executable(${target} ${version} ${ARGN})

  elseif(APPLE)

    add_apple_executable(${target} ${version} ${ARGN})

  else()

    add_unix_executable(${target} ${ARGN})

  endif()

endmacro()


###################### add win32 application icons #########################
macro(add_win32_application_icon  sources icon)

    # check icon EXISTS
    set(_icon ${icon}.ico)
    if(EXISTS "${_icon}")

      message("-- Using application icon: ${_icon}")

      # generate resource file and add to sources
      string(REPLACE _SOURCES "" appname ${sources})
      set(_resource ${CMAKE_CURRENT_BINARY_DIR}/${appname}_win32.rc)
      FILE(WRITE ${_resource} "IDI_ICON1        ICON        DISCARDABLE    \"${_icon}\"\n")
      LIST(APPEND ${sources} ${_resource})

    else()

      message("-- Unable to find icon ${_icon}")

    endif()

endmacro()

###################### add apple application icons #########################
macro(add_apple_application_icon  sources icon)

    # check icon EXISTS
    set(_icon ${icon}.icns)
    if(EXISTS "${_icon}")

      get_filename_component(_iconName ${_icon} NAME)
      set(MACOSX_BUNDLE_ICON_FILE  ${_iconName})
      set_source_files_properties(${_icon} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
      list(APPEND ${sources} ${_icon})

    else()

      message("-- Unable to find icon ${_icon}")

    endif()

endmacro()

###################### add unix application icons #########################
macro(add_unix_application_icon  icon name size)

    # check xdg-icon-resource program
    find_program(XDG_ICON_RESOURCE_EXECUTABLE xdg-icon-resource)
    if(NOT XDG_ICON_RESOURCE_EXECUTABLE)

        message("-- Could not find xdg-icon-resource")

    endif()

    set(_icon ${icon}.png)
    if(NOT EXISTS "${_icon}")

      message("-- Unable to find icon ${_icon}")

    endif()

    # add relevant INSTALL command
    if(XDG_ICON_RESOURCE_EXECUTABLE AND EXISTS ${_icon})

      install(
        CODE "message(\"-- Installing: ${_icon}\")"
        CODE "execute_process(COMMAND ${XDG_ICON_RESOURCE_EXECUTABLE} install --novendor --size ${size} ${_icon} ${name})"
     )

    endif()

endmacro()

###################### add application icons #########################
macro(add_application_icon  sources icon)

  if(WIN32)

    add_win32_application_icon(${sources} ${icon} ${ARGN})

  elseif(APPLE)

    add_apple_application_icon(${sources} ${icon} ${ARGN})

  else()

    GET_FILENAME_COMPONENT(_iconName ${icon} NAME)
    ADD_UNIX_APPLICATION_ICON(${icon} ${_iconName} 128 ${ARGN})

  endif()

endmacro()

###################### add desktop file #########################
macro(add_desktop_file desktopFile)

  if(UNIX AND NOT APPLE)

    # check desktop-file-install program
    find_program(XDG_DESKTOP_MENU_EXECUTABLE xdg-desktop-menu)
    if(NOT XDG_DESKTOP_MENU_EXECUTABLE)

        message("-- Could not find xdg-desktop-menu")

    endif()

    # check desktop file existence
    set(_desktopFile ${desktopFile}.desktop)
    if(NOT EXISTS "${_desktopFile}")

      message("-- Unable to find desktop file ${_desktopFile}")

    endif()

    # add relevant INSTALL command
    if(XDG_DESKTOP_MENU_EXECUTABLE AND EXISTS ${_desktopFile})

      install(
        CODE "message(\"-- Installing: ${_desktopFile}\")"
        CODE "execute_process(COMMAND ${XDG_DESKTOP_MENU_EXECUTABLE} install --novendor ${_desktopFile})"
     )

    endif()

  endif()

endmacro()

###################### register a protocol #########################
macro(register_protocol protocol application)

  if(UNIX AND NOT APPLE)

    # check desktop-file-install program
    find_program(XDG_MIME_EXECUTABLE xdg-mime)
    if(NOT XDG_MIME_EXECUTABLE)

        message("-- Could not find xdg-desktop-menu")

    else()

      install(
        CODE "message(\"-- registering application ${application} for ${protocol}:// protocol \")"
        CODE "execute_process(COMMAND ${XDG_MIME_EXECUTABLE} default ${application}.desktop x-scheme-handler/${protocol})"
     )

    endif()

  endif()

endmacro()

###################### register a protocol #########################
include(FeatureSummary)
macro(write_feature_summary)
  feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
endmacro()
