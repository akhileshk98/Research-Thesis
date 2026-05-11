cmake_minimum_required(VERSION 3.10)
project(IWR_Only)

set(CMAKE_CXX_STANDARD 17)
set(IWR_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/third_party/IWR6843/radar_sensor)


include_directories(
  include
  ${IWR_ROOT}
  ${IWR_ROOT}/sensor_data
  ${IWR_ROOT}/sensor_data/lib
)



add_executable(IWR_Only src/main.cpp
src/iwr6843_controller.cpp
${IWR_ROOT}/IWR6843.cpp
${IWR_ROOT}/SensorData.cpp
${IWR_ROOT}/sensor_data/UARTFrame.cpp
${IWR_ROOT}/sensor_data/lib/EndianUtils.cpp
)