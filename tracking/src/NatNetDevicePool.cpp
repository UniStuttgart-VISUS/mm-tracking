/**
 * NatNetDevicePool.cpp
 *
 * Copyright (C) 2014-2018 by VISUS (Universitaet Stuttgart)
 * Alle Rechte vorbehalten.
 */

#include "NatNetDevicePool.h"


tracking::NatNetDevicePool::NatNetDevicePool(void)
    : initialised(false)
    , natnetClient(nullptr)
    , rigidBodies()
    , callbackCounter({ 0, 0 } )
    , clientIP("129.69.205.76") // minyou
    , serverIP("129.69.205.86") // mini
    , cmdPort(1510)
    , dataPort(1511)
    , conType(NatNetDevicePool::ConnectionType::UniCast)
    , verboseClient(false) {

    // intentionally empty...

}


bool tracking::NatNetDevicePool::Initialise(const NatNetDevicePool::Params & inParams) {

    bool check = true;
    this->initialised = false;
    this->natnetClient = nullptr;

    std::string client_ip;
    try {
        client_ip = std::string(inParams.client_ip);
        if (client_ip.length() != inParams.client_ip_len) {
            std::cerr << std::endl << "[ERROR] [NatNetClient] String \"client_ip\" has not expected length. " <<
                "[" << __FILE__ << ", " << __FUNCTION__ << ", line " << __LINE__ << "]" << std::endl << std::endl;
            check = false;
        }
        if (client_ip.empty()) {
            std::cerr << std::endl << "[ERROR] [NatNetClient] Parameter \"client_ip\" must not be empty string. " <<
                "[" << __FILE__ << ", " << __FUNCTION__ << ", line " << __LINE__ << "]" << std::endl << std::endl;
            check = false;
        }
    }
    catch (const std::exception& e) {
        std::cerr << std::endl << "[ERROR] [Tracker] Error reading string param 'active_node': " << e.what() <<
            " [" << __FILE__ << ", " << __FUNCTION__ << ", line " << __LINE__ << "]" << std::endl << std::endl;
        check = false;
    }

    std::string server_ip;
    try {
        server_ip = std::string(inParams.server_ip);
        if (server_ip.length() != inParams.server_ip_len) {
            std::cerr << std::endl << "[ERROR] [NatNetClient] String \"server_ip\" has not expected length. " <<
                "[" << __FILE__ << ", " << __FUNCTION__ << ", line " << __LINE__ << "]" << std::endl << std::endl;
            check = false;
        }
        if (server_ip.empty()) {
            std::cerr << std::endl << "[ERROR] [NatNetClient] Parameter \"server_ip\" must not be empty string. " <<
                "[" << __FILE__ << ", " << __FUNCTION__ << ", line " << __LINE__ << "]" << std::endl << std::endl;
            check = false;
        }
    }
    catch (const std::exception& e) {
        std::cerr << std::endl << "[ERROR] [Tracker] Error reading string param 'active_node': " << e.what() <<
            " [" << __FILE__ << ", " << __FUNCTION__ << ", line " << __LINE__ << "]" << std::endl << std::endl;
        check = false;
    }

    if (inParams.cmd_port >= 65535) {
        std::cerr << std::endl << "[ERROR] [NatNetClient] Parameter \"cmd_port\" must be less than 65535. " <<
            "[" << __FILE__ << ", " << __FUNCTION__ << ", line " << __LINE__ << "]" << std::endl << std::endl;
        check = false;
    }

    if (inParams.data_port >= 65535) {
        std::cout << std::endl << "[ERROR] [NatNetClient] Parameter \"data_port\" must be less than 65535. " <<
            "[" << __FILE__ << ", " << __FUNCTION__ << ", line " << __LINE__ << "]" << std::endl << std::endl;
        check = false;
    }

    if (check) {
        this->callbackCounter = { 0, 0 };
        this->clientIP = client_ip;
        this->serverIP = server_ip;

        this->cmdPort = inParams.cmd_port;
        this->dataPort = inParams.data_port;
        this->conType = inParams.con_type;
        this->verboseClient = inParams.verbose_client;

        this->printParams();
        this->initialised = true;
    }

    return this->initialised;
}


void tracking::NatNetDevicePool::printParams(void) {
    std::cout << "[PARAMETER] [NatNetDevicePool] Client IP:               " << this->clientIP.c_str() << std::endl;
    std::cout << "[PARAMETER] [NatNetDevicePool] Server IP:               " << this->serverIP.c_str() << std::endl;
    std::cout << "[PARAMETER] [NatNetDevicePool] Command Port:            " << this->cmdPort << std::endl;
    std::cout << "[PARAMETER] [NatNetDevicePool] Data Port:               " << this->dataPort << std::endl;
    std::cout << "[PARAMETER] [NatNetDevicePool] Connection Type:         " << (int)this->conType << std::endl;
    std::cout << "[PARAMETER] [NatNetDevicePool] Verbose NatNet client:   " << ((this->verboseClient)?("yes"):("no")) << std::endl;
}


tracking::NatNetDevicePool::~NatNetDevicePool(void) {

    this->Disconnect();
}


bool tracking::NatNetDevicePool::Connect(void) {

    if (!this->initialised) {
        std::cerr << std::endl << "[ERROR] [NatNetDevicePool] Not initialised. " <<
            "[" << __FILE__ << ", " << __FUNCTION__ << ", line " << __LINE__ << "]" << std::endl << std::endl;
        return false;
    }

    ::sServerDescription  serverDesc;
    ::sDataDescriptions  *dataDesc   = nullptr;
    ::ErrorCode           errorcode  = ::ErrorCode_OK;

    // Terminate previous connection.
    this->Disconnect();

    // Print local natnet version.
    //unsigned char        version[4];
    //::NatNet_GetVersion(version);
    //std::cout << "[info] [NatNetDevicePool] Local NatNet version: " << version[0] << "." << version[1] << "." << version[2] << "." << version[3] << std::endl;
    
    // Get connection parameters
    ::sNatNetClientConnectParams connectParams;
    connectParams.connectionType    = static_cast<::ConnectionType>(this->conType);
    connectParams.localAddress      = this->clientIP.c_str();
    connectParams.serverAddress     = this->serverIP.c_str();
    connectParams.serverCommandPort = static_cast<uint16_t>(this->cmdPort);
    connectParams.serverDataPort    = static_cast<uint16_t>(this->dataPort);
    if (connectParams.connectionType == ::ConnectionType::ConnectionType_Multicast) {
        std::cerr << std::endl << "[ERROR] [NatNetDevicePool] MultiCast is currently not supported. " <<
            "[" << __FILE__ << ", " << __FUNCTION__ << ", line " << __LINE__ << "]" << std::endl << std::endl;
        return false;
        //connectParams.multicastAddress = "224.0.0.1";
    }
    
    // Create thenatnetClient.
    this->natnetClient = std::make_unique<::NatNetClient>();

    std::cout << "[INFO] [NatNetDevicePool] Connecting to NatNet server ..." << std::endl;
    errorcode = this->natnetClient->Connect(connectParams);

    // Check whether the connection was successful.
    if (errorcode == ::ErrorCode_OK) {
        ::ZeroMemory(&serverDesc, sizeof(serverDesc));
        this->natnetClient->GetServerDescription(&serverDesc);

        if (!serverDesc.HostPresent) {
            std::cerr << std::endl << "[ERROR] [NatNetDevicePool] Disconnecting. No NatNet host is present. " <<
                "[" << __FILE__ << ", " << __FUNCTION__ << ", line " << __LINE__ << "]" << std::endl << std::endl;
            this->natnetClient.reset(nullptr);
            return false;
        }

        // Print some information on connection.
        std::cout << "[INFO] [NatNetDevicePool] Successfully connected to NatNet server: " << serverDesc.szHostComputerName << " - IP: " <<
            (int)serverDesc.HostComputerAddress[0] << "." << (int)serverDesc.HostComputerAddress[1] << "." <<
            (int)serverDesc.HostComputerAddress[2] << "." << (int)serverDesc.HostComputerAddress[3] << std::endl;

        std::cout << "[INFO] [NatNetDevicePool] NatNet host application: " << serverDesc.szHostApp << " " <<
            (int)serverDesc.HostAppVersion[0] << "." << (int)serverDesc.HostAppVersion[1] << "." <<
            (int)serverDesc.HostAppVersion[2] << "." << (int)serverDesc.HostAppVersion[3] << std::endl;

        std::cout << "[INFO] [NatNetDevicePool] Server side NatNet version: " << (int)serverDesc.NatNetVersion[0] << "." << 
            (int)serverDesc.NatNetVersion[1] << "." << (int)serverDesc.NatNetVersion[2] << "." <<
            (int)serverDesc.NatNetVersion[3] << std::endl;
    }
    else {
        std::cerr << std::endl << "[ERROR] [NatNetDevicePool] Failed to connect to NatNet server. - NETNET ERROR CODE: " << (int)errorcode  << " (see NatNetTypes.h, line 115). " <<
            "[" << __FILE__ << ", " << __FUNCTION__ << ", line " << __LINE__ << "]" << std::endl << std::endl;
        this->natnetClient.reset(nullptr);
        return false;
    }

    // Register callback handlers.
    std::cout << "[INFO] [NatNetDevicePool] Registering callbacks ..." << std::endl;
    this->natnetClient->SetFrameReceivedCallback(NatNetDevicePool::onData, const_cast<NatNetDevicePool *>(this));
    if (this->verboseClient) {
        NatNet_SetLogCallback(NatNetDevicePool::onMessage);
    }

    // Send remote command(s)
    float *frResponse  = nullptr;
    int    cntResponse = 0;
    // FrameRate
    errorcode = this->natnetClient->SendMessageAndWait("FrameRate", (void **)&frResponse, &cntResponse);
    if (errorcode == ErrorCode_OK) {
        std::cout << "[INFO] [NatNetDevicePool] NatNet remote command test PASSED. Current framerate: " << (*frResponse) << std::endl;
    }
    else {
        std::cerr << std::endl << "[WARNING] [NatNetDevicePool] Unable to process NatNet framerate request. - NETNET ERROR CODE: " << (int)errorcode << " (see NatNetTypes.h, line 115). " <<
            "[" << __FILE__ << ", " << __FUNCTION__ << ", line " << __LINE__ << "]" << std::endl << std::endl;
        this->natnetClient.reset(nullptr);
        return false;
    }

    // Look up rigid body data descriptions.
    std::cout << "[INFO] [NatNetDevicePool] Looking up rigid bodies ..." << std::endl;
    errorcode = this->natnetClient->GetDataDescriptionList(&dataDesc);
    if (errorcode == ErrorCode_OK) {
        for (int i = 0; i < dataDesc->nDataDescriptions; ++i) {
            if (dataDesc->arrDataDescriptions[i].type == Descriptor_RigidBody) { // DataDescriptors
                auto *rb = dataDesc->arrDataDescriptions[i].Data.RigidBodyDescription;
                if (rb == nullptr) {
                    std::cout << std::endl << "[WARNING] [NatNetDevicePool] Empty rigid body description. " <<
                        "[" << __FILE__ << ", " << __FUNCTION__ << ", line " << __LINE__ << "]" << std::endl << std::endl;
                    continue;
                }
                // Create new lock free ring for rigid body data
                this->rigidBodies.emplace_back(std::make_shared<RigidBody>(rb->ID, rb->szName));

                std::cout << "[INFO] [NatNetDevicePool] >>> PROVIDED RIGID BODY \"" << this->rigidBodies.back()->name.c_str() << "\"."<< std::endl;
            }
        }
    }
    else {
        std::cerr << std::endl << "[ERROR] [NatNetDevicePool] Unable to retrieve rigid body data descriptions. - NETNET ERROR CODE: " << (int)errorcode << " (see NatNetTypes.h, line 115). " <<
            "[" << __FILE__ << ", " << __FUNCTION__ << ", line " << __LINE__ << "]" << std::endl << std::endl;
        this->Disconnect();
        return false;
    }

    return true;
}


bool tracking::NatNetDevicePool::Disconnect(void) {

    if (this->natnetClient != nullptr) {
        ::ErrorCode errorcode = this->natnetClient->Disconnect();
        this->natnetClient.reset(nullptr);
        if (errorcode == ErrorCode_OK) {
            std::cout << "[INFO] [NatNetDevicePool] Successfully disconnected from NatNet server." << std::endl;
        }
        else {
            std::cerr << "[ERROR] [NatNetDevicePool] Disconnected from NatNet server. - NATNET ERROR CODE: " << (int)errorcode << " (see NatNetTypes.h, line 115). " <<
                "[" << __FILE__ << ", " << __FUNCTION__ << ", line " << __LINE__ << "]" << std::endl << std::endl;
        }
    }

    // Clear rigid body data after disconnecting (otherwise callback for natnet might still be accessing data).
    for (unsigned int i = 0; i < this->rigidBodies.size(); ++i) {
        this->rigidBodies[i].reset();
    }
    this->rigidBodies.clear();

    return true;
}


tracking::Quaternion tracking::NatNetDevicePool::GetOrientation(const std::string& rbn) {

    tracking::Quaternion retOr((std::numeric_limits<float>::max)(),
        (std::numeric_limits<float>::max)(),
        (std::numeric_limits<float>::max)(),
        (std::numeric_limits<float>::max)());

    if (!this->initialised) {
        std::cerr << std::endl << "[ERROR] [NatNetDevicePool] Not initialised. " <<
            "[" << __FILE__ << ", " << __FUNCTION__ << ", line " << __LINE__ << "]" << std::endl << std::endl;
        return retOr;
    }

    // Check for updated data
    if (this->callbackCounter[0] == this->callbackCounter[1]) {
        this->callbackCounter[0] = this->callbackCounter[1] = 0;
        std::cout << std::endl << "[WARNING] [NatNetDevicePool] Didn't receive updated tracking data yet. " <<
            ">>> Please check your firewall settings if this warning appears repeatedly! " <<
            "[" << __FILE__ << ", " << __FUNCTION__ << ", line " << __LINE__ << "]" << std::endl << std::endl;
    }

    for (auto it : this->rigidBodies) {
        if (rbn == it->name) {
            retOr = it->lockFreeData[it->read.load()].orientation;
            break;
        }
    }

    return retOr;
}


tracking::Vector3D tracking::NatNetDevicePool::GetPosition(const std::string& rbn) {

    tracking::Vector3D retPos((std::numeric_limits<float>::max)(),
        (std::numeric_limits<float>::max)(),
        (std::numeric_limits<float>::max)());

    if (!this->initialised) {
        std::cerr << std::endl << "[ERROR] [NatNetDevicePool] Not initialised. " <<
            "[" << __FILE__ << ", " << __FUNCTION__ << ", line " << __LINE__ << "]" << std::endl << std::endl;
        return retPos;
    }

    // Check for updated data
    if (this->callbackCounter[0] == this->callbackCounter[1]) {
        this->callbackCounter[0] = this->callbackCounter[1] = 0;
        std::cout << std::endl << "[WARNING] [NatNetDevicePool] Didn't receive updated tracking data yet. " << 
            ">>> Please check your firewall settings if this warning appears repeatedly! " <<
            "[" << __FILE__ << ", " << __FUNCTION__ << ", line " << __LINE__ << "]" << std::endl << std::endl;
    }

    for (auto it : this->rigidBodies) { 
        if (rbn == it->name) {
            retPos = it->lockFreeData[it->read.load()].position;
            break;
        }
    }

    return retPos;
}


std::vector<std::string> tracking::NatNetDevicePool::GetRigidBodyNames(void) const {

    std::vector<std::string> retval;
    retval.clear();

    if (!this->initialised) {
        std::cerr << std::endl << "[ERROR] [NatNetDevicePool] Not initialised. " <<
            "[" << __FILE__ << ", " << __FUNCTION__ << ", line " << __LINE__ << "]" << std::endl << std::endl;
        return retval;
    }

    for (auto it : this->rigidBodies) { 
        retval.emplace_back(it->name);
    }

    return retval;
}


void __cdecl tracking::NatNetDevicePool::onData(sFrameOfMocapData *pFrameOfData, void *pUserData) {

	auto that = static_cast<NatNetDevicePool *>(pUserData);
    if ((pFrameOfData == nullptr) || (that == nullptr)) {
        std::cerr << std::endl << "[ERROR] [NatNetDevicePool] Pointer to userData is NULL. " <<
            "[" << __FILE__ << ", " << __FUNCTION__ << ", line " << __LINE__ << "]" << std::endl << std::endl;
        return;
    }

    // Simple counter to be able to check if callback has been called
    ++that->callbackCounter[1];
    // Prevent overflow
    if (that->callbackCounter[1] > ((std::numeric_limits<unsigned int>::max)() - 2)) {
        that->callbackCounter[0] = 0;
        that->callbackCounter[1] = 1;
    }

    for (int i = 0; i < pFrameOfData->nRigidBodies; ++i) {
        // All zero seems to be an indicator that the rigid body is not
        // visible at the moment. Skip ...
        bool isValid = (
               (pFrameOfData->RigidBodies[i].qx != 0.0f)
            || (pFrameOfData->RigidBodies[i].qy != 0.0f)
            || (pFrameOfData->RigidBodies[i].qz != 0.0f)
            || (pFrameOfData->RigidBodies[i].qw != 0.0f)
            || (pFrameOfData->RigidBodies[i].x  != 0.0f)
            || (pFrameOfData->RigidBodies[i].y  != 0.0f)
            || (pFrameOfData->RigidBodies[i].z  != 0.0f)
            );
#ifdef TRACKING_DEBUG_OUTPUT
        //std::cout << "[DEBUG] <NatNetDevicePool::onData> ID = " << pFrameOfData->RigidBodies[i].ID << " MeanError = " << pFrameOfData->RigidBodies[i].MeanError <<
        //    "; Params = " << pFrameOfData->RigidBodies[i].params << "; Orientation = " << pFrameOfData->RigidBodies[i].qx << ", " <<
        //    pFrameOfData->RigidBodies[i].qy << ", " << pFrameOfData->RigidBodies[i].qz << ", " << pFrameOfData->RigidBodies[i].qw <<
        //    "; Position = " << pFrameOfData->RigidBodies[i].x << ", " << pFrameOfData->RigidBodies[i].y << ", " << pFrameOfData->RigidBodies[i].z << "; (isValid = " << isValid << ")." << std::endl;
#endif

        if (isValid) {
            sRigidBodyData data = pFrameOfData->RigidBodies[i];

            // Set data for current rigid body
            for (auto it : that->rigidBodies) {
                if (data.ID == it->id) {

                    // Write new data to object with index denoted as 'write'
                    it->lockFreeData[it->write.load()].orientation.SetX(data.qx);
                    it->lockFreeData[it->write.load()].orientation.SetY(data.qy);
                    it->lockFreeData[it->write.load()].orientation.SetZ(data.qz);
                    it->lockFreeData[it->write.load()].orientation.SetW(data.qw);

                    it->lockFreeData[it->write.load()].position.SetX(data.x);
                    it->lockFreeData[it->write.load()].position.SetY(data.y);
                    it->lockFreeData[it->write.load()].position.SetZ(data.z);

                    // Determine index of currently unused free object
                    unsigned int free = (it->read.load() + 1) % 3;
                    free = (it->write.load() == free) ? ((it->write.load() + 1) % 3) : (free);
#ifdef TRACKING_DEBUG_OUTPUT
                    //std::cout << "[DEBUG] <NatNetDevicePool::onData> READ = " << it->read.load() << " - WRTIE = " << it->write.load() << " - FREE = " << free << "." << std::endl;
#endif
                    // Swap lock free read/write buffers
                    it->read.store(it->write.load());
                    it->write.store(free);
                    
                    break; /// Break loop if rigid body is found
                }
            }
        }
    }
}


void __cdecl tracking::NatNetDevicePool::onMessage(Verbosity level, const char *message) {

    switch (level) {
        case (::Verbosity::Verbosity_None):    break;
        case (::Verbosity::Verbosity_Debug):   std::cout << "[DEBUG]   [NatNetClient] "   << message << std::endl; break;
        case (::Verbosity::Verbosity_Info):    std::cout << "[INFO]    [NatNetClient] "    << message << std::endl; break;
        case (::Verbosity::Verbosity_Warning): std::cout << "[WARNING] [NatNetClient] " << message << std::endl; break;
        case (::Verbosity::Verbosity_Error):   std::cout << "[ERROR]   [NatNetClient] "   << message << std::endl; break;
        default: break;
    }
}

