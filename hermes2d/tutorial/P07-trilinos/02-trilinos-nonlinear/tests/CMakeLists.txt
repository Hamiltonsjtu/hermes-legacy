if (NOT H2D_REAL)
    return()
endif (NOT H2D_REAL)

if (WITH_TRILINOS)

project(test-tutorial-P09-02-trilinos-nonlinear)

add_executable(${PROJECT_NAME} main.cpp ../definitions.cpp)
include (${hermes2d_SOURCE_DIR}/CMake.common)
set_common_target_properties(${PROJECT_NAME})
set(BIN ${PROJECT_BINARY_DIR}/${PROJECT_NAME})
add_test(test-tutorial-P09-02-trilinos-nonlinear ${BIN})

endif (WITH_TRILINOS)

