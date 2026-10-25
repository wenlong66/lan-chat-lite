cmake_minimum_required(VERSION 3.16)

project(lan-chat-lite VERSION 0.1.0 LANGUAGES CXX)

# Override the display version with -DLMC_VERSION for tagged builds.
set(LMC_VERSION "0.1.0-alpha" CACHE STRING "Application display version")
message(STATUS "LMC version: ${LMC_VERSION}")

option(LMC_LITE_BUILD "Use the minimal LanChat Lite interface" ON)
option(LMC_ENABLE_SANITIZERS "Enable address/undefined sanitizers for development" OFF)
if(LMC_ENABLE_SANITIZERS AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-fsanitize=address,undefined -fno-omit-frame-pointer)
    add_link_options(-fsanitize=address,undefined)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Export compile_commands.json for clangd / IDEs.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

if(WIN32)
    enable_language(RC)
    # win32-msvc mkspec defines UNICODE/_UNICODE (TCHAR is wchar_t); the code
    # relies on it (qtlockedfile_win.cpp, shared.cpp getLogonName).
    add_compile_definitions(UNICODE _UNICODE)
endif()

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

add_subdirectory(lmcapp)
add_subdirectory(lmc)

include(CTest)
if(BUILD_TESTING)
    add_subdirectory(tests)
endif()

# The legacy lmm test daemon is excluded unless explicitly enabled.
option(LMC_BUILD_LMM "Build the upstream lmm test daemon (legacy file protocol)" OFF)
if(LMC_BUILD_LMM)
    add_subdirectory(tools/lmm)
endif()

# In-process GUI test agent (the lan-messenger-test binary).
# Test tooling lives in tools/ next to the lmm mock daemon; tests/ holds
# only the suites themselves. See tools/lmc-agent/PROTOCOL.md.
option(LMC_BUILD_TEST_AGENT "Build the in-process GUI test agent (development only)" OFF)
if(LMC_BUILD_TEST_AGENT)
    add_subdirectory(tools/lmc-agent)
endif()
