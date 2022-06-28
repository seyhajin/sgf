#
# Copyright (C) 2005-2020 Centre National d'Etudes Spatiales (CNES)
#
# This file is part of Orfeo Toolbox
#
#     https://www.orfeo-toolbox.org/
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

# Locate the glfw3 library
# This module defines the following variables:
# GLFW3_LIBRARY, the name of the library;
# GLFW3_INCLUDE_DIR, where to find glfw3 include files.
# GLFW3_FOUND, true if both the GLFW3_LIBRARY and GLFW3_INCLUDE_DIR have been found.
#
# To help locate the library and include file, you could define an environment variable called
# GLFW3_ROOT which points to the root of the glfw3 library installation. This is pretty useful
# on a Windows platform.
#
#
# Usage example to compile an "executable" target to the glfw3 library:
#
# FIND_PACKAGE (glfw3 REQUIRED)
# INCLUDE_DIRECTORIES (${GLFW3_INCLUDE_DIR})
# ADD_EXECUTABLE (executable ${EXECUTABLE_SRCS})
# TARGET_LINK_LIBRARIES (executable ${GLFW3_LIBRARY})
#
# TODO:
# Allow the user to select to link to a shared library or to a static library.

#Search for the include file...
FIND_PATH(GLFW3_INCLUDE_DIR GLFW/glfw3.h DOC "Path to GLFW3 include directory."
    HINTS
      $ENV{GLFW3_ROOT}
    PATH_SUFFIX
      include # For finding the include file under the root of the glfw3 expanded archive, typically on Windows.
)

FIND_LIBRARY(GLFW3_LIBRARY DOC "Absolute path to GLFW3 library."
    NAMES glfw3dll glfw3 glfw
    HINTS
      $ENV{GLFW3_ROOT}
    PATH_SUFFIXES
       lib # For finding the library file under the root of the glfw3 expanded archive, typically on Windows.
)


IF(GLFW3_INCLUDE_DIR AND EXISTS "${GLFW3_INCLUDE_DIR}/GLFW/glfw3.h")
    FILE(STRINGS "${GLFW3_INCLUDE_DIR}/GLFW/glfw3.h" GLFW3_version_str
         REGEX "^#[\t ]*define[\t ]+GLFW3_VERSION_(MAJOR|MINOR|REVISION)[\t ]+[0-9]+$")

    UNSET(GLFW3_VERSION_STRING)
    FOREACH(VPART MAJOR MINOR REVISION)
        FOREACH(VLINE ${GLFW3_version_str})
            IF(VLINE MATCHES "^#[\t ]*define[\t ]+GLFW3_VERSION_${VPART}")
                STRING(REGEX REPLACE "^#[\t ]*define[\t ]+GLFW3_VERSION_${VPART}[\t ]+([0-9]+)$" "\\1"
                       GLFW3_VERSION_PART "${VLINE}")
                IF(GLFW3_VERSION_STRING)
                    SET(GLFW3_VERSION_STRING "${GLFW3_VERSION_STRING}.${GLFW3_VERSION_PART}")
                ELSE()
                    SET(GLFW3_VERSION_STRING "${GLFW3_VERSION_PART}")
                ENDIF()
                UNSET(GLFW3_VERSION_PART)
            ENDIF()
        ENDFOREACH()
    ENDFOREACH()
ENDIF()

INCLUDE(${CMAKE_ROOT}/Modules/FindPackageHandleStandardArgs.cmake)
find_package_handle_standard_args(GLFW3
  REQUIRED_VARS GLFW3_LIBRARY GLFW3_INCLUDE_DIR
  VERSION_VAR GLFW3_VERSION_STRING)
