# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles/calendarClient_autogen.dir/AutogenUsed.txt"
  "CMakeFiles/calendarClient_autogen.dir/ParseCache.txt"
  "calendarClient_autogen"
  )
endif()
