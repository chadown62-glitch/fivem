cmake_minimum_required(VERSION 3.22)
project(fivem_internal CXX C)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded")
add_compile_options(/W4 /permissive- /MP /GS- /Zc:preprocessor)
add_compile_definitions(_CRT_SECURE_NO_WARNINGS WIN32_LEAN_AND_MEAN NOMINMAX)

# ============================================================
# payload.dll  (gets welded into ollie.exe as a resource)
# ============================================================
file(GLOB PAYLOAD_SRC
  payload/dllmain.cpp
  payload/core/*.cpp
  payload/invoker/*.cpp
  payload/hooks/*.cpp
  payload/features/*.cpp
  payload/gui/*.cpp
)
file(GLOB IMGUI_SRC
  payload/third_party/imgui/*.cpp
  payload/third_party/imgui/backends/imgui_impl_dx11.cpp
  payload/third_party/imgui/backends/imgui_impl_win32.cpp
)
file(GLOB MINHOOK_SRC
  payload/third_party/minhook/src/*.c
  payload/third_party/minhook/src/hde/*.c
)

add_library(payload SHARED
  ${PAYLOAD_SRC}
  ${IMGUI_SRC}
  ${MINHOOK_SRC}
)
target_include_directories(payload PRIVATE
  payload
  payload/third_party/imgui
  payload/third_party/imgui/backends
  payload/third_party/minhook/include
)
target_link_libraries(payload PRIVATE d3d11 dxgi d3dcompiler ntdll winmm user32 gdi32)
set_target_properties(payload PROPERTIES
  OUTPUT_NAME "ollie_payload"
  SUFFIX ".dll"
  PREFIX ""
)

# After payload builds, stage the dll next to loader/resource.rc so RC embeds it.
add_custom_command(TARGET payload POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "$<TARGET_FILE:payload>"
    "${CMAKE_CURRENT_SOURCE_DIR}/loader/ollie_payload.dll"
  COMMENT "staging payload for loader embed"
)

# ============================================================
# ollie.exe  (single-file: loader + embedded payload)
# ============================================================
add_executable(ollie
  loader/loader.cpp
  loader/resource.rc
)
target_link_libraries(ollie PRIVATE psapi shlwapi)
set_target_properties(ollie PROPERTIES OUTPUT_NAME "ollie")

# loader's resource compile must run AFTER payload is staged
add_dependencies(ollie payload)

# tell RC compiler where "ollie_payload.dll" (referenced in resource.rc) lives
set_source_files_properties(loader/resource.rc PROPERTIES
  COMPILE_FLAGS "/I\"${CMAKE_CURRENT_SOURCE_DIR}/loader\""
)

# install the lone artifact into dist/
install(TARGETS ollie RUNTIME DESTINATION dist)
