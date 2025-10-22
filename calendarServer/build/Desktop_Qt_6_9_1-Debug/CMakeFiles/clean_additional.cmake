# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles/calendarServer_autogen.dir/AutogenUsed.txt"
  "CMakeFiles/calendarServer_autogen.dir/ParseCache.txt"
  "calendarServer_autogen"
  )
endif()
