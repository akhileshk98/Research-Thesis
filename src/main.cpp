#include <iostream>
#include "iwr6843_controller.hpp"

int main() {

    Iwr6843Controller  iwrController("/dev/ttyUSB0", "/dev/ttyUSB1","/home/akhilesh/Desktop/IWR_Only/IWR_Config.cfg");
    iwrController.initialize_and_capture_Data();

    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}



