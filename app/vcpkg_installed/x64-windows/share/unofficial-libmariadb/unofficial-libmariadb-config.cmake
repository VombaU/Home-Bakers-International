if(NOT "ON")
  include(CMakeFindDependencyMacro)
  if("OFF")
    find_dependency(zstd CONFIG)
  endif()
endif()
include("${CMAKE_CURRENT_LIST_DIR}/unofficial-libmariadb-targets.cmake")
