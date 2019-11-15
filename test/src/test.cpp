// test.cpp 
// Defines the entry point for the console application.  


#include <iomanip>
#include <iostream>

#include "TrackingUtilizer.h"


using namespace std;
using namespace tracking;


/**** HOWTO: ******************************************************************
*
* 1) Start 'Motive' software on NatNet server 'mini'
* 2) Start VRPN server on VRPN server 'mini' -> C:\vrpn\start64.bat
* 3) Start test program: ".\test.exe"
*
******************************************************************************/

int main() {

    std::string progName = "test";

    // PARAMETERS for Tracker and TrackingUtilizer /////////////////////////////

    /// Tracker Parameters
    Tracker::Params tp;
    tp.activeNode                   = ""; // = all
    /// NatNet
    tp.natnet_params.clientIP       = "129.69.205.123"; //TODO >>> => CHANGE!
    tp.natnet_params.serverIP       = "129.69.205.86";
    tp.natnet_params.cmdPort        = 1510;
    tp.natnet_params.dataPort       = 1511;
    tp.natnet_params.conType        = NatNetDevicePool::ConnectionType::UniCast;
    tp.natnet_params.verboseClient  = true;
    /// VRPN button devices
    tp.vrpn_params.clear();
    tracking::VrpnDevice<vrpn_Button_Remote>::Params btnDeviceParams;
    btnDeviceParams.deviceName      = "ControlBox";
    btnDeviceParams.serverName      = "mini";
    btnDeviceParams.port            = 3884;
    btnDeviceParams.protocol        = tracking::VrpnDevice<vrpn_Button_Remote>::Protocols::VRPN_TCP;
    tp.vrpn_params.push_back(btnDeviceParams);

    /// TrackingUtilizer Parameters
    TrackingUtilizer::Params tup;
    tup.buttonDeviceName            = btnDeviceParams.deviceName; /// Insert btnDeviceParams.deviceName only for appropriate rigid body.
    tup.rigidBodyName               = ""; // >>> Will be set in line 87 depending on what rigid bodies are available.
    tup.selectButton                = -1;
    tup.rotateButton                = -1;
    tup.translateButton             = 0;
    tup.zoomButton                  = 1;
    tup.invertRotate                = true;
    tup.invertTranslate             = true;
    tup.invertZoom                  = true;
    tup.translateSpeed              = 10.0f;
    tup.zoomSpeed                   = 20.0f;
    tup.singleInteraction           = false;
    tup.fovMode                     = TrackingUtilizer::FovMode::WIDTH_AND_ASPECT_RATIO;
    tup.fovHeight                   = 0.2f;
    tup.fovWidth                    = 0.2f;
    tup.fovHoriAngle                = 60.0f;
    tup.fovVertAngle                = 30.0f;
    tup.fovAspectRatio              = TrackingUtilizer::FovAspectRatio::AR_1_77__1; // 16:9


    // TRACKING INITIALISATION ////////////////////////////////////////////////

    // Tracker
    /// Handles all communication with NatNet and VRPN.
    /// Creating one (!) Tracker which runs in separate thread.
    auto tracker = std::make_shared<Tracker>(tp);
    if (!tracker->Connect()) {
        std::cerr << std::endl << "[ERROR] <" << progName.c_str() << "> Failed to establish tracker connection." << std::endl << std::endl;
        return 0;
    }

    // TrackingUtilizers
    /// Manage tracking data for one rigid body each.
    /// Creating a TrackingUtilizer for each found rigid body (and corresponding button device).
    std::vector<std::string> rigidBodies;
    rigidBodies.clear();
    tracker->GetRigidBodyNames(rigidBodies);
    std::vector<TrackingUtilizer> trackingUtilizers;
    trackingUtilizers.clear();
    for (auto it : rigidBodies) {
        tup.rigidBodyName = it;
        trackingUtilizers.push_back(TrackingUtilizer(tup, tracker));
    }


    // LOOP ///////////////////////////////////////////////////////////////////

    tracking::Point2D ist;
    tracking::Rectangle fov;
    bool state;

    bool exit = false;
    while (!exit) {

        // Get current tracking data for each rigid body managed by a TrackingUtilizer.
        for (auto& tu : trackingUtilizers) {

            // Field of View
            //state = tu.GetFieldOfView(fov);
            //std::cout << std::fixed << std::setprecision(4) <<
            //    "[info] <" << progName.c_str() << "> RIGID-BODY \"" << tu.GetRigidBodyName().c_str() << "\" - FIELD OF VIEW (valid = "
            //    << ((state) ? ("TRUE") : ("FALSE")) << ") ";
            //if (state) {
            //    std::cout << " - Coordinates: LEFT_TOP (" <<
            //        fov.left_top.X() << "," << fov.left_top.Y() << ") | LEFT_BOTTOM (" <<
            //        fov.left_bottom.X() << "," << fov.left_bottom.Y() << ") | RIGHT_TOP (" <<
            //        fov.right_top.X() << "," << fov.right_top.Y() << ") | RIGHT_BOTTOM (" <<
            //        fov.right_bottom.X() << "," << fov.right_bottom.Y() << ") ";
            //}
            //std::cout << std::endl;

            // Intersection
            state = tu.GetIntersection(ist);
            std::cout << std::fixed << std::setprecision(4) <<
                "[info] <" << progName.c_str() << "> RIGID-BODY \"" << tu.GetRigidBodyName().c_str() << "\" - INTERSECTION (valid = "
                << ((state) ? ("TRUE") : ("FALSE")) << ") ";
            if (state) {
                std::cout << " - Coordinates: (" << ist.X() << "," << ist.Y() << ") ";
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
        std::this_thread::sleep_for(std::chrono::milliseconds(250));
    }

    return 0;
}
