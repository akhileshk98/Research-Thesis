#pragma once

#include <chrono>
#include <thread>
#include <deque>
#include <mutex>
#include <condition_variable>
#include "IWR6843.h"
#include <memory>


/**
 * @file iwr6843_controller.hpp
 * @brief Header file for the IWR6843 Radar Controller.
 * @details For more Information look into @c Iwr6843Controller Class description.
 *
*/


/**
 * @class Iwr6843Controller
 * @brief Controls data acquisition from a TI IWR6843 radar sensor.
 *
 * @details This class wraps an underlying @c IWR6843 driver and provides a
 * ROS 2–friendly interface for working with radar point clouds. It is
 * responsible for:
 *
 * - configuring and initializing the radar via UART (cfg + data ports and
 *   a radar configuration file),
 * - starting a background polling thread that continuously fetches new frames
 *   from the sensor,
 * - decoding detected points (x, y, z, doppler) into internal @c Packet
 *   structures with host-side timestamps,
 * - buffering packets in a time-ordered queue for later consumption,
 * - assembling points from a given time window into a
 *   @c sensor_msgs::msg::PointCloud2 message.
 *
 * Typical usage is:
 * - call @c initialize_and_capture_Data() once to set up the sensor and start
 *   polling,
 * - periodically call @c read_pointClouds() with a time window and reference
 *   timestamp to obtain radar point clouds,
 * - call @c stop_publishing() during shutdown to stop the polling thread and
 *   cleanly release resources.
 *
 * The controller is intended to be owned by a higher-level node that handles
 * lifecycle management, logging and optional recording of the produced clouds.
 */

class Iwr6843Controller {
public:



/**
 * @brief Construct an Iwr6843Controller with initial UART ports and cfg path.
 *
 * Initializes @c uart_port_cfg, @c uart_port_data, and @c cfg_file_path_.
 *
 * @param cfg_port UART port for configuration commands.
 * @param data_port UART port for point-cloud data.
 * @param cfg_file Filesystem path to the radar cfg file.
 */
  Iwr6843Controller(
    const std::string& cfg_port,
    const std::string& data_port,
    const std::string& cfg_file);  




    /**
 * @brief Destructor: stop background publishing/capture thread.
 */
  ~Iwr6843Controller();



  bool read_pointClouds();
  


  bool initialize_and_capture_Data();



private:
  std::string uart_port_cfg;
  std::string uart_port_data ;
  std::string cfg_file_path_ ;

  std::unique_ptr<IWR6843> sensor_ptr;




  std::thread polling_thread_;
  bool running;
 


  struct RadarPoint {
  float x, y, z, doppler;
  };




  struct Packet {
    std::vector<RadarPoint> points;
    std::chrono::steady_clock::time_point ts;
  };



  void pollingLoop();



};




