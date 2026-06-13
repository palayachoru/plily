cmake_minimum_required(VERSION 4.0)

set(CMAKE_C_COMPILER "clang")         # Set the compiler to use
set(CMAKE_C_STANDARD 11)              # Set the c standard
set(CMAKE_C_STANDARD_REQUIRED ON)     # Strict adherence to chosen standard
set(CMAKE_C_EXTENSIONS OFF)           # Stick strictly to ISO C rules

project(PEACELILY LANGUAGES C)

# Force cmake to generate compile_commands.json (contain entire compilation stages)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Check for CUnit testing library installation
find_library(CUNIT_LIB cunit)
if(NOT CUNIT_LIB)
  message(FATAL_ERROR "CUnit library not found! Please install it.")
endif()

# Specify global compiler flags
add_compile_options(
  -Wall               # Enable all standard warning
  -Wextra             # Enable extra useful warning
  -Wpedantic          # Warn if C standard is not followed
  -Wshadow            # Warn about variable shadowing
  -Wpointer-arith     # Warn about invalid pointer arithmetic
  -Wundef             # Warn if undefined identifier is evaluated
  -Wcast-align        # Warn if any unsafe pointer casting happens
  -Wstrict-prototypes # Warn if function declared without specifying arg types
  -Wconversion        # Warn if implicite convertion will lose pricision
  -Wunreachable-code  # Warn dead code
  -Werror             # All warnings will be treated as error

  -funsigned-char                 # Treat char as unsigned char
  -fno-strict-overflow            # Detect integer overflow
  -fstack-protector-strong        # Enable Stack based buffer overflow checks at Runtime
)

# Initialize a global variable to collect all object files & header files
set_property(GLOBAL PROPERTY PLILY_OBJECTS "")
set_property(GLOBAL PROPERTY PLILY_HEADERS "")

# Enter each sub directories to make object targets
add_subdirectory(src/util)
add_subdirectory(src/list)

# Pull all the registered object & header files
get_property(COLLECTED_OBJECTS GLOBAL PROPERTY PLILY_OBJECTS)
get_property(COLLECTED_HEADERS GLOBAL PROPERTY PLILY_HEADERS)

# Bundle all compiled object files into a single library
add_library(plily STATIC ${COLLECTED_OBJECTS})
target_include_directories(plily PUBLIC ${COLLECTED_HEADERS})

enable_testing()
add_subdirectory(test)


# Package the library and the associated headers to dist folder
# While running cmake: cmake --install build --prefix ./dist
# this will create ./dist/lib & ./dist/include/
install(TARGETS plily DESTINATION lib)

# Loop through each headers path and pick the .h files to install location
foreach (HEADER_DIR IN LISTS COLLECTED_HEADERS)
  install(DIRECTORY "${HEADER_DIR}/"
          DESTINATION include
          FILES_MATCHING PATTERN "*.h")
endforeach()
