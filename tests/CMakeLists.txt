add_library(caffeine-obs-plugin-tests OBJECT test-main.cpp)

set(TEST_SOURCES test-main.cpp test-helper.hpp test-policy.hpp)

add_executable(caffeine-tests ${TEST_SOURCES})

# Link only the files we need
target_sources(
  caffeine-tests PRIVATE ${PROJECT_SOURCE_DIR}/src/policy.hpp ${PROJECT_SOURCE_DIR}/src/policy.cpp
                         ${PROJECT_SOURCE_DIR}/src/utils.hpp ${PROJECT_SOURCE_DIR}/src/caffeine.hpp)
