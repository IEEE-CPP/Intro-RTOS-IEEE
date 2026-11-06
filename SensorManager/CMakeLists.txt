target_sources(app PRIVATE
    src/main.cpp
    src/SensorManager.cpp
)

target_include_directories(app 
    PRIVATE
    include
)

target_link_libraries(app PRIVATE
    pw_sync.mutex
    pw_thread.thread
    pw_thread.sleep
    pw_chrono.system_clock
)