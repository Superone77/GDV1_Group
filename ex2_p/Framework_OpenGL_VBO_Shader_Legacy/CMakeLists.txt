cmake_minimum_required(VERSION 3.8)

project(uebung_02 LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)

find_package(Qt5 COMPONENTS Gui Core REQUIRED)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(${PROJECT_NAME}
        main.cpp
        mainwindow.cpp
        TriangleMesh.cpp
        mainwindow.h
        TriangleMesh.h
        Vec3.h
        shader.h
        )

target_link_libraries(${PROJECT_NAME} Qt5::Core Qt5::Gui)

#On Windows and MacOS, we should run *deployqt
#in order to make sure the required
#Qt dlls/dylibs are available. It is in the same directory as QMake.
get_target_property(_qmake_executable Qt5::qmake IMPORTED_LOCATION)
get_filename_component(_qt_bin_dir "${_qmake_executable}" DIRECTORY)
IF(WIN32)
    find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS "${_qt_bin_dir}")
    add_custom_command(TARGET uebung_02 POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E env PATH="${_qt_bin_dir}" "${WINDEPLOYQT_EXECUTABLE}" "$<TARGET_FILE:uebung_02>"
            COMMENT "Deploying Qt library files...")
ELSEIF(APPLE)
    target_link_libraries(${PROJECT_NAME} "-framework OpenGL")
    target_link_libraries(${PROJECT_NAME} "-framework GLUT")
    find_program(MACDEPLOYQT_EXECUTABLE macdeployqt HINTS "${_qt_bin_dir}")
    add_custom_command(TARGET uebung_02 POST_BUILD
            COMMAND "${MACDEPLOYQT_EXECUTABLE}" "$<TARGET_FILE:uebung_02>"
            COMMENT "Deploying Qt library files...")
ENDIF()
