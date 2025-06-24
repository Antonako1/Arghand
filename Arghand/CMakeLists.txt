cmake_minimum_required(VERSION 3.14)
set(LIB_NAME "Arghand")
project(${LIB_NAME} VERSION 1.0.0 LANGUAGES CXX)

add_library(${LIB_NAME} SHARED "src/Arghand.cpp")

# Include directories
target_include_directories(${LIB_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)


# Set C++ standard
target_compile_features(${LIB_NAME} PUBLIC cxx_std_11)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Export package for use in other projects
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/${LIB_NAME}ConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

install(TARGETS ${LIB_NAME}
        EXPORT ${LIB_NAME}Targets
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
        INCLUDES DESTINATION include)

install(DIRECTORY include/ DESTINATION include)

install(EXPORT ${LIB_NAME}Targets
        FILE ${LIB_NAME}Targets.cmake
        NAMESPACE ${LIB_NAME}::
        DESTINATION lib/cmake/${LIB_NAME})

install(FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/${LIB_NAME}Config.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/${LIB_NAME}ConfigVersion.cmake"
    DESTINATION lib/cmake/${LIB_NAME}
)

