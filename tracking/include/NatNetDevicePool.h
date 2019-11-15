/**
 * NatNetDevicePool.h
 *
 * Copyright (C) 2014-2018 by VISUS (Universitaet Stuttgart)
 * Alle Rechte vorbehalten.
 */

#ifndef TRACKING_NATNETDEVICEPOOL_H_INCLUDED
#define TRACKING_NATNETDEVICEPOOL_H_INCLUDED

#include "stdafx.h"

#include "NatNetTypes.h"
#include "NatNetClient.h"
#include "NatNetCAPI.h"


namespace mm_tracking{

    /***************************************************************************
    *
    * Manages the connection to a NatNet host application and holds the 
    * tracking data of available rigid bodies.
    *
    ***************************************************************************/
    class NatNetDevicePool {

    public:
       
        /** Supported connection types for NatNet. */
        enum ConnectionType {
            MultiCast = 0,
            UniCast   = 1
        };

        /** Data structure for setting parameters as batch. */
        typedef struct { 
            std::string                      clientIP;      /** The IP address of the NatNet client.       */
            std::string                      serverIP;      /** The IP address of the NatNet server.       */
            unsigned int                     cmdPort;       /** The NatNet command port.                   */
            unsigned int                     dataPort;      /** The NatNet data port.                      */
            NatNetDevicePool::ConnectionType conType;       /** The NatNet connection type.                */
            bool                             verboseClient; /** Turn on/off NatNet client massage output.  */
        } Params;

        /** Data of one rigid body. */
        typedef struct {
            mm_tracking::Quaternion orientation;               /** The current orientation of the motion device. */
            mm_tracking::Vector3D   position;                  /** The current position of the motion device. */
        } RigidBodyData;

        ///////////////////////////////////////////////////////////////////////

        /**
        * CTOR
        */
        NatNetDevicePool(void);

        NatNetDevicePool(NatNetDevicePool::Params& inParams);

        /**
        * DTOR
        */
        ~NatNetDevicePool(void);

        /**
        * Connect to natnet server.
        *
        * @return Id name pairs of all found rigid bodies.
        */
        bool Connect(void);

        /**
        * Disconnect from natnet server.
        *
        * @return True on success, false otherwise.
        */
        bool Disconnect(void);

        /*********************************************************************/
        // GET

        /**
        * Get orientation of rigid body. 
        * If no name is given, the data of the first rigid body is returned.
        *
        * @param rbn The rigid body name to get the orientation of
        * 
        * @return The current orientation of the given rigid body.
        */
        mm_tracking::Quaternion GetOrientation(std::string& rbn); // const;

        /**
        * Get position of rigid body.
        * If no name is given, the data of the first rigid body is returned.
        *
        * @param rbn The rigid body name to get the position of.
        * 
        * @return The current position of the given rigid body.
        */
        mm_tracking::Vector3D GetPosition(std::string& rbn); // const;

        /**
        * Get all available rigid body names.
        *
        * @return All available rigid body names.
        */
        std::vector<std::string> GetRigidBodyNames(void) const;

         /*********************************************************************/
         // SET

         inline void SetClientIP(std::string cip) {
             this->clientIP = cip;
         }

         inline void SetServertIP(std::string sip) {
             this->serverIP = sip;
         }

         inline void SetCmdPort(unsigned int cp) {
             this->cmdPort = cp;
         }

         inline void SetDataPort(unsigned int dp) {
             this->dataPort = dp;
         }

         inline void SetConnectionType(NatNetDevicePool::ConnectionType ct) {
             this->conType = ct;
         }

         inline void SetClientVerbose(bool  v) {
             this->verboseClient = v;
         }

    private:

        /***********************************************************************
        * types and structs
        **********************************************************************/

        /** 
        *  Data structure for rigid bodies. 
        *
        *  Atomic variables have no copy constructor, therefore class
        *  can only be used in std:.vector via (shared) pointer.
        *
        *  Mutable data is stored in lock free (triple-)buffer for concurrent 
        *  access by natnet callback and megamol callback.
        */
        class RigidBody {
        public:
            RigidBody(int ctor_id, std::string ctor_name) : 
                id(ctor_id), 
                name(ctor_name),
                read(0), 
                write(1), 
                lockFreeData()
            { 
            
            }

            const int                 id;                // ID of motoin device (never changes)
            const std::string         name;              // Name of motion device (never changes)
            std::atomic<unsigned int> read;              // index of readable RigidBodyData
            std::atomic<unsigned int> write;             // index of writable RigidBodyData
            RigidBodyData             lockFreeData[3];   // triple buffer of data for one rigid body

        };

        /**********************************************************************
        * variables
        **********************************************************************/

        /** Pointer to the NatNet client. */
        std::unique_ptr<NatNetClient> natnetClient;

        /** Data of all available rigid bodies. */
        std::vector<std::shared_ptr<RigidBody>> rigidBodies;

        std::array<unsigned int, 2> callbackCounter; // [last, current]

        /** parameters ********************************************************/

        /** 
        * Specifies the localnatnetClient IP address used for NatNet.
        */
        std::string clientIP;

        /**
        * Specifies the server IP address used for NatNet.
        */
        std::string serverIP;

        /**
        * Specifies the port used for NatNet commands.
        */
        unsigned int cmdPort;

        /**
        * Specifies the port used for NatNet data.
        */
        unsigned int dataPort;

        /** *
        * Specifies the type of NatNet connection.
        */
        NatNetDevicePool::ConnectionType conType;

        /**
        * TODO.
        */
        bool verboseClient;

        /**********************************************************************
        * functions
        **********************************************************************/

        /** Print used parameter values. */
        void paramsPrint(void);

        /** Print used parameter values. */
        bool paramsCheck(void);

        /**
        * NatNet client callback for data.
        *
        * @param pFrameOfData Single frame of data for all tracked objects.
        * @param pUserData    Pointer to class which registered callback (that).
        */
        static void __cdecl onData(sFrameOfMocapData *pFrameOfData, void *pUserData);

        /**
        * NatNet client callback for messages.
        *
        * @param level   The verbosity level of the message.
        * @param message The message.
        */
        static void __cdecl onMessage(Verbosity level, const char *message);
        
    };

} /** end namespace mm_tracking */

#endif /** TRACKING_NATNETDEVICEPOOL_H_INCLUDED */
