cmake_minimum_required(VERSION 3.25)

# Ninja learns each object's headers from cl's /showIncludes lines, matched byte for byte against the
# prefix CMake detected at configure time. A localized cl writes that prefix in the console's code page,
# so a build from a console on another code page records no headers and leaves objects stale after
# header edits, with no error. Fails the build instead, whichever way it was started.

foreach(required COMPILER COMPILER_INFO PROBE_DIR)
    if(NOT DEFINED ${required})
        message(FATAL_ERROR "${required} must be defined")
    endif()
endforeach()

# Read as bytes: a prefix detected on a legacy code page isn't valid UTF-8.
file(READ "${COMPILER_INFO}" compiler_info)
if(NOT compiler_info MATCHES "CMAKE_CXX_CL_SHOWINCLUDES_PREFIX \"([^\"]+)\"")
    return()
endif()
set(detected_prefix "${CMAKE_MATCH_1}")

file(MAKE_DIRECTORY "${PROBE_DIR}")
file(WRITE "${PROBE_DIR}/probe.h" "")
file(WRITE "${PROBE_DIR}/probe.cpp" "#include \"probe.h\"\n")
execute_process(
    COMMAND "${COMPILER}" /nologo /showIncludes /Zs /TP probe.cpp
    WORKING_DIRECTORY "${PROBE_DIR}"
    RESULT_VARIABLE probe_result
    OUTPUT_VARIABLE probe_output
    ERROR_VARIABLE probe_output
    ENCODING NONE
)
if(NOT probe_result EQUAL 0)
    message(FATAL_ERROR "the /showIncludes probe failed:\n${probe_output}")
endif()

string(FIND "${probe_output}" "${detected_prefix}" prefix_position)
if(prefix_position EQUAL -1)
    message(FATAL_ERROR
        "This console's code page differs from the one CMake detected the compiler on, so ninja "
        "would record no headers for what it compiles and leave objects stale after header edits.\n"
        "Build with scripts/build-and-run.ps1, or configure again from this console after deleting "
        "the directory that holds ${COMPILER_INFO}.")
endif()
