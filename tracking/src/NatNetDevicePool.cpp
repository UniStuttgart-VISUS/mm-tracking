/**
 * NatNetDevicePool.cpp
 *
 * Copyright (C) 2014-2018 by VISUS (Universitaet Stuttgart)
 * Alle Rechte vorbehalten.
 */

#include "NatNetDevicePool.h"


tracking::NatNetDevicePool::NatNetDevicePool(void) :
    natnetClient(nullptr),
    rigidBodies(),
    callbackCounter({ 0, 0 } ),
    clientIP("129.69.205.76"), // minyou
    serverIP("129.69.205.86"), // mini
    cmdPort(1510), 
    dataPort(1511),
    conType(NatNetDevicePool::ConnectionType::UniCast),
    verboseClient(false)
{
    this->paramsPrint();
}


tracking::NatNetDevicePool::NatNetDevicePool(const NatNetDevicePool::Params& p) :
    natnetClient(nullptr),
    rigidBodies(),
    callbackCounter({ 0, 0 }),
    clientIP(p.clientIP),
    serverIP(p.serverIP),
    cmdPort(p.cmdPort),
    dataPort(p.dataPort),
    conType(p.conType),
    verboseClient(p.verboseClient)
{
    this->paramsPrint();
}


void tracking::NatNetDevicePool::paramsPrint(void) {
    std::cout << "[parameter] <NatNetDevicePool> Client IP:               " << this->clientIP.c_str() << std::endl;
    std::cout << "[parameter] <NatNetDevicePool> Server IP:               " << this->serverIP.c_str() << std::endl;
    std::cout << "[parameter] <NatNetDevicePool> Command Port:            " << this->cmdPort << std::endl;
    std::cout << "[parameter] <NatNetDevicePool> Data Port:               " << this->dataPort << std::endl;
    std::cout << "[parameter] <NatNetDevicePool> Connection Type:         " << (int)this->conType << std::endl;
    std::cout << "[parameter] <NatNetDevicePool> Verbose NatNet client:   " << ((this->verboseClient)?("yes"):("no")) << std::endl;
}


bool tracking::NatNetDevicePool::paramsCheck(void) {

    bool retval = true;

    if (this->clientIP.empty()) {
        std::cout << std::endl << "[WARNING] <VrpnDevice> Parameter \"clientIP\" must not be empty." << std::endl << std::endl;
        retval = false;
    }
    if (this->serverIP.empty()) {
        std::cout << std::endl << "[WARNING] <VrpnDevice> Parameter \"serverIP\" must not be empty." << std::endl << std::endl;
        retval = false;
    }
    if (this->cmdPort > 65535) {
        std::cout << std::endl << "[WARNING] <VrpnDevice> Parameter \"cmdPort\" must not be greater than 65535." << std::endl << std::endl;
        retval = false;
    }
    if (this->dataPort > 65535) {
        std::cout << std::endl << "[WARNING] <VrpnDevice> Parameter \"dataPort\" must not be greater than 65535." << std::endl << std::endl;
        retval = false;
    }

    /// No check necessary for:
    // this->conType
    // this->verbose

    return retval;
}


tracking::NatNetDevicePool::~NatNetDevicePool(void) {

    this->Disconnect();
}


bool tracking::NatNetDevicePool::Connect(void) {

    ::sServerDescription serverDesc;

    ::sDataDescriptions *dataDesc   = nullptr;
    ::ErrorCode          errorcode  = ::ErrorCode_OK;

    // Check for valid parameters
    if (!this->paramsCheck()) {
        return false;
    }

    // Terminate previous connection.
    this->Disconnect();

    // Print local natnet version.
    //unsigned char        version[4];
    //::NatNet_GetVersion(version);
    //std::cout << "[info] <NatNetDevicePool> Local NatNet version: " << version[0] << "." << version[1] << "." << version[2] << "." << version[3] << std::endl;
    
    // Get connection parameters
    ::sNatNetClientConnectParams connectParams;
    connectParams.connectionType    = static_cast<::ConnectionType>(this->conType);
    connectParams.localAddress      = this->clientIP.c_str();
    connectParams.serverAddress     = this->serverIP.c_str();
    connectParams.serverCommandPort = static_cast<uint16_t>(this->cmdPort);
    connectParams.serverDataPort    = static_cast<uint16_t>(this->dataPort);
    if (connectParams.connectionType == ::ConnectionType::ConnectionType_Multicast) {
        std::cerr << std::endl << "[ERROR] <NatNetDevicePool> MultiCast is currently not supported." << std::endl << std::endl;
        return false;
        //connectParams.multicastAddress = "224.0.0.1";
    }

    // Check required parameters
    if (std::string(connectParams.localAddress).empty()) {
        std::cerr << std::endl << "[ERROR] <NatNetDevicePool> No CLIENT IP address given for conenction to NatNet server." << std::endl << std::endl;
        return false;
    }
    if (std::string(connectParams.serverAddress).empty()) {
        std::cerr << std::endl << "[ERROR] <NatNetDevicePool> No SERVER IP address given for conenction to NatNet server." << std::endl << std::endl;
        return false;
    }
    
    // Create thenatnetClient.
    this->natnetClient = std::make_unique<::NatNetClient>();

    std::cout << "[info] <NatNetDevicePool> Connecting to NatNet server ..." << std::endl;
    errorcode = this->natnetClient->Connect(connectParams);

    // Check whether the connection was successful.
    if (errorcode == ::ErrorCode_OK) {
        ::ZeroMemory(&serverDesc, sizeof(serverDesc));
        this->natnetClient->GetServerDescription(&serverDesc);

        if (!serverDesc.HostPresent) {
            std::cerr << std::endl << "[ERROR] <NatNetDevicePool> Disconnecting. No NatNet host is present." << std::endl << std::endl;
            this->natnetClient.reset(nullptr);
            return false;
        }

        // Print some information on connection.
        std::cout << "[info] <NatNetDevicePool> Successfully connected to NatNet server: " << serverDesc.szHostComputerName << " - IP: " <<
            (int)serverDesc.HostComputerAddress[0] << "." << (int)serverDesc.HostComputerAddress[1] << "." <<
            (int)serverDesc.HostComputerAddress[2] << "." << (int)serverDesc.HostComputerAddress[3] << std::endl;

        std::cout << "[info] <NatNetDevicePool> NatNet host application: " << serverDesc.szHostApp << " " <<
            (int)serverDesc.HostAppVersion[0] << "." << (int)serverDesc.HostAppVersion[1] << "." <<
            (int)serverDesc.HostAppVersion[2] << "." << (int)serverDesc.HostAppVersion[3] << std::endl;

        std::cout << "[info] <NatNetDevicePool> Server side NatNet version: " << (int)serverDesc.NatNetVersion[0] << "." << 
            (int)serverDesc.NatNetVersion[1] << "." << (int)serverDesc.NatNetVersion[2] << "." <<
            (int)serverDesc.NatNetVersion[3] << std::endl;
    }
    else {
        std::cerr << std::endl << "[ERROR] <NatNetDevicePool> Failed to connect to NatNet server - NETNET ERROR CODE: " << (int)errorcode  << " (see NatNetTypes.h, line 115)." << std::endl << std::endl;
        this->natnetClient.reset(nullptr);
        return false;
    }

    // Register callback handlers.
    std::cout << "[info] <NatNetDevicePool> Registering callbacks ..." << std::endl;
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
        std::cout << "[info] <NatNetDevicePool> NatNet remote command test PASSED. Current framerate: " << (*frResponse) << std::endl;
    }
    else {
        std::cerr << std::endl << "[WARNING] <NatNetDevicePool> Unable to process NatNet framerate request - NETNET ERROR CODE: " << (int)errorcode << " (see NatNetTypes.h, line 115)" << std::endl << std::endl;
        this->natnetClient.reset(nullptr);
        return false;
    }

    // Look up rigid body data descriptions.
    std::cout << "[info] <NatNetDevicePool> Looking up rigid bodys ..." << std::endl;
    errorcode = this->natnetClient->GetDataDescriptionList(&dataDesc);
    if (errorcode == ErrorCode_OK) {
        for (int i = 0; i < dataDesc->nDataDescriptions; ++i) {
            if (dataDesc->arrDataDescriptions[i].type == Descriptor_RigidBody) { // DataDescriptors
                auto *rb = dataDesc->arrDataDescriptions[i].Data.RigidBodyDescription;
                if (rb == nullptr) {
                    std::cout << std::endl << "[WARNING] <NatNetDevicePool> Empty rigid body description." << std::endl << std::endl;
                    continue;
                }
                // Create new lock free ring for rigid body data
                this->rigidBodies.push_back(std::make_shared<RigidBody>(rb->ID, rb->szName));

                std::cout << "[info] <NatNetDevicePool> >>> PROVIDED RIGID BODY \"" << this->rigidBodies.back()->name.c_str() << "\"."<< std::endl;
            }
        }
    }
    else {
        std::cerr << std::endl << "[ERROR] <NatNetDevicePool> Unable to retrieve rigid body data descriptions - NETNET ERROR CODE: " << (int)errorcode << " (see NatNetTypes.h, line 115)" << std::endl << std::endl;
        this->Disconnect();
        return false;
    }

    return true;
}


bool tracking::NatNetDevicePool::Disconnect(void) {

    if (this->natnetClient) {
        ::ErrorCode errorcode = this->natnetClient->Disconnect();
        this->natnetClient.reset(nullptr);
        if (errorcode == ErrorCode_OK) {
            std::cout << "[info] <NatNetDevicePool> Successfully disconnected from NatNet server." << std::endl;
        }
        else {
            std::cout << "[info] <NatNetDevicePool> Disconnected from NatNet server - NETNET ERROR CODE: " << (int)errorcode << " (see NatNetTypes.h, line 115)" << std::endl;
        }
    }

    // Clear rigid body data after disconnecting (otherwise callback for natnet might still be accessing data).
    for (unsigned int i = 0; i < this->rigidBodies.size(); ++i) {
        this->rigidBodies[i].reset();
    }
    this->rigidBodies.clear();

    return true;
}


tracking::Quaternion tracking::NatNetDevicePool::GetOrientation(std::string& rbn) {

    tracking::Quaternion retOr((std::numeric_limits<float>::max)(),
        (std::numeric_limits<float>::max)(),
        (std::numeric_limits<float>::max)(),
        (std::numeric_limits<float>::max)());

    // Check for updated data
    if (this->callbackCounter[0] == this->callbackCounter[1]) {
        this->callbackCounter[0] = this->callbackCounter[1] = 0;
        std::cout << std::endl << "[WARNING] <NatNetDevicePool::GetOrientation> Didn't receive updated tracking data yet." << 
            " >>> Please check your firewall settings if this warning appears repeatedly!" << std::endl << std::endl;
    }

    for (auto it : this->rigidBodies) {
        if (rbn == it->name) {
            retOr = it->lockFreeData[it->read.load()].orientation;
            break;
        }
    }

    return retOr;
}


tracking::Vector3D tracking::NatNetDevicePool::GetPosition(std::string& rbn) {

    tracking::Vector3D retPos((std::numeric_limits<float>::max)(), 
        (std::numeric_limits<float>::max)(), 
        (std::numeric_limits<float>::max)());

    // Check for updated data
    if (this->callbackCounter[0] == this->callbackCounter[1]) {
        this->callbackCounter[0] = this->callbackCounter[1] = 0;
        std::cout << std::endl << "[WARNING] <NatNetDevicePool::GetPosition> Didn't receive updated tracking data yet." << 
            ">>> Please check your firewall settings if this warning appears repeatedly!" << std::endl << std::endl;
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

    for (auto it : this->rigidBodies) { 
        retval.push_back(it->name);
    }

    return retval;
}


void __cdecl tracking::NatNetDevicePool::onData(sFrameOfMocapData *pFrameOfData, void *pUserData) {

	auto that = static_cast<NatNetDevicePool *>(pUserData);
    if ((pFrameOfData == nullptr) || (that == nullptr)) {
        std::cerr << std::endl << "[ERROR] <NatNetDevicePool> Pointer to userData is NULL. " << __FILE__ << " " << __LINE__ << std::endl << std::endl;
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
        //std::cout << "[debug] <NatNetDevicePool::onData> ID = " << pFrameOfData->RigidBodies[i].ID << " MeanError = " << pFrameOfData->RigidBodies[i].MeanError <<
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
                    //std::cout << "[debug] <NatNetDevicePool::onData> READ = " << it->read.load() << " - WRTIE = " << it->write.load() << " - FREE = " << free << "." << std::endl;
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
        case (::Verbosity::Verbosity_Debug):   std::cout << "[debug] <NatNetClient> "   << message << std::endl; break;
        case (::Verbosity::Verbosity_Info):    std::cout << "[info] <NatNetClient> "    << message << std::endl; break;
        case (::Verbosity::Verbosity_Warning): std::cout << "[WARNING] <NatNetClient> " << message << std::endl; break;
        case (::Verbosity::Verbosity_Error):   std::cout << "[ERROR] <NatNetClient> "   << message << std::endl; break;
        default: break;
    }
}

