# cmake-format: off
#
# bind2iface
# Copyright (c) 2024 Adam Allaf <adam.allaf@hotmail.com>
#
# This file is part of bind2iface, licensed under
# the GNU General Public License, version 2.
# For details, see the LICENSE file in the root of this project.
#
# cmake-format: on

FIND_PACKAGE(Git)
IF (DEFINED GIT_EXECUTABLE)
  EXECUTE_PROCESS(
    COMMAND ${GIT_EXECUTABLE} rev-parse HEAD
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_COMMIT_SHA_LONG
    RESULT_VARIABLE GIT_REV_PARSE_RESULT
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )
  IF (NOT GIT_REV_PARSE_RESULT)
    STRING(
      SUBSTRING ${GIT_COMMIT_SHA_LONG}
                0
                8
                GIT_COMMIT_SHA
    )
    EXECUTE_PROCESS(
      COMMAND ${GIT_EXECUTABLE} describe --tags --match "v*"
      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
      OUTPUT_VARIABLE GIT_DESCRIBE
      RESULT_VARIABLE GIT_DESCRIBE_ERROR
      OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    IF (GIT_DESCRIBE_ERROR)
      SET(BUILD_VERSION v0.0.0-0-${GIT_COMMIT_SHA})
    ELSE ()
      SET(BUILD_VERSION ${GIT_DESCRIBE})
    ENDIF ()
  ELSE ()
    SET(BUILD_VERSION v0.0.0)
  ENDIF ()
  STRING(
    REGEX MATCH
          "^v([0-9]+)\\.([0-9]+)\\.([0-9]+)"
          BUILD_VERSION_MATCH
          ${BUILD_VERSION}
  )
  SET(GIT_TAG_VERSION ${CMAKE_MATCH_1}.${CMAKE_MATCH_2}.${CMAKE_MATCH_3})
ELSE ()
  SET(GIT_TAG_VERSION 0.0.0)
ENDIF ()
