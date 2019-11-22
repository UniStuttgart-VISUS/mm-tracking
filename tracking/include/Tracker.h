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
            const char*                                                   active_node;      /** The name of the active node which should receive the tracking data exclusively. */
            size_t                                                        active_node_len;
            tracking::VrpnDevice<vrpn_Button_Remote>::Params*             vrpn_params;
            size_t                                                        vrpn_params_count;
            tracking::NatNetDevicePool::Params                            natnet_params;
        };

        /** Current tracking raw data. */
        struct TrackingData {
            tracking::NatNetDevicePool::RigidBodyData rigid_body;
            tracking::Button                          button;
        };

        ///////////////////////////////////////////////////////////////////////

        /**
        * CTOR
        */
        Tracker(void);

        /**
        * DTOR
        */
        ~Tracker(void);

        /**
        * Initialisation.
        *
        * @return True for success, false otherwise.
        */
        bool Initialise(const tracking::Tracker::Params& params);

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
        * Get number of available rigid body names.
        *
        * @return All available rigid body names.
        */
        inline size_t GetRigidBodyCount(void) {
            return this->motionDevices.GetRigidBodyNames().size();
        }

        /**
        * Get all available rigid body names.
        *
        * @return All available rigid body names.
        */
        const char* GetRigidBodyName(size_t index) {
            if (index < this->motionDevices.GetRigidBodyNames().size()) {
                return this->motionDevices.GetRigidBodyNames()[index].c_str();
            }
            else {
                return nullptr;
            }
        }

        /**********************************************************************/
        // Only to be called by TrackingUtilizer (only use inside dll).

        /**
        *  Get current tracking data.
        *
        * @param inRigidBody    The name of the rigid body getting data for
        * @param inButtonDevice The name of the button device getting data for.
        * @param outData         Returns the current tracking raw data.
        *
        * @return True for success, false otherwise.
        */
        bool GetData(const std::string& rigid_body, const std::string& button_device, tracking::Tracker::TrackingData& data);

    private:

        /**********************************************************************
        * types
        **********************************************************************/

        typedef std::vector<std::unique_ptr<tracking::VrpnButtonDevice>> VrpnButtonPoolType;

        /**********************************************************************
        * variables
        **********************************************************************/

        bool initialised;
        bool connected;

        VrpnButtonPoolType buttonDevices;

        tracking::NatNetDevicePool motionDevices;

        /** parameters ********************************************************/

        /** Enables the tracker only on the node with the specified name. */
        std::string activeNode;

        /**********************************************************************
        * functions
        **********************************************************************/

        void printParams(void);

    };

} /** end namespace tracking */

#endif /** TRACKING_TRACKER_H_INCLUDED */