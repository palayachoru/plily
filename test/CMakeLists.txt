message("-- Processing: ${CMAKE_CURRENT_LIST_FILE}")

# Gather all the source test files
file(GLOB TEST_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.c")

# Loop through each test file and generate an executable for each one
foreach(test_file ${TEST_SOURCES})
  # Extract filename without path and extension
  get_filename_component(test_name ${test_file} NAME_WE)

  # Create executable
  add_executable(${test_name} ${test_file})

  # Link the executable with our library
  target_link_libraries(${test_name} PRIVATE plily cunit)

  # Register the executable with CTest execution engine
  add_test(NAME ${test_name} COMMAND ${test_name})

  # DEBUG: print upon successful registration of test
  message(STATUS "Test Registered: ${test_name}")

endforeach()


# to run the registered commands,
# cd build
# ctest --output-on-failure
