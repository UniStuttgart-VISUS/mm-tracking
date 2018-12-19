/**
* VrpnButtonDevice.h
*
* Copyright (C) 2018 by VISUS (Universitaet Stuttgart)
* Alle Rechte vorbehalten.
*/

#ifndef TRACKING_VRPNBUTTONDEVICE_H_INCLUDED
#define TRACKING_VRPNBUTTONDEVICE_H_INCLUDED

#if (defined(_MSC_VER) && (_MSC_VER > 1000))
#pragma once
#endif /** (defined(_MSC_VER) && (_MSC_VER > 1000)) */


#include "stdafx.h"

#include "VrpnDevice.h"


namespace tracking {

    /***************************************************************************
    *
    * VRPN button device.
    *
    ***************************************************************************/
    class VrpnButtonDevice : public VrpnDevice<vrpn_Button_Remote> {

    public:

        /**
        * CTOR
        */
        VrpnButtonDevice(void);

        VrpnButtonDevice(VrpnDevice<vrpn_Button_Remote>::Params& inParams);

        /**
        * DTOR
        */
        ~VrpnButtonDevice(void);

        /**
        * Connect to vrpn button device.
        *
        * @return True on success, false otherwise.
        */
        bool Connect(void);

        /**
        * Disconnect from vrpn button device
        *
        * @return True on success, false otherwise.
        */
        bool Disconnect(void);

        /**
        * Get current button states.
        *
        * @return The current button states.
        */
        tracking::ButtonMask GetButtonStates(void) const;

    private:

        /***********************************************************************
        * variables
        **********************************************************************/

        /** 
        * Determines the running state of the Vrpn thread. 
        */
        std::atomic<bool> runThreadLoop;

        /** 
        * The current button states. 
        */
        std::atomic<tracking::ButtonMask> states;

        /***********************************************************************
        * functions
        **********************************************************************/

        /**
        * Vrpn device callback for button changes.
        *
        * @param userData Pinter to class, which register callback (that).
        * @param vrpnData Data struct holding current button data.
        */
        static void VRPN_CALLBACK onButtonChanged(void *userData, const vrpn_BUTTONCB vrpnData);
    };

} /** end namespace tracking */

#endif /** TRACKING_VRPNBUTTONDEVICE_H_INCLUDED */
