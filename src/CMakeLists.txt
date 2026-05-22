cmake_minimum_required(VERSION 3.5...3.31)
project(BambuStudio-native)

add_subdirectory(build-utils)
add_subdirectory(admesh)
# add_subdirectory(avrdude)
# boost/nowide
add_subdirectory(boost)
add_subdirectory(clipper)
add_subdirectory(clipper2)
add_subdirectory(miniz)
add_subdirectory(minilzo)
add_subdirectory(glu-libtess)
add_subdirectory(qhull)
add_subdirectory(Shiny)
add_subdirectory(semver)
add_subdirectory(libigl)
add_subdirectory(hints)
add_subdirectory(mcut)

# Adding libnest2d project for bin packing...
add_subdirectory(libnest2d)

# libnoise for structured fuzzy skin (Perlin, Billow, etc.); use dep via CMAKE_PREFIX_PATH
find_package(libnoise REQUIRED)

add_subdirectory(libslic3r)

if (SLIC3R_GUI)
    add_subdirectory(imgui)
    add_subdirectory(imguizmo)
    add_subdirectory(hidapi)
    include_directories(hidapi/include)

    # Qt6 replaces wxWidgets
    find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets OpenGL OpenGLWidgets Svg)
    find_package(Qt6 OPTIONAL_COMPONENTS WebEngineWidgets Multimedia MultimediaWidgets)
    set(CMAKE_AUTOMOC ON)
    set(CMAKE_AUTORCC ON)
    set(CMAKE_AUTOUIC ON)
    message(STATUS "Qt6 version: ${Qt6_VERSION}")
    message(STATUS "Qt6 Core: ${Qt6Core_VERSION}")

    add_subdirectory(slic3r)
endif()


# Create a slic3r executable
# Process manifests for various platforms.
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/platform/msw/BambuStudio.rc.in ${CMAKE_CURRENT_BINARY_DIR}/BambuStudio.rc @ONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/platform/msw/BambuStudio.manifest.in ${CMAKE_CURRENT_BINARY_DIR}/BambuStudio.manifest @ONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/platform/osx/Info.plist.in ${CMAKE_CURRENT_BINARY_DIR}/Info.plist @ONLY)
if (WIN32)
    add_library(BambuStudio SHARED BambuStudio.cpp BambuStudio.hpp)
else ()
    add_executable(BambuStudio BambuStudio.cpp BambuStudio.hpp)
endif ()

if (MINGW)
    target_link_options(BambuStudio PUBLIC "-Wl,-allow-multiple-definition")
    set_target_properties(BambuStudio PROPERTIES PREFIX "")
endif (MINGW)

if (NOT WIN32 AND NOT APPLE)
    # Binary name on unix like systems (Linux, Unix)
    set_target_properties(BambuStudio PROPERTIES OUTPUT_NAME "bambu-studio")
    set(SLIC3R_APP_CMD "bambu-studio")
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/platform/unix/BuildLinuxImage.sh.in ${CMAKE_CURRENT_BINARY_DIR}/BuildLinuxImage.sh @ONLY)
endif ()

target_link_libraries(BambuStudio libslic3r cereal::cereal)
if (APPLE)
#    add_compile_options(-stdlib=libc++)
#    add_definitions(-DBOOST_THREAD_DONT_USE_CHRONO -DBOOST_NO_CXX11_RVALUE_REFERENCES -DBOOST_THREAD_USES_MOVE)
    # -liconv: boost links to libiconv by default
    target_link_libraries(BambuStudio "-liconv -framework IOKit" "-framework CoreFoundation" "-framework AVFoundation" "-framework AVKit" "-framework CoreMedia" "-framework VideoToolbox" -lc++)
elseif (MSVC)
    # Manifest is provided through BambuStudio.rc, don't generate your own.
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /MANIFEST:NO")
else ()
    # Boost on Raspberry-Pi does not link to pthreads explicitly.
    target_link_libraries(BambuStudio ${CMAKE_DL_LIBS} -lstdc++ Threads::Threads pangoft2-1.0)
endif ()

# Add the Slic3r GUI library, libcurl, OpenGL and GLU libraries.
if (SLIC3R_GUI)
#    target_link_libraries(BambuStudio ws2_32 uxtheme setupapi libslic3r_gui ${wxWidgets_LIBRARIES})
target_link_libraries(BambuStudio libslic3r_gui)
    if (MSVC)
        # Generate debug symbols even in release mode.
        target_link_options(BambuStudio PUBLIC "$<$<CONFIG:RELEASE>:/DEBUG>")
        target_link_libraries(BambuStudio user32.lib Setupapi.lib)
    elseif (MINGW)
        target_link_libraries(BambuStudio ws2_32 uxtheme setupapi)
    elseif (APPLE)
        target_link_libraries(BambuStudio "-framework OpenGL")
    else ()
        target_link_libraries(BambuStudio -ldl)
    endif ()
    #if (WIN32)
    #    find_library(PSAPI_LIB NAMES Psapi)
    #    target_link_libraries(BambuStudio ${PSAPI_LIB})
    #endif ()
endif ()

# On Windows, a shim application is required to produce a console / non console version of the Slic3r application.
# Also the shim may load the Mesa software OpenGL renderer if the default renderer does not support OpenGL 2.0 and higher.
if (WIN32)
    if (MINGW)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -municode")
    endif()

    add_executable(BambuStudio_app_gui WIN32 BambuStudio_app_msvc.cpp ${CMAKE_CURRENT_BINARY_DIR}/BambuStudio.rc)
    # Generate debug symbols even in release mode.
    if(MSVC)
        target_link_options(BambuStudio_app_gui PUBLIC "$<$<CONFIG:RELEASE>:/DEBUG>")
    endif()
    target_compile_definitions(BambuStudio_app_gui PRIVATE -DSLIC3R_WRAPPER_NOCONSOLE)
    add_dependencies(BambuStudio_app_gui BambuStudio)
    set_target_properties(BambuStudio_app_gui PROPERTIES OUTPUT_NAME "bambu-studio")
    target_link_libraries(BambuStudio_app_gui PRIVATE boost_headeronly)
endif ()

# Link the resources dir to where Slic3r GUI expects it
set(output_dlls_Release "")
set(output_dlls_Debug "")
set(output_dlls_RelWithDebInfo "")
if (WIN32)
    # This has to be a separate target due to the windows command line length limits
    add_custom_target(BambuStudioDllsCopy ALL DEPENDS BambuStudio)

    if (CMAKE_CONFIGURATION_TYPES)
        foreach (CONF ${CMAKE_CONFIGURATION_TYPES})
            file(TO_NATIVE_PATH "${CMAKE_CURRENT_BINARY_DIR}/${CONF}" WIN_CONF_OUTPUT_DIR)
            file(TO_NATIVE_PATH "${CMAKE_CURRENT_BINARY_DIR}/${CONF}/resources" WIN_RESOURCES_SYMLINK)
            add_custom_command(TARGET BambuStudio POST_BUILD
                COMMAND if exist "${WIN_CONF_OUTPUT_DIR}" "("
                        if not exist "${WIN_RESOURCES_SYMLINK}" "("
                            mklink /J "${WIN_RESOURCES_SYMLINK}" "${SLIC3R_RESOURCES_DIR_WIN}"
                        ")"
                    ")"
                COMMENT "Symlinking the resources directory into the build tree"
                VERBATIM
            )
        endforeach ()

        if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
            bambustudio_copy_dlls(BambuStudioDllsCopy "Debug" "d" output_dlls_Debug)
        elseif("${CMAKE_BUILD_TYPE}" STREQUAL "RelWithDebInfo")
            bambustudio_copy_dlls(BambuStudioDllsCopy "RelWithDebInfo" "" output_dlls_RelWithDebInfo)
        else()
            bambustudio_copy_dlls(BambuStudioDllsCopy "Release" "" output_dlls_Release)
        endif()
    else ()
        file(TO_NATIVE_PATH "${CMAKE_CURRENT_BINARY_DIR}/resources" WIN_RESOURCES_SYMLINK)
        add_custom_command(TARGET BambuStudio POST_BUILD
            COMMAND if not exist "${WIN_RESOURCES_SYMLINK}" "(" mklink /J "${WIN_RESOURCES_SYMLINK}" "${SLIC3R_RESOURCES_DIR_WIN}" ")"
            COMMENT "Symlinking the resources directory into the build tree"
            VERBATIM
        )
    endif ()


else ()
    if (NOT APPLE AND NOT FLATPAK)
        set(output_sos_Release "")
        set(output_sos_Debug "")
        add_custom_target(BambuStudioSosCopy ALL DEPENDS BambuStudio)
        if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
            bambustudio_copy_sos(BambuStudioSosCopy "Debug" "d" output_sos_Debug)
        else()
            bambustudio_copy_sos(BambuStudioSosCopy "Release" "" output_sos_Release)
        endif()
    endif()
    if (APPLE AND NOT CMAKE_MACOSX_BUNDLE)
        # On OSX, the name of the binary matches the name of the Application.
        add_custom_command(TARGET BambuStudio POST_BUILD
            COMMAND ln -sf BambuStudio bambu-studio
            WORKING_DIRECTORY "$<TARGET_FILE_DIR:BambuStudio>"
            VERBATIM)
    else ()
        add_custom_command(TARGET BambuStudio POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E echo "Post-build done"
            WORKING_DIRECTORY "$<TARGET_FILE_DIR:BambuStudio>"
            VERBATIM)
    endif ()
    if (XCODE)
        # Because of Debug/Release/etc. configurations (similar to MSVC) the slic3r binary is located in an extra level
        set(BIN_RESOURCES_DIR "${CMAKE_CURRENT_BINARY_DIR}/resources")
        # Use generator expression for Xcode multi-config builds
        set(BIN_CONF_DIR "$<CONFIG>")
    else ()
        set(BIN_RESOURCES_DIR "${CMAKE_CURRENT_BINARY_DIR}/../resources")
    endif ()
    if (CMAKE_MACOSX_BUNDLE)
        set(BIN_RESOURCES_DIR "${CMAKE_CURRENT_BINARY_DIR}/${BIN_CONF_DIR}/BambuStudio.app/Contents/Resources")
        set(MACOSX_BUNDLE_ICON_FILE Icon.icns)
        set(MACOSX_BUNDLE_BUNDLE_NAME "Bambu Studio")
        set(MACOSX_BUNDLE_SHORT_VERSION_STRING ${SLIC3R_VERSION})
        set(MACOSX_BUNDLE_COPYRIGHT "Copyright(C) 2021-2025 Lunkuo All Rights Reserved")
    endif()
    add_custom_command(TARGET BambuStudio POST_BUILD
        COMMAND ln -sfn "${SLIC3R_RESOURCES_DIR}" "${BIN_RESOURCES_DIR}"
        COMMENT "Symlinking the resources directory into the build tree"
        VERBATIM)
    #if (UNIX AND NOT APPLE)
    #    add_custom_command(TARGET BambuStudio POST_BUILD
    #        COMMAND ln -sfn "zh_cn" "${BIN_RESOURCES_DIR}/i18n/zh_CN"
    #        COMMENT "Symlinking zh_CN language setting to zh_cn"
    #        VERBATIM)
    #    add_custom_command(TARGET BambuStudio POST_BUILD
    #        COMMAND ln -sfn "pt-BR" "${BIN_RESOURCES_DIR}/i18n/pt_BR"
    #        COMMENT "Symlinking pt_BR language setting to pt_BR"
    #        VERBATIM)
    #endif()
endif ()

# Slic3r binary install target. Default build type is release in case no CMAKE_BUILD_TYPE is provided.
if( ("${CMAKE_BUILD_TYPE}" STREQUAL "Release") OR ("${CMAKE_BUILD_TYPE}" STREQUAL "") )
    set (build_type "Release")
    set(CMAKE_BUILD_POSTFIX "")
elseif ("${CMAKE_BUILD_TYPE}" STREQUAL "RelWithDebInfo")
    set (build_type "RelWithDebInfo")
    set(CMAKE_BUILD_POSTFIX "")
else()
    set (build_type "Debug")
    set(CMAKE_BUILD_POSTFIX "d")
endif()
message(STATUS "libslic3r-CMAKE_BUILD_TYPE: ${build_type}")
message(STATUS "CMAKE_CURRENT_BINARY_DIR: ${CMAKE_CURRENT_BINARY_DIR}")
if (WIN32)
    install(TARGETS BambuStudio RUNTIME DESTINATION "${CMAKE_INSTALL_PREFIX}")
    if (MSVC)
        install(TARGETS BambuStudio_app_gui RUNTIME DESTINATION "${CMAKE_INSTALL_PREFIX}")
    endif ()
    install(FILES ${output_dlls_${build_type}} DESTINATION "${CMAKE_INSTALL_PREFIX}")
else ()
    if (APPLE)
    else()
        install(FILES ${output_sos_${build_type}} DESTINATION "${CMAKE_INSTALL_PREFIX}")
    endif()
    install(TARGETS BambuStudio RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}" BUNDLE DESTINATION ${CMAKE_INSTALL_BINDIR})
endif ()
