message("-- Processing: ${CMAKE_CURRENT_LIST_FILE}")

# Generate an object file (list.c -> list.o)
add_library(list_obj OBJECT list.c)

# Locate the header files
target_include_directories(list_obj PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# link it with common files
target_link_libraries(list_obj PRIVATE pl_common)

# Register this object & header file to global pool
set_property(GLOBAL APPEND PROPERTY PLILY_OBJECTS $<TARGET_OBJECTS:list_obj>)
set_property(GLOBAL APPEND PROPERTY PLILY_HEADERS ${CMAKE_CURRENT_SOURCE_DIR})
