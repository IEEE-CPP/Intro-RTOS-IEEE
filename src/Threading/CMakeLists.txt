target_sources(app PRIVATE src/PeriodicTask.cpp)
target_include_directories(app PRIVATE include)
target_link_libraries(app PRIVATE
    pw_thread.thread
    pw_thread.sleep
    pw_chrono.system_clock
    pw_function
    pw_assert.check
)
