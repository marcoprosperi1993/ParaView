###############################################################################
# This file defines the macro for cpack configuration generation for a
# ParaView-based client builds with custom branding and configuration.
#
# build_paraview_client_cpack_config_init(
#   PACKAGE_NAME "ParaView"
#   ORGANIZATION "Kitware Inc."
#   VERSION_MAJOR 0
#   VERSION_MINOR 0
#   VERSION_PATCH 0
#   DESCRIPTION "Short Description"
#   LICENSE_FILE "License.txt"
#   DESCRIPTION_FILE "Description.txt"
#   PACKAGE_EXECUTABLES "paraview;ParaView"
#   )
#
# After build_paraview_client_cpack_config_init() the application is free to
# modify cpack variables as deemed necessary before calling
# build_paraview_client_cpack_config() which will generate the config.
#
# build_paraview_client_cpack_config()
#

MACRO(build_paraview_client_cpack_config_init)
  PV_PARSE_ARGUMENTS("BCC"
    "PACKAGE_NAME;ORGANIZATION;VERSION_MAJOR;VERSION_MINOR;VERSION_PATCH;DESCRIPTION;LICENSE_FILE;DESCRIPTION_FILE;PACKAGE_EXECUTABLES;"
    ""
    ${ARGN}
    )

  IF(APPLE)
    SET(CPACK_BINARY_TBZ2 OFF)
    SET(CPACK_BINARY_DRAGNDROP ON)
    SET(CPACK_BINARY_PACKAGEMAKER OFF)
    SET(CPACK_BINARY_STGZ OFF)
  ENDIF(APPLE)

  SET(CPACK_PACKAGE_NAME "${BCC_PACKAGE_NAME}")
  SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${BCC_DESCRIPTION}")
  SET(CPACK_PACKAGE_VENDOR "${BCC_ORGANIZATION}")
  IF (BCC_DESCRIPTION_FILE)
    SET(CPACK_PACKAGE_DESCRIPTION_FILE "${BCC_DESCRIPTION_FILE}")
  ENDIF()
  IF (BCC_LICENSE_FILE)
    SET(CPACK_RESOURCE_FILE_LICENSE "${BCC_LICENSE_FILE}")
  ENDIF()
  SET(CPACK_PACKAGE_VERSION_MAJOR "${BCC_VERSION_MAJOR}")
  SET(CPACK_PACKAGE_VERSION_MINOR "${BCC_VERSION_MINOR}")
  SET(CPACK_PACKAGE_VERSION_PATCH "${BCC_VERSION_PATCH}")
  SET(CPACK_PACKAGE_EXECUTABLES "${BCC_PACKAGE_EXECUTABLES}")
  SET(CPACK_PACKAGE_INSTALL_DIRECTORY "ParaView ${BCC_VERSION_MAJOR}.${BCC_VERSION_MINOR}.${BCC_VERSION_PATCH}")
  SET(CPACK_NSIS_MODIFY_PATH OFF)
  SET(CPACK_STRIP_FILES OFF)
  SET(CPACK_SOURCE_STRIP_FILES OFF)
  SET (CPACK_OUTPUT_CONFIG_FILE
    "${CMAKE_CURRENT_BINARY_DIR}/CPack${BCC_PACKAGE_NAME}Config.cmake")

  IF (CMAKE_SYSTEM_PROCESSOR MATCHES "unknown")
    SET (CMAKE_SYSTEM_PROCESSOR "x86")
  ENDIF (CMAKE_SYSTEM_PROCESSOR MATCHES "unknown")
  IF(NOT DEFINED CPACK_SYSTEM_NAME)
    SET(CPACK_SYSTEM_NAME ${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR})
  ENDIF(NOT DEFINED CPACK_SYSTEM_NAME)
  IF(${CPACK_SYSTEM_NAME} MATCHES Windows)
    IF(CMAKE_CL_64)
      SET(CPACK_SYSTEM_NAME win64-${CMAKE_SYSTEM_PROCESSOR})
    ELSE(CMAKE_CL_64)
      SET(CPACK_SYSTEM_NAME win32-${CMAKE_SYSTEM_PROCESSOR})
    ENDIF(CMAKE_CL_64)
  ENDIF(${CPACK_SYSTEM_NAME} MATCHES Windows)

  SET (CPACK_INSTALL_CMAKE_PROJECTS
    "${ParaView_BINARY_DIR}" "ParaView Runtime Libs" "Runtime" "/"
    "${ParaView_BINARY_DIR}" "VTK Runtime Libs" "RuntimeLibraries" "/"
    "${CMAKE_CURRENT_BINARY_DIR}" "${BCC_PACKAGE_NAME} Components" "BrandedRuntime" "/"
  )
  
  # Override this variable to choose a different component for mac drag-n-drop
  # generator.
  SET (CPACK_INSTALL_CMAKE_PROJECTS_DRAGNDROP
    ${CPACK_INSTALL_CMAKE_PROJECTS})
ENDMACRO(build_paraview_client_cpack_config_init)

MACRO(build_paraview_client_cpack_config)
  CONFIGURE_FILE("${ParaView_CMAKE_DIR}/ParaViewCPackOptions.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/CPack${CPACK_PACKAGE_NAME}Options.cmake"
    @ONLY
    )
  SET (CPACK_PROJECT_CONFIG_FILE
    "${CMAKE_CURRENT_BINARY_DIR}/CPack${CPACK_PACKAGE_NAME}Options.cmake")
  INCLUDE(CPack)
ENDMACRO(build_paraview_client_cpack_config)


# Function to install qt libraries. qtliblist is a list of libraries to install
# of the form "QTCORE QTGUI QTNETWORK QTXML QTTEST QTSQL" etc.
FUNCTION(install_qt_libs qtliblist componentname)
  IF (NOT APPLE)
    FOREACH(qtlib ${qtliblist})
      IF (NOT WIN32)
        IF (QT_${qtlib}_LIBRARY_RELEASE)
          GET_FILENAME_COMPONENT(QT_LIB_DIR_tmp ${QT_${qtlib}_LIBRARY_RELEASE} PATH)
          GET_FILENAME_COMPONENT(QT_LIB_NAME_tmp ${QT_${qtlib}_LIBRARY_RELEASE} NAME)
          FILE(GLOB QT_LIB_LIST RELATIVE "${QT_LIB_DIR_tmp}" "${QT_${qtlib}_LIBRARY_RELEASE}*")
          INSTALL(CODE "
                MESSAGE(STATUS \"Installing \${CMAKE_INSTALL_PREFIX}/${PV_INSTALL_LIB_DIR}/${QT_LIB_NAME_tmp}\")
                EXECUTE_PROCESS (WORKING_DIRECTORY ${QT_LIB_DIR_tmp}
                     COMMAND tar c ${QT_LIB_LIST}
                     COMMAND tar -xC \${CMAKE_INSTALL_PREFIX}/${PV_INSTALL_LIB_DIR})
                     " COMPONENT ${componentname})
        ENDIF (QT_${qtlib}_LIBRARY_RELEASE)
      ELSE (NOT WIN32)
        GET_FILENAME_COMPONENT(QT_DLL_PATH_tmp ${QT_QMAKE_EXECUTABLE} PATH)
          INSTALL(FILES ${QT_DLL_PATH_tmp}/${qtlib}4.dll 
                  DESTINATION ${PV_INSTALL_LIB_DIR} 
                  COMPONENT ${componentname})
      ENDIF (NOT WIN32)
    ENDFOREACH(qtlib)
  ENDIF (NOT APPLE)
ENDFUNCTION(install_qt_libs)
