/**
* VrpnButtonDevice.h
*
* Copyright (C) 2018 by VISUS (Universitaet Stuttgart)
* Alle Rechte vorbehalten.
*/

#ifndef TRACKING_VRPNBUTTONDEVICE_H_INCLUDED
#define TRACKING_VRPNBUTTONDEVICE_H_INCLUDED

#include "stdafx.h"
#include "VrpnDevice.h"


namespace tracking {

    /***************************************************************************
    *
    * VRPN button device.
    *
    ***************************************************************************/
    class VrpnButtonDevice : public tracking::VrpnDevice<vrpn_Button_Remote> {

    public:

        /**
        * CTOR
        */
        VrpnButtonDevice(void);

        /**
        * DTOR
        */
        ~VrpnButtonDevice(void);

        /**
        * Initialisation.
        *
        * @return True for success, false otherwise.
        */
        bool Initialise(const tracking::VrpnDevice<vrpn_Button_Remote>::Params& params);

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

        /**********************************************************************/
        // GET

        /**
        * Get current button states.
        *
        * @return The current button states.
        */
        tracking::Button GetButton(void) const;

    private:

        /***********************************************************************
        * variables
        **********************************************************************/

        bool initialised;
        bool connected;

        std::atomic<bool> runThreadLoop;

        std::atomic<tracking::Button> button;

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
