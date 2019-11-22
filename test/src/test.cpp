﻿// test.cpp 
// Defines the entry point for the console application.  


#include <iomanip>
#include <iostream>

#include "TrackingUtilizer.h"


using namespace std;


/**** HOWTO: ******************************************************************
*
* 1) Start 'Motive' software on NatNet server 'mini'
* 2) Start VRPN server on VRPN server 'mini' -> C:\vrpn\start64.bat
* 3) Start test program: ".\test.exe"
*
******************************************************************************/

int main() {

    // PARAMETERS for Tracker and TrackingUtilizer /////////////////////////////

    /// Tracker Parameters
    tracking::Tracker::Params tp;
    std::string active_node(""); // Allowing all clients to access tracking data
    tp.active_node                   = active_node.c_str();
    tp.active_node_len               = active_node.length();
    std::string client_ip("129.69.205.123"); // TODO: Change to appropriate Client IP (.76 = MINYOU)
    tp.natnet_params.client_ip       = client_ip.c_str();
    tp.natnet_params.client_ip_len   = client_ip.length();
    std::string server_ip("129.69.205.86"); // (.86 = MINI)
    tp.natnet_params.server_ip       = server_ip.c_str();
    tp.natnet_params.server_ip_len   = server_ip.length();
    tp.natnet_params.cmd_port        = 1510;
    tp.natnet_params.data_port       = 1511;
    tp.natnet_params.con_type        = tracking::NatNetDevicePool::ConnectionType::UniCast;
    tp.natnet_params.verbose_client  = false;

    tracking::VrpnDevice<vrpn_Button_Remote>::Params bp;
    std::vector<tracking::VrpnDevice<vrpn_Button_Remote>::Params> bps;
    std::string device_name("ControlBox");
    bp.device_name                   = device_name.c_str();
    bp.device_name_len               = device_name.length();
    std::string server_name("mini");
    bp.server_name                   = server_name.c_str();
    bp.server_name_len               = server_name.length();
    bp.port                          = 3884;
    bp.protocol                      = tracking::VrpnDevice<vrpn_Button_Remote>::Protocols::VRPN_TCP;
    bps.emplace_back(bp);
    tp.vrpn_params                   = bps.data();
    tp.vrpn_params_count             = bps.size();

    /// TrackingUtilizer Parameters
    tracking::TrackingUtilizer::Params tup; 
    device_name                      = bp.device_name; // TODO: Insert bp.device_name only for appropriate rigid body. 
    tup.btn_device_name              = device_name.c_str(); 
    tup.btn_device_name_len          = device_name.length();
    std::string rigid_body_name("");
    tup.rigid_body_name              = rigid_body_name.c_str(); // (Will be set in line 108 depending on what rigid bodies are available.)
    tup.rigid_body_name_len          = rigid_body_name.length();
    tup.select_btn                   = 0;
    tup.rotate_btn                   = 1;
    tup.translate_btn                = 2;
    tup.zoom_btn                     = 3;
    tup.invert_rotate                = true;
    tup.invert_translate             = true;
    tup.invert_zoom                  = true;
    tup.translate_speed              = 10.0f;
    tup.zoom_speed                   = 20.0f;
    tup.single_interaction           = false;
    tup.fov_mode                     = tracking::TrackingUtilizer::FovMode::WIDTH_AND_ASPECT_RATIO;
    tup.fov_height                   = 0.2f;
    tup.fov_width                    = 0.2f;
    tup.fov_horiz_angle              = 60.0f;
    tup.fov_vert_angle               = 30.0f;
    tup.fov_aspect_ratio             = tracking::TrackingUtilizer::FovAspectRatio::AR_1_77__1; // 16:9

    /// Powerwall Resolution: 10.800 x 4.096 pixel | Aspect Ratio: 2,64

    // TRACKING INITIALISATION ////////////////////////////////////////////////
    
    // --- Tracker ---
    // Handles all communication with NatNet and VRPN.
    // Creating one (!) Tracker which runs in separate thread.
    auto tracker = std::make_shared<tracking::Tracker>();
    if (!tracker->Initialise(tp)) {
        std::cerr << std::endl << "[ERROR] [test] Unable to initialise <Tracker>. " <<  "[" << __FILE__ << ", " << __FUNCTION__ << ", line " << __LINE__ << "]" << std::endl << std::endl;
        return 1;
    }
    if (!tracker->Connect()) {
        std::cerr << std::endl << "[ERROR] [test] Failed to establish <Tracker> connection. " << "[" << __FILE__ << ", " << __FUNCTION__ << ", line " << __LINE__ << "]" << std::endl << std::endl;
        return 1;
    }

    // --- TrackingUtilizers ---
    // Manage tracking data for one rigid body each.
    // Creating a TrackingUtilizer for each found rigid body (and corresponding button device).
    // Initialising TrackingUtilizer with exisiting Tracker.
    size_t rigiBodyCount = tracker->GetRigidBodyCount();
    std::vector<std::string> rigidBodies;
    for (size_t i = 0; i < rigiBodyCount; i++) {
        rigidBodies.emplace_back(std::string(tracker->GetRigidBodyName(i)));
    }
    std::vector<tracking::TrackingUtilizer> utilizers;
    utilizers.clear();
    for (auto rb : rigidBodies) {
        tup.rigid_body_name = rb.c_str();
        tup.rigid_body_name_len = rb.length();
        utilizers.emplace_back(tracking::TrackingUtilizer());
        if (!utilizers.back().Initialise(tup, tracker)) {
            std::cerr << std::endl << "[ERROR] [test] Failed to initialise <TrackingUtilizer>. " << "[" << __FILE__ << ", " << __FUNCTION__ << ", line " << __LINE__ << "]" << std::endl << std::endl;
            return 1;

        }
    }

    // LOOP ///////////////////////////////////////////////////////////////////

    unsigned int btn;
    float pos_x, pos_y, pos_z;
    float orient_x, orient_y, orient_z, orient_w;
    float inters_x, inters_y;
    tracking::Rectangle fov;
    bool state;

    bool exit = false;
    while (!exit) {

        // Get current tracking data for each rigid body managed by the TrackingUtilizers.
        for (auto& tu : utilizers) {

            // Button State
            state = tu.GetRawData(btn, pos_x, pos_y, pos_z, orient_x, orient_y, orient_z, orient_w);
            std::cout << std::fixed << std::setprecision(4) <<
                "[INFO] [test] BUTTON-DEVICE \"" << tu.GetButtonDeviceName() << "\" - STATE (valid = "
                << ((state) ? ("TRUE") : ("FALSE")) << ") ";
            if (state) {
                std::cout << " - " << btn;
            }
            std::cout << std::endl;

            // Intersection
            state = tu.GetIntersection(inters_x, inters_y);
            std::cout << std::fixed << std::setprecision(4) <<
                "[INFO] [test] RIGID-BODY \"" << tu.GetRigidBodyName() << "\" - INTERSECTION (valid = "
                << ((state) ? ("TRUE") : ("FALSE")) << ") ";
            if (state) {
                std::cout << " - Coordinates: (" << inters_x << "," << inters_y << ") ";
            }
            std::cout << std::endl;

        }
        std::cout << std::endl;

        // Wait for user input to end loop
        std::cout << "[Press 'ESC' to exit program]" << std::endl << std::endl;
        int key = 0x1B; // ESC
        if (GetAsyncKeyState(key)) {
            exit = true;
        }

        // Sleep
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    return 0;
}
