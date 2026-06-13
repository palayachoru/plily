message("-- Processing: ${CMAKE_CURRENT_LIST_FILE}")

# Generate an object file (util.c -> util.o)
add_library(util_obj OBJECT util.c)

# Locate the header files
target_include_directories(util_obj PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Register this object & header file to global pool
set_property(GLOBAL APPEND PROPERTY PLILY_OBJECTS $<TARGET_OBJECTS:util_obj>)
set_property(GLOBAL APPEND PROPERTY PLILY_HEADERS ${CMAKE_CURRENT_SOURCE_DIR})
