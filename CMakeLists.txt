cmake_minimum_required(VERSION 3.28.0)

set(ZEPHYR_BASE "${CMAKE_CURRENT_LIST_DIR}/lib/zephyr-workspace/zephyr" CACHE PATH "Zephyr base" FORCE)

set(Zephyr_DIR "${ZEPHYR_BASE}/share/zephyr-package/cmake" CACHE PATH "Zephyr package" FORCE)

# Zephyr discovers module CMake and Kconfig files during find_package().
list(APPEND EXTRA_ZEPHYR_MODULES "${CMAKE_CURRENT_SOURCE_DIR}/lib/pigweed")
find_package(Zephyr REQUIRED HINTS "${ZEPHYR_BASE}")

project(IEEE_Zephyr_Demo C CXX)

add_subdirectory(SensorManager)

