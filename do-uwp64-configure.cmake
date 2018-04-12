execute_process(
    COMMAND ${CMAKE_COMMAND}
    -DCMAKE_SYSTEM_NAME=WindowsStore
    -DCMAKE_SYSTEM_VERSION=10.0
    -G "Visual Studio 15 2017 Win64"
    ${CMAKE_CURRENT_LIST_DIR})
