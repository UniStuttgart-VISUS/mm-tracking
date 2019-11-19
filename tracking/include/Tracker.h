/**
 * Tracker.h
 *
 * Copyright (C) 2014-2018 by VISUS (Universitaet Stuttgart)
 * Alle Rechte vorbehalten.
 */

#ifndef TRACKING_TRACKER_H_INCLUDED
#define TRACKING_TRACKER_H_INCLUDED

#ifdef TRACKING_EXPORTS  
#define TRACKING_API __declspec(dllexport)   
#else  
#define TRACKING_API __declspec(dllimport)   
#endif  

#include "stdafx.h"
#include "VrpnButtonDevice.h"
#include "NatNetDevicePool.h"


namespace tracking {

    /***************************************************************************
    *
    * Collects 6 DOF tracking data of rigid bodies (via NetNet) and 
    * the states of VRPN button devices.
    *
    ***************************************************************************/
    class TRACKING_API Tracker {

    public:

        /** Data structure for setting parameters as batch. */
        struct Params {
            std::string                                                   activeNode;      /** The name of the active node which should receive the tracking data exclusively. */
            std::vector<tracking::VrpnDevice<vrpn_Button_Remote>::Params> vrpn_params;
            tracking::NatNetDevicePool::Params                            natnet_params;
        };

        /** Current tracking raw data. */
        struct TrackingData  {
            tracking::NatNetDevicePool::RigidBodyData rigidBody;
            tracking::ButtonMask                      buttonState;
        };

        ///////////////////////////////////////////////////////////////////////

        /**
        * CTOR
        */
        Tracker(const tracking::Tracker::Params& inParams);

        /**
        * DTOR
        */
        ~Tracker(void);

        /**
        * Callback for connection tracking.
        *
        * @return True for success, false otherwise.
        */
        bool Connect(void);

        /**
        * Callback for disconnection tracking.
        *   
        * @return True for success, false otherwise.
        */
        bool Disconnect(void);

        /**********************************************************************/
        // GET

        /**
        *  Get current tracking data.
        *
        * @param buttonDevice The name of the button device getting data for.
        * @param rigidBody    The name of the rigid body getting data for
        * @param data         Returns the current tracking raw data.
        *
        * @return True for success, false otherwise.
        */
        bool GetData(std::string& rigidBody, std::string& buttonDevice, tracking::Tracker::TrackingData& data);

        /**
        * Get all available rigid body names.
        *
        * @return All available rigid body names.
        */
        void GetRigidBodyNames(std::vector<std::string>& inoutNames) const;

        /**********************************************************************/
        // SET

        inline void SetActiveNode(std::string an) {
            this->activeNode = an;
        }

    private:

        /**********************************************************************
        * types
        **********************************************************************/

        typedef std::vector<std::unique_ptr<tracking::VrpnButtonDevice>> VrpnButtonPoolType;

        /**********************************************************************
        * variables
        **********************************************************************/

        /** The VRPN devices that handle button presses. */
        VrpnButtonPoolType buttonDevices;

        /** The NatNat motion devices that handle position and orientation of rigid bodies. */
        tracking::NatNetDevicePool motionDevices;

        /** Connection status. */
        bool isConnected;

        /** parameters ********************************************************/

        /** Enables the tracker only on the node with the specified name. */
        std::string activeNode;

        /**********************************************************************
        * functions
        **********************************************************************/

        /** Print used parameter values. */
        void paramsPrint(void);

        /** Print used parameter values. */
        bool paramsCheck(void);

    };

} /** end namespace tracking */

#endif /** TRACKING_TRACKER_H_INCLUDED */