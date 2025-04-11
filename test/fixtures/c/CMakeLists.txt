cmake_minimum_required(VERSION 3.31)

find_package(cmake-napi REQUIRED PATHS node_modules/cmake-napi)

project(bare_addon C)

add_napi_module(addon)

target_sources(
  ${addon}
  PRIVATE
    binding.c
)

target_link_libraries(
  ${addon}
  PRIVATE
    bare_compat_napi
)
