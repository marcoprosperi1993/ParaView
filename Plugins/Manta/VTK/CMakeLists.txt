PROJECT ( vtkManta )

#vtkManta is an interface from VTK's rendering classes to Manta,
#a "real time" Ray Tracer. 

#It creates a library that includes Manta specific subclasses
#of VTK's rendering classes, RenderWindow, Renderer, Camera, etc.
#These can be instantiated by name, or by using object factory overloading 
#at post compile time (ie, set VTK_AUTOLOAD_PATH to vtkManta's build 
#directory and run a normal vtk app)

# CMake version and policy issues
CMAKE_MINIMUM_REQUIRED( VERSION 2.4 )
IF( COMMAND CMAKE_POLICY )
  CMAKE_POLICY( SET CMP0003 NEW )
ENDIF( COMMAND CMAKE_POLICY )

# Find VTK and take settings we need from it #################################
INCLUDE ( ${CMAKE_ROOT}/Modules/FindVTK.cmake )
FIND_PACKAGE ( VTK REQUIRED )  
INCLUDE ( ${VTK_USE_FILE} )

#TODO: What is this? Is it necessary?
# Set the directory for exporting vtkManta executables and libraries
# to VTK's build directory (just if necessary)
SET ( EXES_LIBS_EXPORT_DIR ${VTK_DIR} )
MARK_AS_ADVANCED ( EXES_LIBS_EXPORT_DIR )

# Give user option of building shared or static libraries.
# Default to the choice made for VTK.
OPTION ( MANTA_BUILD_SHARED_LIBS "Build vtkManta with shared libraries." 
  ${VTK_BUILD_SHARED_LIBS} )

# Set specific VTK libraries to link against
SET ( VTK_LINK_LIBRARIES vtkRendering vtkGraphics)
IF ( VTK_USE_PARALLEL )
  SET (VTK_LINK_LIBRARIES ${VTK_LINK_LIBRARIES} vtkParallel )
ENDIF( VTK_USE_PARALLEL )
IF (BUILD_AGAINST_PARAVIEW)
  SET ( VTK_LINK_LIBRARIES ${VTK_LINK_LIBRARIES} vtkPVFilters)
  INCLUDE_DIRECTORIES(
    ${PARAVIEW_INCLUDE_DIRS}
    )
ENDIF (BUILD_AGAINST_PARAVIEW)

# Find Manta and take settings we need from it ###############################
SET ( MANTA_BUILD_NAMES
  buildmanta build_manta build-manta buildManta build_Manta build-Manta
  )

SET ( MANTA_BUILD_SEARCH_PATH "" )
FOREACH ( manta_build_name ${MANTA_BUILD_NAMES} )
  SET ( MANTA_BUILD_SEARCH_PATH
    ${MANTA_BUILD_SEARCH_PATH}
    ../${manta_build_name}
    /bin/${manta_build_name}
    /usr/bin/${manta_build_name}
    /usr/local/bin/${manta_build_name}
    )
ENDFOREACH ( manta_build_name )

FIND_PATH ( MANTA_BUILD 
  MantaConfigure.cmake ${MANTA_BUILD_SEARCH_PATH} 
  DOC "Manta build directory" 
  )
IF ( MANTA_BUILD )
  SET ( MANTA_BUILD_INCLUDE ${MANTA_BUILD}/include)
  SET ( MANTA_BUILD_LIB ${MANTA_BUILD}/lib)
  #export this so paraview plugin can use it
  SET (MANTA_BUILD_LIB ${MANTA_BUILD_LIB} PARENT_SCOPE)
ENDIF ( MANTA_BUILD )

#TODO: Change Manta's config.h.in to point to source tree then autofind it
# Locate Manta source directory through Manta build directory
FIND_PATH ( MANTA_SOURCE 
  manta-commands.txt ../manta ../Manta 
  DOC "Manta source directory" 
  )
IF ( MANTA_BUILD AND NOT MANTA_SOURCE )
  INCLUDE ( ${MANTA_BUILD}/CMakeFiles/CMakeDirectoryInformation.cmake )
  SET ( MANTA_SOURCE ${CMAKE_RELATIVE_PATH_TOP_SOURCE} )
ENDIF ( MANTA_BUILD AND NOT MANTA_SOURCE )

# Include Manta source and build-include directories
INCLUDE_DIRECTORIES(${MANTA_SOURCE} ${MANTA_BUILD_INCLUDE})
LINK_DIRECTORIES ( ${MANTA_BUILD_LIB} )

# Set specific Manta libraries to link against
SET ( MANTA_LINK_LIBRARIES
  Manta_Factory
  Manta_UserInterface
  Manta_Engine
  Manta_Model
  Manta_Image
  Manta_Interface
  Manta_Core
  )

#Configure vtkManta source proper ############################################

# Define a flag exported to vtkMantaConfigure.h which is used in some
# ParaView-dependent #ifdefs
IF (NOT BUILD_AGAINST_PARAVIEW)
  SET ( BUILD_AGAINST_PARAVIEW 0 )
ENDIF (NOT BUILD_AGAINST_PARAVIEW)

#TODO: I think having the .h in the build tree is breaking wrapping
#choose correct lineage for renderwindow class based on platform
IF(VTK_USE_CARBON)
  SET(RenderWindowType "vtkCarbonRenderWindow")
ELSEIF(VTK_USE_COCOA)
  SET(RenderWindowType "vtkCocoaRenderWindow")
ELSEIF(VTK_USE_X)
  SET(RenderWindowType "vtkXOpenGLRenderWindow")
ELSEIF(VTK_OPENGL_HAS_OSMESA)
 SET(RenderWindowType "vtkOSOpenGLRenderWindow")
ELSE()
  SET(RenderWindowType "vtkWin32OpenGLRenderWindow")
ENDIF()
configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/vtkMantaRenderWindow.h.in"
  "${CMAKE_CURRENT_BINARY_DIR}/vtkMantaRenderWindow.h"
  @ONLY
  IMMEDIATE
  )

# Set VTKMANTA_BUILD_SHARED_LIBS to 1 or 0 for safe keeping in 
#vtkMantaConfigure.h
IF ( MANTA_BUILD_SHARED_LIBS )
  SET ( VTKMANTA_BUILD_SHARED_LIBS_CONFIG 1 )
ELSE ( MANTA_BUILD_SHARED_LIBS )
  SET( VTKMANTA_BUILD_SHARED_LIBS_CONFIG 0 )
ENDIF ( MANTA_BUILD_SHARED_LIBS )

# setup include paths
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR})
INCLUDE_DIRECTORIES(${PROJECT_BINARY_DIR})

# Setup the output directories.
#TODO: What are these for, are they necessary?
SET ( LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin CACHE PATH
      "Single output directory for building all libraries."
    )
SET ( EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin CACHE PATH
      "Single output directory for building all executables."
    )
MARK_AS_ADVANCED ( LIBRARY_OUTPUT_PATH EXECUTABLE_OUTPUT_PATH )

# Create the header to configure the classes.
CONFIGURE_FILE ( ${PROJECT_SOURCE_DIR}/vtkMantaConfigure.h.in
                 ${PROJECT_BINARY_DIR}/${PROJECT_NAME}Configure.h
                 @ONLY
                 IMMEDIATE
               )

# Set vtkManta source files.
SET ( MANTA_CLASSES
  vtkMantaActor.cxx
  vtkMantaCamera.cxx
  vtkMantaLight.cxx
  vtkMantaManager.cxx
  vtkMantaPolyDataMapper.cxx
  vtkMantaProperty.cxx
  vtkMantaRenderer.cxx
  vtkMantaRenderWindow.cxx
  vtkMantaTestSource.cxx
  vtkMantaTexture.cxx
  )

IF (BUILD_AGAINST_PARAVIEW)
  # paraview uses its own LOD actor, put it in here
  SET ( MANTA_CLASSES ${MANTA_CLASSES} vtkMantaLODActor.cxx)
ENDIF (BUILD_AGAINST_PARAVIEW)

SET ( MANTA_SRCS
  ${MANTA_CLASSES}
  vtkMantaObjectFactory.cxx
  )

# Create vtkManta libraries
#TODO: Should we put the wrapping srcs here too? 
#Right now, ${vtkMantaInstantiator_SRCS} is empty
ADD_LIBRARY ( ${PROJECT_NAME} ${MANTA_SRCS})
TARGET_LINK_LIBRARIES ( ${PROJECT_NAME} 
  ${VTK_LINK_LIBRARIES} ${MANTA_LINK_LIBRARIES} )

# Wrapping for use from interpretted languages ##############################

#SET ( VTK_WRAP_HINTS "${PROJECT_SOURCE_DIR}/hints" )
 
# Create the instantiator for these classes.

# Be sure to wrap vtkMantaRenderWindow.h, which lives in the build directory.
# This code was lifted from ParaViewPlugins.cmake
SET(HDRS)
SET(REALSRCS)
SET(INST_SRCS)
FOREACH(SRC ${MANTA_SRCS})
  GET_FILENAME_COMPONENT(src_name "${SRC}" NAME_WE)
  GET_FILENAME_COMPONENT(src_path "${SRC}" ABSOLUTE)
  GET_FILENAME_COMPONENT(src_path "${src_path}" PATH)
  SET(HDR)
  IF(EXISTS "${src_path}/${src_name}.h")
    SET(HDR "${src_path}/${src_name}.h")
  ELSEIF(EXISTS "${CMAKE_CURRENT_BINARY_DIR}/${src_name}.h")
    SET(HDR "${CMAKE_CURRENT_BINARY_DIR}/${src_name}.h")
  ENDIF()
  LIST(APPEND HDRS ${HDR})
  IF(NOT HDR MATCHES ${SRC})
    SET(REALSRCS ${REALSRCS} ${SRC})
  ENDIF(NOT HDR MATCHES ${SRC})
ENDFOREACH(SRC ${ARGN})

SET ( VTK_USE_INSTANTIATOR_NEW 1 )

VTK_MAKE_INSTANTIATOR3 (${PROJECT_NAME}Instantiator
  vtkMantaInstantiator_SRCS
  "${HDRS}"
  "VTK_${PROJECT_NAME}_EXPORT"
  ${PROJECT_BINARY_DIR}
  "${PROJECT_NAME}Configure.h"
  )

# Give the option to create Tcl wrappers, but only if VTK is wrapped in Tcl
#TODO: Test if this works
IF (VTK_WRAP_TCL)
  OPTION ( MANTA_WRAP_TCL "Wrap vtkManta to use from Tcl" ${VTK_WRAP_TCL} )
ENDIF (VTK_WRAP_TCL)
IF ( MANTA_WRAP_TCL )
  INCLUDE ( ${VTK_CMAKE_DIR}/vtkWrapTcl.cmake )
  VTK_WRAP_TCL3 ( ${PROJECT_NAME}TCL vtkMantaTCL_SRCS "${HDRS}" "" )
  ADD_LIBRARY ( ${PROJECT_NAME}TCL ${vtkMantaTCL_SRCS} )
  
  TARGET_LINK_LIBRARIES ( ${PROJECT_NAME}TCL ${PROJECT_NAME} 
    ${MANTA_LINK_LIBRARIES} )
  FOREACH ( c ${VTK_LINK_LIBRARIES} )
    TARGET_LINK_LIBRARIES ( ${PROJECT_NAME}TCL ${c}TCL )
  ENDFOREACH ( c )
  
  # Create the list of kits that are required.
  SET ( VTKManta_KITS "" )
  FOREACH ( c ${VTK_LINK_LIBRARIES} )
    SET ( VTKManta_KITS "${VTKManta_KITS} ${c}" )
  ENDFOREACH ( c )
  
  FOREACH ( c ${MANTA_SRCS} )
    GET_FILENAME_COMPONENT ( VTKManta_LAST_CLASS ${c} NAME_WE )
  ENDFOREACH ( c )
  
  IF ( CMAKE_CONFIGURATION_TYPES )
    FOREACH ( config ${CMAKE_CONFIGURATION_TYPES} )
      SET ( VTKManta_TCL_LIBRARY_DIR "${LIBRARY_OUTPUT_PATH}/${config}" )
      CONFIGURE_FILE ( ${PROJECT_SOURCE_DIR}/pkgIndex.tcl.in
        ${PROJECT_BINARY_DIR}/${config}/pkgIndex.tcl
        @ONLY
        IMMEDIATE
        )
    ENDFOREACH ( config )
  ELSE ( CMAKE_CONFIGURATION_TYPES )
    SET ( VTKManta_TCL_LIBRARY_DIR "${LIBRARY_OUTPUT_PATH}" )
    CONFIGURE_FILE ( ${PROJECT_SOURCE_DIR}/pkgIndex.tcl.in  
      ${PROJECT_BINARY_DIR}/pkgIndex.tcl     
      @ONLY
      IMMEDIATE
      )
  ENDIF ( CMAKE_CONFIGURATION_TYPES )   
ENDIF ( MANTA_WRAP_TCL )

# Give the option to create Python wrappers, but only if VTK is wrapped 
# in Python.
#TODO: This compiles and Cone.py runs and manta is running, but the 
#rendered result doesn't look like it was drawn by manta
#TODO: Can't turn this on inside paraview, because vtkPVFilters is not 
#wrapped, so vtkMantaLODActor can't be either
IF (VTK_WRAP_PYTHON AND NOT BUILD_AGAINST_PARAVIEW)
  OPTION ( MANTA_WRAP_PYTHON "Wrap vtkManta to use from Python" 
    ${VTK_WRAP_PYTHON} )
ENDIF (VTK_WRAP_PYTHON AND NOT BUILD_AGAINST_PARAVIEW)
IF ( MANTA_WRAP_PYTHON )
  INCLUDE ( ${VTK_CMAKE_DIR}/vtkWrapPython.cmake )
  VTK_WRAP_PYTHON3 ( ${PROJECT_NAME}Python vtkMantaPYTHON_SRCS "${HDRS}" )
  ADD_LIBRARY ( ${PROJECT_NAME}PythonD ${vtkMantaPYTHON_SRCS} )
  ADD_LIBRARY ( ${PROJECT_NAME}Python MODULE ${PROJECT_NAME}PythonInit.cxx )
  
  TARGET_LINK_LIBRARIES ( ${PROJECT_NAME}PythonD ${PROJECT_NAME} 
    ${MANTA_LINK_LIBRARIES} )
  FOREACH ( c ${VTK_LINK_LIBRARIES} )
    TARGET_LINK_LIBRARIES ( ${PROJECT_NAME}PythonD ${c}PythonD )
  ENDFOREACH ( c )
  TARGET_LINK_LIBRARIES ( ${PROJECT_NAME}Python ${PROJECT_NAME}PythonD )
ENDIF ( MANTA_WRAP_PYTHON )

# Give the option to create Java wrappers, but only if VTK is wrapped in Java.
IF (VTK_WRAP_JAVA)
  OPTION ( MANTA_WRAP_JAVA "Wrap vtkManta to use from Java" ${VTK_WRAP_JAVA} )
ENDIF (VTK_WRAP_JAVA)
IF ( MANTA_WRAP_JAVA )
  INCLUDE ( ${VTK_CMAKE_DIR}/vtkWrapJava.cmake )
  SET ( VTK_WRAP_JAVA3_INIT_DIR "${PROJECT_SOURCE_DIR}" )
  SET ( VTK_JAVA_HOME ${PROJECT_BINARY_DIR}/java/${PROJECT_NAME} )
  MAKE_DIRECTORY ( ${VTK_JAVA_HOME} )
  VTK_WRAP_JAVA3 ( ${PROJECT_NAME}Java vtkMantaJAVA_SRCS "${HDRS}" )
  ADD_LIBRARY ( ${PROJECT_NAME}Java ${vtkMantaJAVA_SRCS} )
  
  TARGET_LINK_LIBRARIES ( ${PROJECT_NAME}Java ${PROJECT_NAME} )
  FOREACH ( c ${VTK_LINK_LIBRARIES} )
    TARGET_LINK_LIBRARIES ( ${PROJECT_NAME}Java ${c}Java )
  ENDFOREACH ( c )
ENDIF ( MANTA_WRAP_JAVA )

#When building in ParaView, always wrap vtkManta into the ParaView client 
#server language so that the plugin can create vtkManta classes. 
#NOTE: normally the ADD_PARAVIEW_PLUGIN macro does this, but we
#do it here so that we can split up the plugin into two directories.
IF (BUILD_AGAINST_PARAVIEW)

  #TODO: Could we move this to the ParaView directory?
  #TODO: Could we add this to the base vtkManta library?
  #TODO: CSI initializer is hacked together, see call in vtkServerSideFactory
  #would be better if it was initialized automatically like plugin is
  INCLUDE ( ${VTKCS_SOURCE_DIR}/vtkClientServer.cmake )
  VTK_WRAP_ClientServer ( ${PROJECT_NAME}CS vtkMantaCS_SRCS "${HDRS}" )
  ADD_LIBRARY ( ${PROJECT_NAME}CS SHARED ${vtkMantaCS_SRCS})
  TARGET_LINK_LIBRARIES( ${PROJECT_NAME}CS vtkManta 
    vtkClientServer vtkPVServerManager )

ENDIF (BUILD_AGAINST_PARAVIEW)

#Examples ####################################################################
OPTION ( MANTA_BUILD_EXAMPLES "Build vtkManta example applications" 
  ${VTK_BUILD_EXAMPLES} )
IF ( MANTA_BUILD_EXAMPLES )
  ADD_SUBDIRECTORY(Examples)
ENDIF ( MANTA_BUILD_EXAMPLES )
 
#Tests #######################################################################
#TODO: Make these tests meaningful and cdash them regularly
OPTION ( MANTA_BUILD_TESTING "Build vtkManta examples applications" 
  ${VTK_BUILD_TESTING} )
IF ( MANTA_BUILD_TESTING )
  ENABLE_TESTING()
  INCLUDE( CTest )
  ADD_SUBDIRECTORY(Testing)
ENDIF ( MANTA_BUILD_TESTING )

