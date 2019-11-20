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
        enum Protocols  {
            VRPN_TCP = 0,
            VRPN_UDP = 1
        };

        /** Data structure for setting PARAMETERs as batch. */
        struct Params  {
            const char*                                 device_name;     /** The VRPN button device name. */
            size_t                                      device_name_len; 
            const char*                                 server_name;     /** The VRPN server name.        */
            size_t                                      server_name_len; 
            unsigned int                                port;            /** The VRPN port.               */
            typename tracking::VrpnDevice<R>::Protocols protocol;        /** The VRPN protocol.           */
        };

        ///////////////////////////////////////////////////////////////////////

        /**
        * CTOR
        */
        VrpnDevice(void);

        /**
        * Initialisation.
        *
        * @return True for success, false otherwise.
        */
        bool Initialise(const typename tracking::VrpnDevice<R>::Params& inParams);

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

        bool initialised;

        /** 
        * Pointer to remoteDevice vrpn device (e.g. button, tracker).
        */
        std::unique_ptr<R> remoteDevice; 

        /** PARAMETERs ********************************************************/

        /** 
        * Button device name. 
        * The name of the device (defined in vrpn cfg file).
        */
        std::string deviceName;

        /** 
        * VRPN server name.
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

        /** Print used PARAMETER values. */
        void printParams(void);

    };

} /** end namespace tracking */


/// Template classes must be declared AND defined in the header file.


template <class R>
tracking::VrpnDevice<R>::VrpnDevice(void)
    : initialised(false)
    , remoteDevice(nullptr)
    , deviceName("ControlBox")
    , serverName("mini")
    , port(3884)
    , protocol(VrpnDevice<R>::Protocols::VRPN_TCP) {
    
    // intentionally empty...
}


template <class R>
bool tracking::VrpnDevice<R>::Initialise(const typename VrpnDevice<R>::Params& inParams) {

    bool check = true;
    this->initialised = false;

    std::string device_name;
    try {
        device_name = std::string(inParams.device_name);
        if (device_name.length() != inParams.device_name_len) {
            std::cerr << std::endl << "[ERROR] [VrpnDevice] String \"device_name\" has not expected length. " <<
                "[" << __FILE__ << ", " << __FUNCTION__ << ", line " << __LINE__ << "]" << std::endl << std::endl;
            check = false;
        }
        if (device_name.empty()) {
            std::cerr << std::endl << "[ERROR] [VrpnDevice] Parameter \"device_name\" must not be empty string. " <<
                "[" << __FILE__ << ", " << __FUNCTION__ << ", line " << __LINE__ << "]" << std::endl << std::endl;
        }
    }
    catch (const std::exception& e) {
        std::cerr << std::endl << "[ERROR] [Tracker] Error reading string param 'active_node': " << e.what() <<
            " [" << __FILE__ << ", " << __FUNCTION__ << ", line " << __LINE__ << "]" << std::endl << std::endl;
        check = false;
    }

    std::string server_name;
    try {
        server_name = std::string(inParams.server_name);
        if (server_name.length() != inParams.server_name_len) {
            std::cerr << std::endl << "[ERROR] [VrpnDevice] String \"server_name\" has not expected length. " <<
                "[" << __FILE__ << ", " << __FUNCTION__ << ", line " << __LINE__ << "]" << std::endl << std::endl;
            check = false;
        }
        if (server_name.empty()) {
            std::cerr << std::endl << "[ERROR] [VrpnDevice] Parameter \"server_name\" must not be empty string. " <<
                "[" << __FILE__ << ", " << __FUNCTION__ << ", line " << __LINE__ << "]" << std::endl << std::endl;
            check = false;
        }
    }
    catch (const std::exception& e) {
        std::cerr << std::endl << "[ERROR] [Tracker] Error reading string param 'active_node': " << e.what() <<
            " [" << __FILE__ << ", " << __FUNCTION__ << ", line " << __LINE__ << "]" << std::endl << std::endl;
        check = false;
    }

    if (inParams.port >= 65535) {
        std::cerr << std::endl << "[ERROR] [VrpnDevice] Parameter \"port\" must be less than 65535. " <<
            "[" << __FILE__ << ", " << __FUNCTION__ << ", line " << __LINE__ << "]" << std::endl << std::endl;
        check = false;
    }

    if (check) {
        this->deviceName = device_name;
        this->serverName = server_name;
        this->port = inParams.port;
        this->protocol = inParams.protocol;

        this->printParams();
        this->initialised = true;
    }

    return this->initialised;
}


template <class R>
void tracking::VrpnDevice<R>::printParams(void) {
    std::cout << "[PARAMETER] [VrpnDevice] Device Name:                   " << this->deviceName.c_str() << std::endl;
    std::cout << "[PARAMETER] [VrpnDevice] Server Name:                   " << this->serverName.c_str() << std::endl;
    std::cout << "[PARAMETER] [VrpnDevice] Port:                          " << this->port << std::endl;
    std::cout << "[PARAMETER] [VrpnDevice] Protocol:                      " << (int)this->protocol << std::endl;
}


template <class R>
tracking::VrpnDevice<R>::~VrpnDevice(void) {

    this->Disconnect();
}


template <class R>
bool tracking::VrpnDevice<R>::Connect(void) {

    if (!this->initialised) {
        std::cerr << std::endl << "[ERROR] [VrpnDevice] Not initialised. " <<
            "[" << __FILE__ << ", " << __FUNCTION__ << ", line " << __LINE__ << "]" << std::endl << std::endl;
        return false;
    }

    std::string url;

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

    std::cout << "[INFO] [VrpnDevice] Connecting to VRPN server: " << url.c_str() << std::endl;

    vrpn_Connection *connection = vrpn_get_connection_by_name(
        url.c_str()
#ifdef TRACKING_VRPN_DEVICE_WRITE_PLAYBACKLOG
        ,(this->deviceName +  "_local_in.log").c_str(), (this->deviceName +  "_local_out.log").c_str(),
        (this->deviceName + "_remoteDevice_in.log").c_str(), (this->deviceName + "_remoteDevice_out.log").c_str()
#endif
        );

    if (connection->doing_okay() == vrpn_false) {
        std::cerr << std::endl << "[ERROR] [VrpnDevice] Failed to connect to VRPN server. " <<
            "[" << __FILE__ << ", " << __FUNCTION__ << ", line " << __LINE__ << "]" << std::endl << std::endl;
        connection->removeReference(); /// Adjust reference count manually.
        return false;
    }
    
    /// Create remoteDevice object calling CTOR with make_unique() for type R (e.g. vrpn_Button_remoteDevice, vrpn_Tracker_remoteDevice)
    this->remoteDevice = std::make_unique<R>(url.c_str(), connection); 
    this->remoteDevice->shutup = true;
    std::cout << "[INFO] [VrpnDevice] >>> AVAILABEL BUTTON DEVICE: \"" << this->deviceName.c_str() << "\"" << std::endl;

    std::cout << "[INFO] [VrpnDevice] Successfully connected to VRPN server." << std::endl;

    connection->removeReference(); /// Adjust reference count manually.
    return true;
}


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

    std::cout << "[INFO] [VrpnDevice] Successfully disconnected from VRPN server." << std::endl;

    return true;
}


template <class R> template <typename H>
bool tracking::VrpnDevice<R>::Register(H handler, void *userData) {

    if (!this->initialised) {
        std::cerr << std::endl << "[ERROR] [VrpnDevice] Not initialised. " <<
            "[" << __FILE__ << ", " << __FUNCTION__ << ", line " << __LINE__ << "]" << std::endl << std::endl;
        return false;
    }

    if (!this->remoteDevice) {
        std::cerr << std::endl << "[ERROR] [VrpnDevice] No remote device present. Call 'Connect()' prior to 'Register()'. " <<
            "[" << __FILE__ << ", " << __FUNCTION__ << ", line " << __LINE__ << "]" << std::endl << std::endl;
        return false;
    }

    if (userData == nullptr) {
        std::cerr << std::endl << "[ERROR] [VrpnDevice] Pointer to userData is NULL. " << __FILE__ << " " << __LINE__ << std::endl << std::endl;
        return false;
    }

    return (this->remoteDevice->register_change_handler(userData, handler) == 0);
}


template <class R>
bool tracking::VrpnDevice<R>::MainLoop(void) {

    if (!this->initialised) {
        std::cerr << std::endl << "[ERROR] [VrpnDevice] Not initialised. " <<
            "[" << __FILE__ << ", " << __FUNCTION__ << ", line " << __LINE__ << "]" << std::endl << std::endl;
        return false;
    }

    if (!this->remoteDevice) { 
        std::cerr << std::endl << "[ERROR] [VrpnDevice] No remote device present. Call 'Connect()' and 'Register()' prior to 'MainLoop()'. " <<
            "[" << __FILE__ << ", " << __FUNCTION__ << ", line " << __LINE__ << "]" << std::endl << std::endl;
        return false;
    }

    this->remoteDevice->mainloop();

    return true;
}


#endif /** TRACKING_VRPNDEVICE_H_INCLUDED */
