/**
* VrpnButtonDevice.h
*
* Copyright (C) 2018 by VISUS (Universitaet Stuttgart)
* Alle Rechte vorbehalten.
*/

#include "VrpnButtonDevice.h"


tracking::VrpnButtonDevice::VrpnButtonDevice(void) : tracking::VrpnDevice<vrpn_Button_Remote>()
    , initialised(false)
    , runThreadLoop(false)
    , states(0) {

    // intentionally empty...
}



tracking::VrpnButtonDevice::~VrpnButtonDevice(void) {

    this->Disconnect();
}


bool tracking::VrpnButtonDevice::Initialise(const tracking::VrpnDevice<vrpn_Button_Remote>::Params& inParams) {

    this->initialised = tracking::VrpnDevice<vrpn_Button_Remote>::Initialise(inParams);

    return this->initialised;
}


bool tracking::VrpnButtonDevice::Connect(void) {

    if (!this->initialised) {
        std::cout << std::endl << "[ERROR] [VrpnButtonDevice] Not initialised. " <<
            "[" << __FILE__ << ", " << __FUNCTION__ << ", line " << __LINE__ << "]" << std::endl << std::endl;
        return false;
    }

    // Establish connection.
    if (!tracking::VrpnDevice<vrpn_Button_Remote>::Connect()) { 
        return false;
    }

    // Register handle for button changes.
    this->Register<vrpn_BUTTONCHANGEHANDLER>(&VrpnButtonDevice::onButtonChanged, this);

    // Start vrpn main loop thread.
    std::cout << "[INFO] [VrpnButtonDevice] Starting VRPN main loop thread for \"" << this->GetDeviceName().c_str() << "\"" << std::endl;
    this->runThreadLoop.store(true);
    std::thread thread([this]() {
        while (this->runThreadLoop.load()) {
#ifdef TRACKING_DEBUG_OUTPUT
            //std::cout << "[DEBUG] [VrpnButtonDevice] Inside VRPN main loop ..." << std::endl;
#endif
            this->MainLoop();
            std::this_thread::yield();
        }
    });
    thread.detach();

    return true;
}


bool tracking::VrpnButtonDevice::Disconnect(void) {

    // End main loop thread
    this->runThreadLoop.store(false);

    return tracking::VrpnDevice<vrpn_Button_Remote>::Disconnect();
}


tracking::ButtonMask tracking::VrpnButtonDevice::GetButtonStates(void) const {

    if (!this->initialised) {
        std::cout << std::endl << "[ERROR] [VrpnButtonDevice] Not initialised. " <<
            "[" << __FILE__ << ", " << __FUNCTION__ << ", line " << __LINE__ << "]" << std::endl << std::endl;
        return false;
    }

    return this->states.load();
}


void VRPN_CALLBACK tracking::VrpnButtonDevice::onButtonChanged(void *userData, const vrpn_BUTTONCB vrpnData) {

    auto that = static_cast<VrpnButtonDevice*>(userData);
    if (that == nullptr) {
        std::cerr << std::endl << "[ERROR] [VrpnButtonDevice] Invalid user data. " <<
            "[" << __FILE__ << ", " << __FUNCTION__ << ", line " << __LINE__ << "]" << std::endl << std::endl;
        return;
    }

    // Remember the button state.
    tracking::ButtonMask tmpStates = that->states.load();
    tracking::ButtonMask tmpMask   = 1 << vrpnData.button;
    if (vrpnData.state != 0) {
        that->states.store(tmpStates |= tmpMask);
    }
    else {
        that->states.store(tmpStates &= ~tmpMask);
    }
#ifdef TRACKING_DEBUG_OUTPUT
    std::cout << "[DEBUG] [VrpnButtonDevice] Button = " << vrpnData.button << " | State = " << ((that->states.load() & (1 << vrpnData.button)) ? (1) : (0)) << std::endl;
#endif
}
