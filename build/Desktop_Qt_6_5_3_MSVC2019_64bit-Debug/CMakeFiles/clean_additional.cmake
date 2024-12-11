# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\Snack_Guard_rev_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\Snack_Guard_rev_autogen.dir\\ParseCache.txt"
  "Snack_Guard_rev_autogen"
  )
endif()
