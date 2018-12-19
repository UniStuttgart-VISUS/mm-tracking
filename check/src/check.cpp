// check.cpp 
// Defines the entry point for the console application.  


#include <iomanip>
#include <iostream>

//#ifdef HAVE_TRACKING
    #include "TrackingUtilizer.h"
//#endif


using namespace std;
using namespace tracking;


/**** HOWTO: ******************************************************************
*
* 1) Start 'Motive' software on NatNet server 'mini'
* 2) Start VRPN server on VRPN server 'mini' -> C:\vrpn\start64.bat
* 3) Start test program: ".\check.exe"
*
******************************************************************************/

int main() {

    std::string progName = "check";

    // PARAMETERS for Tracker and TrackingUtilizer /////////////////////////////

    /// Tracker
    tracking::Tracker::Params tp;
    tp.activeNode                   = ""; // = all
    /// NatNet
    tp.natnet_params.clientIP       = "129.69.205.205"; // >>> musette (braunms) => CHANGE!
    tp.natnet_params.serverIP       = "129.69.205.86";
    tp.natnet_params.cmdPort        = 1510;
    tp.natnet_params.dataPort       = 1511;
    tp.natnet_params.conType        = tracking::NatNetDevicePool::ConnectionType::UniCast;
    tp.natnet_params.verboseClient  = true;
    /// VRPN button devices
    tp.vrpn_params.clear();
    //tracking::VrpnDevice<vrpn_Button_Remote>::Params btnDeviceParams;
    //btnDeviceParams.deviceName      = "ControlBox";
    //btnDeviceParams.serverName      = "mini";
    //btnDeviceParams.port            = 3884;
    //btnDeviceParams.protocol        = tracking::VrpnDevice<vrpn_Button_Remote>::Protocols::VRPN_TCP;
    //tp.vrpn_params.push_back(btnDeviceParams);

    /// TrackingUtilizer
    tracking::TrackingUtilizer::Params tup;
    tup.buttonDeviceName            = ""; // e.g. "ControlBox";
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
    tup.fovMode                     = tracking::TrackingUtilizer::FovMode::WIDTH_AND_ASPECT_RATIO;
    tup.fovHeight                   = 0.2f;
    tup.fovWidth                    = 0.2f;
    tup.fovHoriAngle                = 60.0f;
    tup.fovVertAngle                = 30.0f;
    tup.fovAspectRatio              = tracking::TrackingUtilizer::FovAspectRatio::AR_1_77__1; // 16:9


    // TRACKING INITIALISATION ////////////////////////////////////////////////

    // Tracker
    std::shared_ptr<tracking::Tracker> tracker = std::make_shared<tracking::Tracker>(tp);
    if (!tracker->Connect()) {
        std::cerr << std::endl << "[ERROR] <" << progName.c_str() << "> Failed to establish tracker connection." << std::endl << std::endl;
        return 0;
    }

    std::vector<std::string> rigidBodies;
    rigidBodies.clear();
    tracker->GetRigidBodyNames(rigidBodies);

    // TrackingUtilizer
    std::vector<tracking::TrackingUtilizer> trackingUtilizers;
    trackingUtilizers.clear();
    for (auto it : rigidBodies) {
        tup.rigidBodyName = it;
        trackingUtilizers.push_back(tracking::TrackingUtilizer(tup, tracker));
    }


    // LOOP ///////////////////////////////////////////////////////////////////

    tracking::Rectangle fov;
    bool exit = false;
    while (!exit) {

        // Get current tracking data
        for (auto& tu : trackingUtilizers) {

            bool stateFov = tu.GetFieldOfView(fov);

            std::cout << std::fixed << std::setprecision(4) <<
                "[info] <" << progName.c_str() << "> RIGID-BODY \"" << tu.GetRigidBodyName().c_str()  << "\" - FIELD OF VIEW (valid = "
                << ((stateFov) ? ("TRUE") : ("FALSE")) << ") " <<
                " - Coordinates: LEFT_TOP (" <<
                fov.left_top.X() << "," << fov.left_top.Y() << ") | LEFT_BOTTOM (" <<
                fov.left_bottom.X() << "," << fov.left_bottom.Y() << ") | RIGHT_TOP (" <<
                fov.right_top.X() << "," << fov.right_top.Y() << ") | RIGHT_BOTTOM (" <<
                fov.right_bottom.X() << "," << fov.right_bottom.Y() << ") " 
                << std::endl;
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
