cmake_minimum_required(VERSION 3.14)
project(controller)


set(CMAKE_CXX_STANDARD 17)

set(
  SOURCES

  main.cpp
  SensorTCPConnection/SensorTCPConnection.cpp SensorTCPConnection/SensorTCPConnection.h SensorTCPConnection/SensorTCPConnectionFactory.cpp SensorTCPConnection/SensorTCPConnectionFactory.h SensorDataConsumer/SensorDataConsumer.cpp SensorDataConsumer/SensorDataConsumer.h SensorDataConsumer/Decision.h SensorDataConsumer/DecisionNotification.h ManipulatorCommunicator/ManipulatorCommunicator.cpp ManipulatorCommunicator/ManipulatorCommunicator.h common/DeathPillNotification.h HTTPSAPI/APIFactory.cpp HTTPSAPI/APIFactory.h HTTPSAPI/DecisionHandler.cpp HTTPSAPI/DecisionHandler.h)

add_executable(
  controller

  ${SOURCES}
)


target_link_libraries(controller atomic)
target_link_libraries(controller pthread)

# Add POCO
# POCO_PREFIX - path to built POCO library
# POCO_INCLUDE_DIR - path to POCO's header files
# POCO_LIB_DIR - path to POCO's built libraries
# POCO_LIBS - pathes to libraries
if(NOT DEFINED POCO_INCLUDE_DIR)
  if(DEFINED POCO_PREFIX)
    set(POCO_INCLUDE_DIR "${POCO_PREFIX}/include")
  else()
    message(
      FATAL_ERROR
      "Neither POCO_INCLUDE_DIR nor POCO_PREFIX variables are defined. The POCO dependency is not satisfied."
    )
  endif()
endif()

if(NOT DEFINED POCO_LIB_DIR)
  if(DEFINED POCO_PREFIX)
    set(POCO_LIB_DIR "${POCO_PREFIX}/lib")
  else()
    message(
      FATAL_ERROR
      "Neither POCO_LIB_DIR nor POCO_PREFIX variables are defined. The POCO dependency is not satisfied."
    )
  endif()
endif()

if(NOT DEFINED POCO_LIBS)
  set(
    POCO_LIBS

    "${POCO_LIB_DIR}/libPocoNet.so"
    "${POCO_LIB_DIR}/libPocoUtil.so"
    "${POCO_LIB_DIR}/libPocoJSON.so"
    "${POCO_LIB_DIR}/libPocoXML.so"
    "${POCO_LIB_DIR}/libPocoFoundation.so"
  )
endif()

# set the include path for the app
target_include_directories(controller PRIVATE "${POCO_INCLUDE_DIR}")
# link the app against POCO
target_link_libraries(controller "${POCO_LIBS}")


# Add cereal to include directories
include_directories(./cereal/include)

# Add Boost
## see https://cmake.org/cmake/help/latest/module/FindBoost.html
#find_package(Boost REQUIRED COMPONENTS system)
#
#include_directories(${Boost_INCLUDE_DIR})
## Note: a target should be already defined using 'add_executable' or 'add_library'
#target_link_libraries(controller ${Boost_LIBRARIES})
