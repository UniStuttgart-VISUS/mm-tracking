/**
 * Tracker.cpp
 *
 * Copyright (C) 2014-2018 by VISUS (Universitaet Stuttgart)
 * Alle Rechte vorbehalten.
 */


#include "Tracker.h"


/**
 * tracking::Tracker::Tracker
 */
tracking::Tracker::Tracker(void) : 
    buttonDevices(),
    motionDevices(),
    isConnected(false),
    activeNode("")
{
    this->paramsPrint();

    // Not creating any button device(s) ...
}


tracking::Tracker::Tracker(Tracker::Params& inParams) :
    buttonDevices(),
    motionDevices(inParams.natnet_params),
    isConnected(false),
    activeNode(inParams.activeNode)
{
    this->paramsPrint();

    // Create button device(s)
    this->buttonDevices.clear();
    for (int i = 0; i < inParams.vrpn_params.size(); ++i) {
        this->buttonDevices.push_back(std::make_unique<VrpnButtonDevice>(inParams.vrpn_params[i]));
    }

}


/**
* tracking::Tracker::paramsPrint
*/
void tracking::Tracker::paramsPrint(void) {
    std::cout << "[parameter] <Tracker> Active Node:                      " << ((this->activeNode.empty())?("<all>"):(this->activeNode.c_str())) << std::endl;
}

/**
* tracking::Tracker::paramsCheck
*/
bool tracking::Tracker::paramsCheck(void) {

    bool retval = true;

    /// No check necessary for:
    // this->activeNode

    return retval;
}

/**
 * tracking::Tracker::~Tracker
 */
tracking::Tracker::~Tracker(void) {

    this->Disconnect();

    for (VrpnButtonPoolType::iterator it = this->buttonDevices.begin(); it != this->buttonDevices.end(); ++it) {
        (*it).reset(nullptr);
    }
}


/**
 * tracking::Tracker::Connect
 */
bool tracking::Tracker::Connect(void) {

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
        std::cout << std::endl << "[WARNING] <Tracker> Node \"" << computerName.c_str() << "\" is not enabled to receive tracker updates (otherwise set as active node)." << std::endl << std::endl;
        return false;
    }
	
    // Connect button devices.
    bool vrpnConStatus = true;
    for (VrpnButtonPoolType::iterator it = this->buttonDevices.begin(); it != this->buttonDevices.end(); ++it) {
        if (!(*it)->Connect()) {
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

    this->isConnected = (vrpnConStatus || natnetConStatus);
    return this->isConnected;
}


/**
 * tracking::Tracker::Disconnect
 */
bool tracking::Tracker::Disconnect(void) {

    for (VrpnButtonPoolType::iterator it = this->buttonDevices.begin(); it != this->buttonDevices.end(); ++it) {
        (*it)->Disconnect();
    }
    this->motionDevices.Disconnect();

    this->isConnected = false;
    return true;
}


/**
* tracking::Tracker::GetData
*/
bool tracking::Tracker::GetData(std::string& rigidBody, std::string& buttonDevice, tracking::Tracker::TrackingData& data) {

#ifdef TRACKING_DEBUG_OUTPUT
    std::cout << "[debug] <Tracker> Requested: Button Device \"" << buttonDevice.c_str() << "\" and Rigid Body \"" << rigidBody.c_str() << "\"." << std::endl;
#endif

    bool retval = false;

    if (this->isConnected) {
        retval = true;

        // Set data of requested rigid body
        data.rigidBody.orientation = this->motionDevices.GetOrientation(rigidBody);
        data.rigidBody.position    = this->motionDevices.GetPosition(rigidBody);

        // Set data of requested button device 
        data.buttonState = 0;
        for (VrpnButtonPoolType::iterator it = this->buttonDevices.begin(); it != this->buttonDevices.end(); ++it) {
            if (buttonDevice == (*it)->GetDeviceName()) {
                data.buttonState = ((*it)->GetButtonStates());
                break; /// Break if button device is found.
            }
        }
    }

    return retval;
}


/**
* tracking::Tracker::GetRigidBodyNames
*/
void tracking::Tracker::GetRigidBodyNames(std::vector<std::string>& inoutNames) const {

    inoutNames = this->motionDevices.GetRigidBodyNames();
}