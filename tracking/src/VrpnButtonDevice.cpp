/**
* VrpnButtonDevice.h
*
* Copyright (C) 2018 by VISUS (Universitaet Stuttgart)
* Alle Rechte vorbehalten.
*/

#include "VrpnButtonDevice.h"


tracking::VrpnButtonDevice::VrpnButtonDevice(void) : VrpnDevice<vrpn_Button_Remote>(),
    runThreadLoop(false),
    states(0)
{

}


tracking::VrpnButtonDevice::VrpnButtonDevice(VrpnDevice<vrpn_Button_Remote>::Params& inParams) : VrpnDevice<vrpn_Button_Remote>(inParams),
    runThreadLoop(false),
    states(0)
{

}


tracking::VrpnButtonDevice::~VrpnButtonDevice(void) {

    this->Disconnect();
}


bool tracking::VrpnButtonDevice::Connect(void) {

    // Establish connection.
    if (!VrpnDevice<vrpn_Button_Remote>::Connect()) { 
        return false;
    }

    // Register handle for button changes.
    this->Register<vrpn_BUTTONCHANGEHANDLER>(&VrpnButtonDevice::onButtonChanged, this);

    // Start vrpn main loop thread.
    std::cout << "[info] <VrpnButtonDevice> Starting VRPN main loop thread for \"" << this->GetDeviceName().c_str() << "\"" << std::endl;
    this->runThreadLoop.store(true);
    std::thread thread([this]() {
        while (this->runThreadLoop.load()) {
#ifdef TRACKING_DEBUG_OUTPUT
            //std::cout << "[debug] <VrpnButtonDevice> Inside VRPN main loop ..." << std::endl;
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

    return VrpnDevice<vrpn_Button_Remote>::Disconnect();
}


tracking::ButtonMask tracking::VrpnButtonDevice::GetButtonStates(void) const {

    return this->states.load();
}


void VRPN_CALLBACK tracking::VrpnButtonDevice::onButtonChanged(void *userData, const vrpn_BUTTONCB vrpnData) {

    auto that = static_cast<VrpnButtonDevice*>(userData);
    if (that == nullptr) {
        std::cerr << std::endl << "[ERROR] <VrpnButtonDevice> Invalid user data. " << __FILE__ << " " << __LINE__ << std::endl << std::endl;
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
    std::cout << "[debug] <VrpnButtonDevice> Button = " << vrpnData.button << " - State = " << ((that->states.load() & (1 << vrpnData.button)) ? (1) : (0)) << std::endl;
#endif
}
