target_sources(app PRIVATE src/SharedSensorData.cpp)
target_include_directories(app PRIVATE include)
target_link_libraries(app PRIVATE pw_sync.mutex)
