cmake_minimum_required (VERSION 3.5)
project (mems-rosco)
set (PROJECTNAME "mems-rosco")





#SET ( Qt5LinguistTools_DIR "C:/Qt/qt5.8/mingw53_32/lib/cmake/Qt5LinguistTools" CACHE PATH "Qt5LinguistTools_DIR" )
#find_package(Qt5 COMPONENTS Widgets LinguistTools)

set(CXXFLAGS += -std=c++11)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# auto everything
# Reference: https://cmake.org/cmake/help/latest/manual/cmake-qt.7.html
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
SET(TRANSLATIONS :/res/translation_german.qm)
set (LANGUAGES

transfile_en
transfile_esp
transfile_fr
transfile_ge
)
# Only call lrelease on each translation files to create qm files.
# This command assumes that the ts files already exists (manually created).
#
# qt5_add_translation(QM_FILES Transfile_en.ts Transfile_ge.ts)

# Call lupdate to generate (or update already existing) ts files from source
# code (.cpp and .ui) code AND generate qm files. We can call it with a source
# directory OR individual files to search for strings to translate. The ts files
# are generated in the source directory and the qm files are created in the
# build directory, as it should, because we don't want to pollute our source
# directory with generated binary files.
#
# Recall that the ts files are edited by translators, while qm files are just
# binary representation of the ts file for perfomance and size optimization.
#
# Using a source file list:
# qt5_create_translation(QM_FILES
#     translatemainwindow.cpp translatemainwindow.ui
#     english.ts french.ts
# )
# 
# Using a source directory:
#qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} Transfile_en.ts Transfile_ge.ts)

# The qm files are generated in the build tree, but the qrc file is inside the
# source directory and the path to resources are relative to the location of
# the qrc file itself. We use configure_file() to copy the qrc file in the build
# directory such that it can find the qm translations files. The qrc file is
# copied if it doesn't exist in the destination or if it is modified.
#configure_file(translations.qrc ${CMAKE_BINARY_DIR} COPYONLY)

# We have to reference the translations.qrc copy in the build directory, not the
# original file in the source directory. We also add qm files to the target to
# create a dependency that will force rebuild in case the translation are
# updated.
ADD_CUSTOM_COMMAND (
OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/project.ts
COMMAND lupdate -silent -I ./includes ./sources -ts ./project.ts ${CMAKE_CURRENT_BINARY_DIR}/project.ts
WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

if(APPLE AND UNIX) ## OSX
	install(FILES ${TRANSLATION_QM} DESTINATION MultiMC.app/Contents/Resources/translations)
else()
	install(FILES ${TRANSLATION_QM} DESTINATION translations)
endif()
ADD_CUSTOM_COMMAND (
OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/project.ts
COMMAND lupdate -silent -I ./includes ./sources -ts ./project.ts ${CMAKE_CURRENT_BINARY_DIR}/project.ts
WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)


string (TOLOWER ${PROJECTNAME} PNAME)

add_definitions (-DPROJECTNAME=\"${PROJECTNAME}\")

set (VER_MAJOR 2)
set (VER_MINOR 2)
set (VER_PATCH 9)

set (CMAKE_INCLUDE_CURRENT_DIR ON)
set (CMAKE_AUTOMOC ON)
set (CMAKE_CXX_FLAGS "-std=c++11")
set (CPACK_SOURCE_GENERATOR "TGZ")
set (CPACK_SOURCE_PACKAGE_FILE_NAME "${PROJECT_NAME}-${VER_MAJOR}.${VER_MINOR}.${VER_PATCH}-src")
set (CPACK_SOURCE_IGNORE_FILES
      ".*~$"
      ".git"
      ".gitignore"
      ".*swp$"
      ".*zip$"
      ".*gz$"
      ".*deb$"
      "Makefile.*"
      "moc_"
      "qrc_"
      "^CPack.*"
      "_CPack_Packages"
      "CPackConfig.cmake"
      "CPackSourceConfig.cmake"
      "CMakeFiles"
      "CMakeCache.txt"
      "CMakeLists.txt.user*"
      "cmake_install.cmake"
      "install_manifest.txt"
      "README.TXT"
      "LICENSE.TXT"
      "build/"
      "build-win32/"
      "settings.ini"
      "debug"
      "release"
      "object_script.*")

configure_file (
  "help.html.in"
  "${CMAKE_SOURCE_DIR}/help.html"
  )
#set (Qt5PrintSupport_DIR "c:/QT/qt5.8/mingw53_32/lib/cmake/Qt6PrintSupport")
#find_package(Qt5PrintSupport REQUIRED)
set (cmake_prefix_path "c:/qt/qt5.7/5.7/mingw53_32/cmake/qt5widgets")
find_package (Qt5Widgets)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")

include_directories ("${CMAKE_SOURCE_DIR}/qledindicator"
                     "${CMAKE_SOURCE_DIR}/analogwidgets"
                     "${CMAKE_CURRENT_BINARY_DIR}"
                     "${Qt5Widgets_INCLUDE_DIRS}")

qt5_wrap_ui (UI_SOURCE mainwindow.ui)
qt5_add_resources (RG_RESOURCE resources.qrc)

add_definitions ("-DVER_MAJOR=${VER_MAJOR}"
                 "-DVER_MINOR=${VER_MINOR}"
                 "-DVER_PATCH=${VER_PATCH}")

add_executable (${PNAME} main.cpp
                         memsinterface.cpp
                         helpviewer.cpp
                         logger.cpp
                         serialdevenumerator.cpp
                         mainwindow.cpp
                         aboutbox.cpp
                         optionsdialog.cpp
                         qledindicator/qledindicator.cpp
                         analogwidgets/led.cpp
                         analogwidgets/functions.cpp
                         analogwidgets/widgetwithbackground.cpp
                         analogwidgets/manometer.cpp
                         analogwidgets/abstractmeter.cpp
                         qcustomplot.cpp
                         ${UI_SOURCE}
                         ${RG_RESOURCE})

if (MINGW)
  message (STATUS "Found Windows/MinGW platform.")

  # prevent the final executable from launching a terminal window in parallel with the main GUI
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mwindows")

  # -fPIC appears to be redundant when building win32 binaries, so disable that flag
  string (REGEX REPLACE "-fPIC" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})

  set (LIBROSCO_PATH "$ENV{SystemDrive}/qt-examples/mems-rosco2.2.9")
  include_directories ("${LIBROSCO_PATH}/include")

  message (STATUS "Defaulting installation path to: $ENV{SystemDrive}/${PNAME}")
  set (CMAKE_INSTALL_PREFIX "$ENV{SystemDrive}/${PNAME}")
  set (EXE_FILE "${CMAKE_CURRENT_BINARY_DIR}/${PNAME}.exe")

  # When we call find_library() below, we're only looking for DLLs, so set the suffix list appropriately
  set (CMAKE_FIND_LIBRARY_SUFFIXES ".dll")
  message (STATUS "Limiting find_library() suffixes to ${CMAKE_FIND_LIBRARY_SUFFIXES}")

  find_library (LIBGCC NAMES libgcc_s_sjlj-1 libgcc_s_seh-1 libgcc_s_dw2-1)
  if (LIBGCC)
    message (STATUS "Found libgcc at ${LIBGCC}")
  else ()
    message (SEND_ERROR "Could not find libgcc!")
  endif ()

  find_library (LIBSTDCPP libstdc++-6)
  if (LIBSTDCPP)
    message (STATUS "Found libstdc++ at ${LIBSTDCPP}")
  else ()
    message (SEND_ERROR "Could not find libstdc++!")
  endif ()

  find_library (ZLIB zlib1)
  if (ZLIB)
    message (STATUS "Found zlib1 at ${ZLIB}")
  else ()
    message (SEND_ERROR "Could not find zlib1!")
  endif ()

  find_library (QTCORE_DLL Qt5Core)
  if (QTCORE_DLL)
    message (STATUS "Found QtCore at ${QTCORE_DLL}")
  else ()
    message (SEND_ERROR "Could not find Qt5Core.dll! Check that the Qt5 bin/ directory (or the qtbase/bin/ directory) is in your PATH.")
  endif ()

  find_library (QTGUI_DLL Qt5Gui)
  if (QTGUI_DLL)
    message (STATUS "Found QtGui at ${QTGUI_DLL}")
  else ()
    message (SEND_ERROR "Could not find Qt5Gui.dll! Check that the Qt5 bin/ directory (or the qtbase/bin/ directory) is in your PATH.")
  endif ()

  find_library (QTWIDGETS_DLL Qt5Widgets)
  if (QTGUI_DLL)
    message (STATUS "Found QtWidgets at ${QTWIDGETS_DLL}")
  else ()
    message (SEND_ERROR "Could not find Qt5Widgets.dll! Check that the Qt5 bin/ directory (or the qtbase/bin/ directory) is in your PATH.")
  endif ()

  find_library (QWINDOWS_DLL qwindows)
  if (QWINDOWS_DLL)
    message (STATUS "Found Qt qwindows platform plugin at ${QWINDOWS_DLL}")
  else ()
    message (SEND_ERROR "Could not find Qt qwindows platform plugin!")
  endif ()

  find_library(LIBROSCO_DLL librosco PATHS ${LIBROSCO_PATH} ${CMAKE_INSTALL_PREFIX})
  if (LIBROSCO_DLL)
    message (STATUS "Found librosco at ${LIBROSCO_DLL}")
  else ()
    message (SEND_ERROR "Could not find librosco.dll! Check that it exists in one of the directories in your PATH.")
  endif ()

  target_link_libraries (${PNAME} ${LIBROSCO_DLL} Qt5::Widgets)

  # convert Unix-style newline characters into Windows-style
  configure_file ("${CMAKE_SOURCE_DIR}/README" "${CMAKE_BINARY_DIR}/README.TXT" NEWLINE_STYLE WIN32)
  configure_file ("${CMAKE_SOURCE_DIR}/LICENSE" "${CMAKE_BINARY_DIR}/LICENSE.TXT" NEWLINE_STYLE WIN32)

  install (FILES "${CMAKE_BINARY_DIR}/README.TXT" "${CMAKE_BINARY_DIR}/LICENSE.TXT" DESTINATION "doc")
  install (FILES ${EXE_FILE}
                 ${LIBGCC}
                 ${LIBSTDCPP}
                 ${QTCORE_DLL}
                 ${QTGUI_DLL}
                 ${QTWIDGETS_DLL}
                 ${LIBROSCO_DLL}
                 ${ZLIB}
           DESTINATION ".")
  install (FILES ${QWINDOWS_DLL} DESTINATION "platforms")

  set (CPACK_GENERATOR "ZIP")
  set (CPACK_PACKAGE_NAME ${PROJECT_NAME})
  set (CPACK_INCLUDE_TOPLEVEL_DIRECTORY 0)
  set (CPACK_PACKAGE_FILE_NAME "${PROJECT_NAME}-${VER_MAJOR}.${VER_MINOR}.${VER_PATCH}-${CMAKE_SYSTEM_NAME}")
  include (CPack)

else()
  message (STATUS "Defaulting to Linux build environment.")

  target_link_libraries (${PNAME} rosco Qt5::Widgets)

  set (CMAKE_SKIP_RPATH TRUE)
  set (CMAKE_INSTALL_PREFIX "/usr/local")
  set (Qt5Core_DIR "/usr/lib/x86_64-linux-gnu/cmake/Qt5Core")
  set (Qt5Gui_DIR "/usr/lib/x86_64-linux-gnu/cmake/Qt5Gui")
  set (Qt5LinguistTools_DIR "/opt/Qt/5.12.2/gcc_64/lib/cmake(Qt5LinguistTools")
  set (Qt5PrintSupport_DIR "/usr/lib/x86_64-linux-gnu/cmake/Qt5PrintSupport")
  set (Qt5Widgets "/usr/lib/x86_64-linux-gnu/cmake/Qt5Widgets")
  set (EXE_FILE "${CMAKE_CURRENT_BINARY_DIR}/${PNAME}")

  # set the installation destinations for the header files,
  # shared library binaries, and reference utility
  install (FILES ${EXE_FILE} DESTINATION "bin"
           PERMISSIONS
            OWNER_READ OWNER_EXECUTE OWNER_WRITE
            GROUP_READ GROUP_EXECUTE
            WORLD_READ WORLD_EXECUTE)

  install (DIRECTORY DESTINATION "share/doc/${PNAME}" DIRECTORY_PERMISSIONS
            OWNER_READ OWNER_EXECUTE OWNER_WRITE
            GROUP_READ GROUP_EXECUTE
            WORLD_READ WORLD_EXECUTE)

  install (FILES "${CMAKE_SOURCE_DIR}/README"
                 "${CMAKE_SOURCE_DIR}/LICENSE"
           DESTINATION "share/doc/${PNAME}"
           PERMISSIONS
            OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ)

  # set the architecture flag that will be used by the Debian package builder
 # if (${CMAKE_SYSTEM_PROCESSOR} STREQUAL "x86_64")
 #   set (CPACK_DEBIAN_PACKAGE_ARCHITECTURE amd64)
 # elseif (${CMAKE_SYSTEM_PROCESSOR} STREQUAL "i686")
 #   set (CPACK_DEBIAN_PACKAGE_ARCHITECTURE i386)
 # endif (${CMAKE_SYSTEM_PROCESSOR} STREQUAL "x86_64")

  set (CPACK_GENERATOR "DEB")
  set (CPACK_PACKAGE_NAME ${PROJECT_NAME})
  set (CPACK_PACKAGE_VERSION_MAJOR ${VER_MAJOR})
  set (CPACK_PACKAGE_VERSION_MINOR ${VER_MINOR})
  set (CPACK_PACKAGE_VERSION_PATCH ${VER_PATCH})
  set (CPACK_DEBIAN_PACKAGE_MAINTAINER "Colin Bourassa <colin.bourassa@gmail.com>")
  set (CPACK_PACKAGE_DESCRIPTION_SUMMARY "Graphical display for data read from Rover MEMS 1.6 (Modular Engine Management System)")
  set (CPACK_DEBIAN_PACKAGE_SECTION "Science")
  set (CPACK_DEBIAN_PACKAGE_DEPENDS "libc6 (>= 2.13), libstdc++6 (>= 4.6.3), librosco (>= 0.1.0), libqt5core5a (>= 5.0.2), libqt5gui5 (>= 5.0.2), libqt5widgets5 (>= 5.0.2)")
  set (CPACK_PACKAGE_FILE_NAME "${PROJECT_NAME}-${VER_MAJOR}.${VER_MINOR}.${VER_PATCH}-${CMAKE_SYSTEM_NAME}-${CPACK_DEBIAN_PACKAGE_ARCHITECTURE}")
  set (CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE")
  set (CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/README")

  include(CPack)
 # CMAKE_C_COMPILER()
 # CMAKE_CXX_COMPILER()
 # CMAKE_MAKE_PROGRAM()
  endif()

