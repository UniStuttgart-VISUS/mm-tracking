/**
 * Tracker.cpp
 *
 * Copyright (C) 2014-2018 by VISUS (Universitaet Stuttgart)
 * Alle Rechte vorbehalten.
 */

#include "Tracker.h"


tracking::Tracker::Tracker(void)
    : initialised(false)
    , buttonDevices()
    , motionDevices()
    , isConnected(false)
    , activeNode() {

    // intentionally empty...
}


tracking::Tracker::~Tracker(void) {

    this->Disconnect();

    for (auto& v : this->buttonDevices) {
        v.reset(nullptr);
    }
}


bool tracking::Tracker::Initialise(const tracking::Tracker::Params& inParams)
{
    bool check = true;
    this->initialised = false;

    std::string active_node;
    try {
        active_node = std::string(inParams.active_node);
        if (active_node.length() != inParams.active_node_len) {
            std::cerr << std::endl << "[ERROR] [Tracker] String \"active_node\" has not expected length. " <<
                "[" << __FILE__ << ", " << __FUNCTION__ << ", line " << __LINE__ << "]" << std::endl << std::endl;
            check = false;
        }
    }
    catch (const std::exception& e) {
        std::cerr << std::endl << "[ERROR] [Tracker] Error reading string param 'active_node': " << e.what() << 
            " [" << __FILE__ << ", " << __FUNCTION__ << ", line " << __LINE__ << "]" << std::endl << std::endl;
        check = false;
    }

    this->buttonDevices.clear();
    std::vector<tracking::VrpnDevice<vrpn_Button_Remote>::Params> vrpn_params;
    try {
        for (size_t i = 0; i < inParams.vrpn_params_count; i++) {
            vrpn_params.emplace_back(inParams.vrpn_params[i]);
        }
    }
    catch (const std::exception& e) {
        std::cerr << std::endl << "[ERROR] [Tracker] Error reading 'vrpn_params' array: " << e.what() <<
            " [" << __FILE__ << ", " << __FUNCTION__ << ", line " << __LINE__ << "]" << std::endl << std::endl;
        check = false;
    }

    if (!this->motionDevices.Initialise(inParams.natnet_params)) {
        check = false;
    }

    if (check) {
        activeNode = active_node;

        for (int i = 0; i < vrpn_params.size(); ++i) {
            this->buttonDevices.emplace_back(std::make_unique<tracking::VrpnButtonDevice>());
            if (!this->buttonDevices.back()->Initialise(vrpn_params[i])) {
                check = false;
            }
        }

        this->printParams();
        this->initialised = true;
    }

    return this->initialised;
}


void tracking::Tracker::printParams(void) {
    std::cout << "[PARAMETER] [Tracker] Active Node:                      " << ((this->activeNode.empty()) ? ("<all>") : (this->activeNode.c_str())) << std::endl;
}


bool tracking::Tracker::Connect(void) {

    if (!this->initialised) {
        std::cerr << std::endl << "[ERROR] [Tracker] Not initialised. " <<
            "[" << __FILE__ << ", " << __FUNCTION__ << ", line " << __LINE__ << "]" << std::endl << std::endl;
        return false;
    }

    // Terminate previous connection.
    this->Disconnect();

    // Check active node.
    std::string computerName;

    const size_t bufSize = 255;
// Windows
#ifdef _WIN32 
    TCHAR infoBuf[bufSize];
    DWORD bufCharCount = bufSize;
    if (GetComputerName(infoBuf, &bufCharCount)) {
        computerName = infoBuf;
    }
// Linux
#else
    char hostname[bufSize];
    gethostname(hostname, bufSize);
#endif /** _WIN32 */

    if (!this->activeNode.empty() && (computerName != activeNode)) {
        std::cout << std::endl << "[WARNING] [Tracker] Node \"" << computerName.c_str() << "\" is not enabled to receive tracker updates (otherwise set as active node)." << std::endl << std::endl;
        return false;
    }
	
    // Connect button devices.
    bool vrpnConStatus = true;
    for (auto& v: this->buttonDevices) {
        if (!v->Connect()) {
            this->Disconnect();
            vrpnConStatus = false;
        }
    }

    // Connect motion devices.
    bool natnetConStatus = true;
    if (!this->motionDevices.Connect()) {
        this->Disconnect();
        natnetConStatus = false;
    }

    this->isConnected = (vrpnConStatus && natnetConStatus);
    return this->isConnected;
}


bool tracking::Tracker::Disconnect(void) {

    for (auto& v : this->buttonDevices) {
        v->Disconnect();
    }
    this->motionDevices.Disconnect();

    this->isConnected = false;
    return true;
}


bool tracking::Tracker::GetData(const std::string& inRigidBody, const std::string& inButtonDevice, tracking::Tracker::TrackingData& outData) {

    if (!this->initialised) {
        std::cerr << std::endl << "[ERROR] [Tracker] Not initialised. " <<
            "[" << __FILE__ << ", " << __FUNCTION__ << ", line " << __LINE__ << "]" << std::endl << std::endl;
        return false;
    }

#ifdef TRACKING_DEBUG_OUTPUT
    std::cout << "[DEBUG] [Tracker] Requested: Button Device \"" << buttonDevice.c_str() << "\" and Rigid Body \"" << rigidBody.c_str() << "\"." << std::endl;
#endif

    bool retval = false;

    if (this->isConnected) {
        retval = true;

        // Set data of requested rigid body
        outData.rigidBody.orientation = this->motionDevices.GetOrientation(inRigidBody);
        outData.rigidBody.position    = this->motionDevices.GetPosition(inRigidBody);

        // Set data of requested button device 
        outData.buttonState = 0;
        for (auto& v : this->buttonDevices) {
            if (inButtonDevice == v->GetDeviceName()) {
                outData.buttonState = (v->GetButtonStates());
                break; /// Break if button device is found.
            }
        }
    }

    return retval;
}


void tracking::Tracker::GetRigidBodyNames(std::vector<std::string>& inoutNames) const {

    if (!this->initialised) {
        std::cerr << std::endl << "[ERROR] [Tracker] Not initialised. " <<
            "[" << __FILE__ << ", " << __FUNCTION__ << ", line " << __LINE__ << "]" << std::endl << std::endl;
        return;
    }

    inoutNames = this->motionDevices.GetRigidBodyNames();
}
