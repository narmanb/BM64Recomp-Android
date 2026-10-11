find_package(Python3 REQUIRED COMPONENTS Interpreter)
execute_process(COMMAND "${Python3_EXECUTABLE}" "${CMAKE_SOURCE_DIR}/tools/apply_android_patches.py"
    RESULT_VARIABLE patch_result)
if(NOT patch_result EQUAL 0)
    message(FATAL_ERROR "Android dependency patch failed; see tools/apply_android_patches.py")
endif()
if(NOT EXISTS "${BM64_FILE_TO_C}")
    message(FATAL_ERROR "Missing host file_to_c. Run tools/build_host_tools.sh or compile lib/rt64/src/tools/file_to_c/file_to_c.cpp to build-host-tools/file_to_c")
endif()
set(RT64_FILE_TO_C "${BM64_FILE_TO_C}" CACHE FILEPATH "Host file embedding tool" FORCE)
add_executable(file_to_c IMPORTED GLOBAL)
set_target_properties(file_to_c PROPERTIES IMPORTED_LOCATION "${BM64_FILE_TO_C}")
include(FetchContent)
set(SDL_SHARED ON CACHE BOOL "" FORCE)
set(SDL_STATIC OFF CACHE BOOL "" FORCE)
set(SDL_TEST OFF CACHE BOOL "" FORCE)
FetchContent_Declare(SDL2 GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG 98d1f3a45aae568ccd6ed5fec179330f47d4d356)
FetchContent_MakeAvailable(SDL2)
set(FT_DISABLE_ZLIB ON CACHE BOOL "" FORCE)
set(FT_DISABLE_BZIP2 ON CACHE BOOL "" FORCE)
set(FT_DISABLE_PNG ON CACHE BOOL "" FORCE)
set(FT_DISABLE_HARFBUZZ ON CACHE BOOL "" FORCE)
set(FT_DISABLE_BROTLI ON CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF)
FetchContent_Declare(freetype GIT_REPOSITORY https://github.com/freetype/freetype.git
    GIT_TAG 534ad3456055ee1f65ecde3bcf22a656a31514d1)
FetchContent_MakeAvailable(freetype)
# FindFreetype treats FREETYPE_LIBRARY as a filename, not a CMake target. Give
# RmlUi the actual build target so its imported dependency cannot become the
# literal linker argument "freetype".
if(NOT TARGET Freetype::Freetype)
    add_library(Freetype::Freetype ALIAS freetype)
endif()
# RmlUi's font engine finds the already cross-compiled FreeType target.
set(FREETYPE_LIBRARY freetype CACHE STRING "" FORCE)
set(FREETYPE_LIBRARIES freetype CACHE STRING "" FORCE)
set(FREETYPE_INCLUDE_DIRS "${freetype_SOURCE_DIR}/include" CACHE STRING "" FORCE)
set(FREETYPE_INCLUDE_DIR_freetype2 "${freetype_SOURCE_DIR}/include" CACHE PATH "" FORCE)
set(FREETYPE_INCLUDE_DIR_ft2build "${freetype_SOURCE_DIR}/include" CACHE PATH "" FORCE)

set(FILE_TO_C "${BM64_FILE_TO_C}")
set(FILE_TO_C_DEP "${BM64_FILE_TO_C}")
