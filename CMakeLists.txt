cmake_minimum_required(VERSION 3.14)
project(sensors_controller_simulation)

set(CMAKE_CXX_STANDARD 17)

set(POCO_PREFIX      "/usr/local")
set(POCO_INCLUDE_DIR "${POCO_PREFIX}/include")
set(POCO_LIB_DIR     "${POCO_PREFIX}/lib")
set(POCO_LIBS        "${POCO_LIB_DIR}/libPocoNet.so"
                     "${POCO_LIB_DIR}/libPocoUtil.so"
                     "${POCO_LIB_DIR}/libPocoJSON.so"
                     "${POCO_LIB_DIR}/libPocoXML.so"
                     "${POCO_LIB_DIR}/libPocoFoundation.so"
)

add_subdirectory(controller)
add_subdirectory(sensor)
