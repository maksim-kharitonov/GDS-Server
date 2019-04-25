cmake_minimum_required (VERSION 3.5)

# Maps to a solution file (GDS_Console.sln). The solution will 
# have all targets (exe, lib, dll) as projects (.vcproj)
project (GdsServer)

# Turn on the ability to create folders to organize projects (.vcproj)
# It creates "CMakePredefinedTargets" folder by default and adds CMake
# defined projects like INSTALL.vcproj and ZERO_CHECK.vcproj
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Set compiler flags and options. 
# Here it is setting the Visual Studio warning level to 4
# set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")

# Command to output information to the console
# Useful for displaying errors, warnings, and debugging
message ("cxx Flags: " ${CMAKE_CXX_FLAGS})

# Properties->C/C++->General->Additional Include Directories
include_directories (.)

file (GLOB APP_SOURCES
  "*.h"
  "*.cpp"
  "lib/*.h"
  "lib/*.cpp")
source_group("" FILES ${APP_SOURCES})

# Set Properties->General->Configuration Type to Application(.exe)
# Creates app.exe with the listed sources (main.cxx)
# Adds sources to the Solution Explorer
add_executable (gdsserver ${APP_SOURCES})

# Creates a folder "executables" and adds target 
# project (app.vcproj) under it
set_property(TARGET gdsserver PROPERTY FOLDER "executables")

# Properties->General->Output Directory
set_target_properties(gdsserver PROPERTIES
                      RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
					  
add_custom_command(
        TARGET gdsserver POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${PROJECT_SOURCE_DIR}/resources/
                ${PROJECT_BINARY_DIR}/resources)

# Adds logic to INSTALL.vcproj to copy app.exe to destination directory
install (TARGETS gdsserver
         RUNTIME DESTINATION ${PROJECT_SOURCE_DIR}/_install)
install(DIRECTORY "resources" 
        DESTINATION ${PROJECT_SOURCE_DIR}/_install
        FILES_MATCHING PATTERN "*.*" )
