message("-- Processing: ${CMAKE_CURRENT_LIST_FILE}")

# Create a header-only template (shares it's location with other part of program)
# pl_common - name given to this library
# INTERFACE - specify this contain no source code only header files
add_library(pl_common INTERFACE)

target_include_directories(pl_common INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

# Register this header file to global pool
set_property(GLOBAL APPEND PROPERTY PLILY_HEADERS ${CMAKE_CURRENT_SOURCE_DIR})
