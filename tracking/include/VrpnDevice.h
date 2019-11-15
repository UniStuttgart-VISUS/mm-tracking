/**
 * VrpnDevice.h
 *
 * Copyright (C) 2014 by Tilo Pfannkuch
 * Copyright (C) 2008-2018 by VISUS (Universitaet Stuttgart)
 * Alle Rechte vorbehalten.
 */

#ifndef TRACKING_VRPNDEVICE_H_INCLUDED
#define TRACKING_VRPNDEVICE_H_INCLUDED

#include "stdafx.h"
#include "vrpn_Tracker.h"
#include "vrpn_Button.h"


namespace tracking {

    /***************************************************************************
    *
    * Template for connecting to remote VRPN device.
    *
    ***************************************************************************/
    template <class R> class VrpnDevice {

    public:

        /** Supported network protocols for VRPN. */
        typedef enum {
            VRPN_TCP = 0,
            VRPN_UDP = 1
        } Protocols;

        /** Data structure for setting parameters as batch. */
        typedef struct  {
            std::string                       deviceName;   /** The VRPN button device name. */
            std::string                       serverName;   /** The VRPN server name.        */
            unsigned int                      port;         /** The VRPN port.               */
            typename VrpnDevice<R>::Protocols protocol;     /** The VRPN protocol.           */
        } Params;

        ///////////////////////////////////////////////////////////////////////

        /**
        * CTOR
        */
        VrpnDevice(void);

        VrpnDevice(typename VrpnDevice<R>::Params& inParams);

        /**
        * DTOR
        */
        ~VrpnDevice(void);

        /**
        * Connect to VRPN device.
        *
        * @return True on success, false otherwise.
        */
        virtual bool Connect(void);

        /**
        * Disconnect from VRPN device
        *
        * @return True on success, false otherwise.
        */
        virtual bool Disconnect(void);

        /**********************************************************************/
        // GET

        inline std::string GetDeviceName(void) const {
            return this->deviceName;
        }

        /**********************************************************************/
        // SET

        inline void SetDeviceName(std::string dn) {
            this->deviceName = dn;
        }

        inline void SetServerName(std::string sn) {
            this->serverName = sn;
        }

        inline void SetPort(unsigned int p) {
            this->port = p;
        }

        inline void SetProtocol(typename VrpnDevice<R>::Protocols p) {
            this->protocol = p;
        }

    protected:

        /**
        * Register callback function which is called by vrpn.
        *
        * @param handler  Handler of callback function.
        * @param userData Pointer to class (this), which registers callback.
        *
        * @return True on success, false otherwise.
        */
        template <typename H> bool Register(H handler, void *userData);

        /**
        * Starting main loop of VRPN device.
        *
        * @return True on success, false otherwise.
        */
        bool MainLoop(void);

    private:

        /***********************************************************************
        * variables
        **********************************************************************/

        /** 
        * Pointer to remoteDevice vrpn device (e.g. button, tracker).
        */
        std::unique_ptr<R> remoteDevice; 

        /** parameters ********************************************************/

        /** 
        * Button device name. 
        * The name of the device (defined in vrpn cfg file).
        */
        std::string deviceName;

        /** 
        * VRPN server name. *
        * The sever IP address or server host name running vrpn and hosting the device.
        */
        std::string serverName;

        /** 
        * The port used for connecting to the vrpn server.
        * Defined in following file on server: \\mini\c$\vrpn\start64.bat
        */
        unsigned int port;

        /**
        * The protocol used for connecting to the vrpn server.
        */
        typename VrpnDevice<R>::Protocols protocol;

        /***********************************************************************
        * functions
        **********************************************************************/

        /** Print used parameter values. */
        void paramsPrint(void);

        /** Print used parameter values. */
        bool paramsCheck(void);

    };

} /** end namespace tracking */


/// Template classes must be declared AND defined in the header file.


/**
* tracking::VrpnDevice::VrpnDevice
*/
template <class R>
tracking::VrpnDevice<R>::VrpnDevice(void) :
    remoteDevice(nullptr),
    deviceName("ControlBox"),
    serverName("mini"),
    port(3884),
    protocol(VrpnDevice<R>::Protocols::VRPN_TCP)
{
    this->paramsPrint();
}


template <class R>
tracking::VrpnDevice<R>::VrpnDevice(typename VrpnDevice<R>::Params& inParams) :
    remoteDevice(nullptr),
    deviceName(inParams.deviceName),
    serverName(inParams.serverName),
    port(inParams.port),
    protocol(inParams.protocol)
{
    this->paramsPrint();
}


/**
* tracking::VrpnDevice::paramsPrint
*/
template <class R>
void tracking::VrpnDevice<R>::paramsPrint(void) {
    std::cout << "[parameter] <VrpnDevice> Device Name:                   " << this->deviceName.c_str() << std::endl;
    std::cout << "[parameter] <VrpnDevice> Server Name:                   " << this->serverName.c_str() << std::endl;
    std::cout << "[parameter] <VrpnDevice> Port:                          " << this->port << std::endl;
    std::cout << "[parameter] <VrpnDevice> Protocol:                      " << (int)this->protocol << std::endl;
}


/**
* tracking::VrpnDevice::paramsCheck
*/
template <class R>
bool tracking::VrpnDevice<R>::paramsCheck(void) {

    bool retval = true;

    if (this->deviceName.empty()) {
        std::cout << std::endl << "[WARNING] <VrpnDevice> Parameter \"deviceName\" must not be empty." << std::endl << std::endl;
        retval = false;
    }
    if (this->serverName.empty()) {
        std::cout << std::endl << "[WARNING] <VrpnDevice> Parameter \"serverName\" must not be empty." << std::endl << std::endl;
        retval = false;
    }
    if (this->port > 65535) {
        std::cout << std::endl << "[WARNING] <VrpnDevice> Parameter \"port\" must not be greater than 65535." << std::endl << std::endl;
        retval = false;
    }

    /// No check necessary for:
    // this->protocol

    return retval;
}


/**
* tracking::VrpnDevice::~VrpnDevice
*/
template <class R>
tracking::VrpnDevice<R>::~VrpnDevice(void) {

    this->Disconnect();
}


/**
* tracking::VrpnDevice::connect
*/
template <class R>
bool tracking::VrpnDevice<R>::Connect(void) {

    std::string url;

    // Check for valid parameters
    if (!this->paramsCheck()) {
        return false;
    }

    // Terminate previous connection.    
    this->Disconnect();

    // Translate protocol to string
    std::string prot = "";
    switch (this->protocol) {
        case(VrpnDevice<R>::Protocols::VRPN_TCP): prot = "tcp";  break;
        case(VrpnDevice<R>::Protocols::VRPN_UDP): prot = "udp";  break;
        default: break;
    }

    /// URL: [device_name]@[protocol]://[server_name]:[port]
    std::ostringstream str;
    str << this->deviceName.c_str() << "@" << prot.c_str() << "://" << this->serverName.c_str() << ":" << this->port;
    url = str.str();

    std::cout << "[info] <VrpnDevice> Connecting to VRPN server: " << url.c_str() << std::endl;

    vrpn_Connection *connection = vrpn_get_connection_by_name(
        url.c_str()
#ifdef TRACKING_VRPN_DEVICE_WRITE_PLAYBACKLOG
        ,(this->deviceName +  "_local_in.log").c_str(), (this->deviceName +  "_local_out.log").c_str(),
        (this->deviceName + "_remoteDevice_in.log").c_str(), (this->deviceName + "_remoteDevice_out.log").c_str()
#endif
        );

    if (connection->doing_okay() == vrpn_false) {
        std::cerr << std::endl << "[ERROR] <VrpnDevice> Failed to connect to VRPN server." << std::endl << std::endl << std::endl;
        connection->removeReference(); /// Adjust reference count manually.
        return false;
    }
    
    /// Create remoteDevice object calling CTOR with make_unique() for type R (e.g. vrpn_Button_remoteDevice, vrpn_Tracker_remoteDevice)
    this->remoteDevice = std::make_unique<R>(url.c_str(), connection); 
    this->remoteDevice->shutup = true;
    std::cout << "[info] <VrpnDevice> >>> AVAILABEL BUTTON DEVICE: \"" << this->deviceName.c_str() << "\"" << std::endl;

    std::cout << "[info] <VrpnDevice> Successfully connected to VRPN server." << std::endl;

    connection->removeReference(); /// Adjust reference count manually.
    return true;
}


/**
* tracking::VrpnDevice::disconnect
*/
template <class R>
bool tracking::VrpnDevice<R>::Disconnect(void) {

    if (!this->remoteDevice) {
        return false;
    }

    // Disconnecting (= deletion) is handled by vrpn_ConnectionManager which 
    // is called in DTOR of vrpn_Connection. Deleting the remote device object will 
    // also delete the connection object and this is the way to disconnect.
    // The callbacks are unregistered in this way, too. Resetting unique_ptr 
    // to nullptr will delete the remoteDevice object and calls the other DTORs recursively.
    this->remoteDevice.reset(nullptr);

    std::cout << "[info] <VrpnDevice> Successfully disconnected from VRPN server." << std::endl;

    return true;
}


/**
* tracking::VrpnDevice::Register
*/
template <class R> template <typename H>
bool tracking::VrpnDevice<R>::Register(H handler, void *userData) {

    if (!this->remoteDevice) {
        std::cerr << std::endl << "[ERROR] <VrpnDevice> No remote device present. (Hint: Call [Connect] prior to [Register])" << std::endl << std::endl;
        return false;
    }

    if (userData == nullptr) {
        std::cerr << std::endl << "[ERROR] <VrpnDevice> Pointer to userData is NULL. " << __FILE__ << " " << __LINE__ << std::endl << std::endl;
        return false;
    }

    return (this->remoteDevice->register_change_handler(userData, handler) == 0);
}


/**
* tracking::VrpnDevice::MainLoop
*/
template <class R>
bool tracking::VrpnDevice<R>::MainLoop(void) {

    if (!this->remoteDevice) { 
        std::cerr << std::endl << "[ERROR] <VrpnDevice> No remote device present. (Hint: Call [Connect] and [Register] prior to [MainLoop])" << std::endl << std::endl;
        return false;
    }

    this->remoteDevice->mainloop();

    return true;
}


#endif /** TRACKING_VRPNDEVICE_H_INCLUDED */
