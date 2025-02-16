# Copied from: https://fossies.org/linux/apt/CMake/FindUdev.cmake

# - Try to find UDEV
# Once done, this will define
#
#  YAML_CPP_FOUND - system has yaml-cpp
#  YAML_CPP_INCLUDE_DIR - the yaml-cppV include directories
#  YAML_CPP_LIB - the yaml-cpp library
find_package(PkgConfig)

pkg_check_modules(YAML_CPP REQUIRED yaml-cpp)
 
find_path(YAML_CPP_INCLUDEDIR
   NAMES libyaml-cpp.h
   PATHS ${YAML_CPP_PKGCONF_INCLUDEDIR}
)


find_library(YAML_CPP_LIB
             NAMES yaml-cpp
             PATH_SUFFIXES lib64 lib
             PATHS  /usr/local
                    /usr
                    /sw
                    /opt/local
                    /opt/csw
                    /opt
                    ${YAML_CPP_DIR}/lib)

# YAML does not use the standard INCLUDE_DIR so make it here
set( YAML_CPP_INCLUDE_DIR ${YAML_CPP_INCLUDEDIR})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(yaml-cpp DEFAULT_MSG YAML_CPP_INCLUDE_DIR YAML_CPP_LIB)

mark_as_advanced(YAML_CPP_INCLUDE_DIR YAML_CPP_LIB)