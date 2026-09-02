cmake_minimum_required (VERSION 3.31.3)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("v")

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB_RECURSE SOURCES "src/*.cpp")

if(CLANG)
    add_compile_options(
        "$<$<CONFIG:DEBUG>:-O0;-fstack-protector-all;-g3;-ggdb;-fPIC>"
        "$<$<CONFIG:RELEASE>:-O3;-g0;-fPIC;-DNDEBUG>"
        "-std=gnu++2a;-fmessage-length=0;-Wall;-fms-extensions;-Wno-unused-function"
    )
elseif(GCC)
    add_compile_options(
        "$<$<CONFIG:DEBUG>:-O0;-fstack-protector-all;-g3;-ggdb;-fPIC>"
        "$<$<CONFIG:RELEASE>:-O3;-g0;-fPIC;-DNDEBUG>"
        "-std=gnu++2a;-fmessage-length=0;-Wall;-Wno-unused-function"
    )
elseif(MSVC)
    add_compile_options(
        "$<$<CONFIG:DEBUG>:/Od;/GS;/ZI>"
        "$<$<CONFIG:RELEASE>:/O2;/DEBUG:NONE;-DNDEBUG>"
        "/W3;/wd4505;/wd4100"
    )
endif()

add_executable(v ${SOURCES})

set_target_properties(v PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/v/bin"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/v/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/v/lib"
) 

target_include_directories(v
	PRIVATE "src/"
)

target_link_libraries(v
    PRIVATE mist
)

target_compile_definitions(v 
    PRIVATE $<$<CONFIG:Debug>:DEBUG>
    PRIVATE $<$<PLATFORM_ID:Windows>:MIST_DLL>
)

set(ASSETS_SRC_DIR "${CMAKE_SOURCE_DIR}/v/assets")
set(ASSETS_DEST_DIR "${CMAKE_BINARY_DIR}/v/bin/assets")

add_custom_target(CopyVAssets ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${ASSETS_SRC_DIR}"
            "${ASSETS_DEST_DIR}"
    COMMENT "Copying v assets"
)

add_dependencies(v CopyVAssets)