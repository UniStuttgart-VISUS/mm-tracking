/**
* TrackingUtilizer.cpp
*
* Copyright (C) 2018 by VISUS (Universitaet Stuttgart)
* Alle Rechte vorbehalten.
*/

#include "TrackingUtilizer.h"


tracking::TrackingUtilizer::TrackingUtilizer(tracking::TrackingUtilizer::Params& inUtilizerParams, std::shared_ptr<tracking::Tracker> inTrackerPtr) :
    tracker(inTrackerPtr), 
    curCameraPosition(),
    curCameraUp(),
    curCameraLookAt(),
    curOrientation(),
    curPosition(),
    curButtonStates(0),
    curSelecting(false),
    positionBuffer(),
    bufferIdx(0),
    constPosition(false)
{
    this->paramsInit(inUtilizerParams);
    this->paramsRead();
    this->paramsPrint();
}


tracking::TrackingUtilizer::~TrackingUtilizer(void) {

    // nothing to do here ...
}


void tracking::TrackingUtilizer::paramsInit(void) {
      
    this->curPosition.Set((std::numeric_limits<float>::max)(), (std::numeric_limits<float>::max)(), (std::numeric_limits<float>::max)());

    this->curIntersection.Set((std::numeric_limits<float>::max)(), (std::numeric_limits<float>::max)());

    this->curFOV.left_top.Set((std::numeric_limits<float>::max)(), (std::numeric_limits<float>::max)());
    this->curFOV.left_bottom.Set((std::numeric_limits<float>::max)(), (std::numeric_limits<float>::max)());
    this->curFOV.right_top.Set((std::numeric_limits<float>::max)(), (std::numeric_limits<float>::max)());
    this->curFOV.right_bottom.Set((std::numeric_limits<float>::max)(), (std::numeric_limits<float>::max)());

    this->buttonDeviceName  = "ControlBox";
    this->rigidBodyName     = "Stick";
    this->selectButton      = -1;
    this->rotateButton      = -1;
    this->translateButton   = -1;
    this->zoomButton        = -1;
    this->invertRotate      = true;
    this->invertTranslate   = true;
    this->invertZoom        = true;
    this->translateSpeed    = 10.0f;
    this->zoomSpeed         = 20.0f;
    this->singleInteraction = false;
    this->fovMode           = TrackingUtilizer::FovMode::WIDTH_AND_HEIGHT;
    this->fovHeight         = 0.2f;
    this->fovWidth          = 0.2f;
    this->fovHoriAngle      = 60.0f;
    this->fovVertAngle      = 30.0f;
    this->fovAspectRatio    = TrackingUtilizer::FovAspectRatio::AR_1_77__1;
}


void tracking::TrackingUtilizer::paramsInit(TrackingUtilizer::Params& p) {

    this->curPosition.Set((std::numeric_limits<float>::max)(), (std::numeric_limits<float>::max)(), (std::numeric_limits<float>::max)());

    this->curIntersection.Set((std::numeric_limits<float>::max)(), (std::numeric_limits<float>::max)());

    this->curFOV.left_top.Set((std::numeric_limits<float>::max)(), (std::numeric_limits<float>::max)());
    this->curFOV.left_bottom.Set((std::numeric_limits<float>::max)(), (std::numeric_limits<float>::max)());
    this->curFOV.right_top.Set((std::numeric_limits<float>::max)(), (std::numeric_limits<float>::max)());
    this->curFOV.right_bottom.Set((std::numeric_limits<float>::max)(), (std::numeric_limits<float>::max)());

    this->buttonDeviceName       = p.buttonDeviceName;
    this->rigidBodyName          = p.rigidBodyName;
    this->selectButton           = p.selectButton;
    this->rotateButton           = p.rotateButton;
    this->translateButton        = p.translateButton;
    this->zoomButton             = p.zoomButton;
    this->invertRotate           = p.invertRotate;
    this->invertTranslate        = p.invertTranslate;
    this->invertZoom             = p.invertZoom;
    this->translateSpeed         = p.translateSpeed;
    this->zoomSpeed              = p.zoomSpeed;
    this->singleInteraction      = p.singleInteraction;
    this->fovMode                = p.fovMode;
    this->fovHeight              = p.fovHeight;
    this->fovWidth               = p.fovWidth;
    this->fovHoriAngle           = p.fovHoriAngle;
    this->fovVertAngle           = p.fovVertAngle;
    this->fovAspectRatio         = p.fovAspectRatio;
}


void tracking::TrackingUtilizer::paramsPrint(void) {
    std::cout << "[parameter] <TrackingUtilizer> Button Device Name:      " << this->buttonDeviceName.c_str() << std::endl;
    std::cout << "[parameter] <TrackingUtilizer> Rigid Body Name:         " << this->rigidBodyName.c_str() << std::endl;
    std::cout << "[parameter] <TrackingUtilizer> Select Button:           " << this->selectButton << std::endl;
    std::cout << "[parameter] <TrackingUtilizer> Rotate Button:           " << this->rotateButton << std::endl;
    std::cout << "[parameter] <TrackingUtilizer> Translate Button:        " << this->translateButton << std::endl;
    std::cout << "[parameter] <TrackingUtilizer> Zoom Button:             " << this->zoomButton << std::endl;
    std::cout << "[parameter] <TrackingUtilizer> Invert Rotate:           " << ((this->invertRotate)?("true"):("false")) << std::endl;
    std::cout << "[parameter] <TrackingUtilizer> Invert Translate:        " << ((this->invertTranslate) ? ("true") : ("false")) << std::endl;
    std::cout << "[parameter] <TrackingUtilizer> Invert Zoom:             " << ((this->invertZoom) ? ("true") : ("false")) << std::endl;
    std::cout << "[parameter] <TrackingUtilizer> Translate Speed:         " << this->translateSpeed << std::endl;
    std::cout << "[parameter] <TrackingUtilizer> Zoom Speed:              " << this->zoomSpeed << std::endl;
    std::cout << "[parameter] <TrackingUtilizer> Single Interaction:      " << ((this->singleInteraction) ? ("true") : ("false")) << std::endl;
    std::cout << "[parameter] <TrackingUtilizer> FOV Mode:                " << (int)this->fovMode << std::endl;
    std::cout << "[parameter] <TrackingUtilizer> FOV Height:              " << this->fovHeight << std::endl;
    std::cout << "[parameter] <TrackingUtilizer> FOV Width:               " << this->fovWidth << std::endl;
    std::cout << "[parameter] <TrackingUtilizer> FOV Horizontal Angle:    " << this->fovHoriAngle << std::endl;
    std::cout << "[parameter] <TrackingUtilizer> FOV Vertical Angle:      " << this->fovVertAngle << std::endl;
    std::cout << "[parameter] <TrackingUtilizer> FOV Aspect Ratio:        " << (int)this->fovAspectRatio << std::endl;
    std::cout << "[parameter] <TrackingUtilizer> Physical Height:         " << this->physicalHeight << std::endl;
    std::cout << "[parameter] <TrackingUtilizer> Physical Width:          " << this->physicalWidth << std::endl;
    std::cout << "[parameter] <TrackingUtilizer> Physical Origin:         (" << this->physicalOrigin.X() << "," << this->physicalOrigin.Y() << "," << this->physicalOrigin.Z() << ")" << std::endl;
    std::cout << "[parameter] <TrackingUtilizer> Physical X Dir:          (" << this->physicalXDir.X() << "," << this->physicalXDir.Y() << "," << this->physicalXDir.Z() << ")" << std::endl;
    std::cout << "[parameter] <TrackingUtilizer> Physical Y Dir:          (" << this->physicalYDir.X() << "," << this->physicalYDir.Y() << "," << this->physicalYDir.Z() << ")" << std::endl;
    std::cout << "[parameter] <TrackingUtilizer> Calibration Orientation: (" << this->calibrationOrientation.X() << "," << this->calibrationOrientation.Y() << 
        "," << this->calibrationOrientation.Z() << "," << this->calibrationOrientation.W() << ")" << std::endl;
}


bool tracking::TrackingUtilizer::paramsRead(void) {

    std::string line, tag, name;
    float x, y, z, w;
    bool isCalibrationAvailable = false;

    const std::string filename = "tracking.conf";
    std::ifstream file(filename);
    if (!file.good()) {
        std::cerr << std::endl << "[ERROR] <TrackingUtilizer> Failed to open \"" << filename.c_str() << "\" for reading." << std::endl << std::endl;
    }

    unsigned int lineNmbr = 1;
    while (std::getline(file, line))
    {
        std::istringstream istream(line);
        if (!(istream >> tag)) { 
            std::cerr << std::endl << "[ERROR] <TrackingUtilizer> Can not read line tag in \"" << filename.c_str() << "\" in line number: " << lineNmbr << std::endl << std::endl;
            break; 
        }

        if (tag[0] == '#') { // Ignore comment lines
        }
        else if (tag == "PHYSICAL_SCREEN_HEIGHT") {
            if (!(istream >> x)) {
                std::cerr << std::endl << "[ERROR] <TrackingUtilizer> Can not read value for PHYSICAL_SCREEN_HEIGHT in \"" << filename.c_str() << "\" in line number: " << lineNmbr << std::endl << std::endl;
                file.close();
                break;
            }
            this->physicalHeight = x;
        }
        else if (tag == "PHYSICAL_SCREEN_WIDTH") {
            if (!(istream >> x)) {
                std::cerr << std::endl << "[ERROR] <TrackingUtilizer> Can not read value for PHYSICAL_SCREEN_WIDTH in \"" << filename.c_str() << "\" in line number: " << lineNmbr << std::endl << std::endl;
                file.close();
                break;
            }
            this->physicalWidth = x;
        }
        else if (tag == "PHYSICAL_SCREEN_ORIGIN") {
            if (!(istream >> x >> y >> z)) {
                std::cerr << std::endl << "[ERROR] <TrackingUtilizer> Can not read values for PHYSICAL_SCREEN_ORIGIN in \"" << filename.c_str() << "\" in line number: " << lineNmbr << std::endl << std::endl;
                file.close();
                break;
            }
            this->physicalOrigin.Set(x, y, z);
        }
        else if (tag == "PHYSICAL_SCREEN_X_DIR") {
            if (!(istream >> x >> y >> z)) {
                std::cerr << std::endl << "[ERROR] <TrackingUtilizer> Can not read values for PHYSICAL_SCREEN_X_DIR in \"" << filename.c_str() << "\" in line number: " << lineNmbr << std::endl << std::endl;
                file.close();
                break;
            }
            this->physicalXDir.Set(x, y, z);
        }
        else if (tag == "PHYSICAL_SCREEN_Y_DIR") {
            if (!(istream >> x >> y >> z)) {
                std::cerr << std::endl << "[ERROR] <TrackingUtilizer> Can not read values for PHYSICAL_SCREEN_Y_DIR in \"" << filename.c_str() << "\" in line number: " << lineNmbr << std::endl << std::endl;
                file.close();
                break;
            }
            this->physicalYDir.Set(x, y, z);
        }
        else if (tag == "PHYSICAL_CALIBRATION") {
            if (!(istream >> name >> x >> y >> z >> w)) {
                std::cerr << std::endl << "[ERROR] <TrackingUtilizer> Can not read values for PHYSICAL_CALIBRATION in \"" << filename.c_str() << "\" in line number: " << lineNmbr << std::endl << std::endl;
                file.close();
                break;
            }
            if (name == this->rigidBodyName) {
                this->calibrationOrientation.Set(x, y, z, w);
                isCalibrationAvailable = true;
            }
        }
        else {
            std::cerr << std::endl << "[ERROR] <TrackingUtilizer> Unknown line tag in \"" << filename.c_str() << "\"" << std::endl << std::endl;
            file.close();
            break;
        }
        lineNmbr++;
    }

    // If no calibration orientation is available, use current orientation of rigid body ...
    if (!isCalibrationAvailable) {
        this->Calibration();
        std::ofstream file(filename, std::ifstream::app);
        if (!file.good()) {
            std::cerr << std::endl << "[ERROR] <TrackingUtilizer> Failed to open \"" << filename.c_str() << "\" for writing." << std::endl << std::endl;
        }
        file << "PHYSICAL_CALIBRATION " << this->rigidBodyName.c_str() << " " << this->calibrationOrientation.X() << " " <<
            this->calibrationOrientation.Y() << " " << this->calibrationOrientation.Z() << " " << this->calibrationOrientation.W() << std::endl;
        std::cout << "[info] <TrackingUtilizer> Wrote calibration orientation to \"" << filename.c_str() << "\"" << std::endl;
    }

    file.close();

    return true;
}


bool tracking::TrackingUtilizer::paramsCheck(void) {

    bool retval  = true;
    bool changed = true;

    int  btn_min = -1;
    int  btn_max = 100;

    this->selectButton = this->limit<int>(this->selectButton, btn_min, btn_max, changed);
    if (changed) {
        std::cout << std::endl << "[WARNING] <TrackingUtilizer> Parameter \"selectButton\" must be in range ["<< btn_min << "," << btn_max << "]." << std::endl << std::endl;
        retval = false;
    }
    this->rotateButton = this->limit<int>(this->rotateButton, btn_min, btn_max, changed);
    if (changed) {
        std::cout << std::endl << "[WARNING] <TrackingUtilizer> Parameter \"rotateButton\" must be in range [" << btn_min << "," << btn_max << "]." << std::endl << std::endl;
        retval = false;
    }
    this->translateButton = this->limit<int>(this->translateButton, btn_min, btn_max, changed);
    if (changed) {
        std::cout << std::endl << "[WARNING] <TrackingUtilizer> Parameter \"translateButton\" must be in range [" << btn_min << "," << btn_max << "]." << std::endl << std::endl;
        retval = false;
    }
    this->zoomButton= this->limit<int>(this->zoomButton, btn_min, btn_max, changed);
    if (changed) {
        std::cout << std::endl << "[WARNING] <TrackingUtilizer> Parameter \"zoomButton\" must be in range [" << btn_min << "," << btn_max << "]." << std::endl << std::endl;
        retval = false;
    }
    this->translateSpeed = this->limit<float>(this->translateSpeed, 0.0f, (std::numeric_limits<float>::max)(), changed);
    if (changed) {
        std::cout << std::endl << "[WARNING] <TrackingUtilizer> Parameter \"translateSpeed\" must be in range [" << 0.0f << "," << (std::numeric_limits<float>::max)() << "]." << std::endl << std::endl;
        retval = false;
    }
    this->zoomSpeed = this->limit<float>(this->zoomSpeed, 0.0f, (std::numeric_limits<float>::max)(), changed);
    if (changed) {
        std::cout << std::endl << "[WARNING] <TrackingUtilizer> Parameter \"zoomSpeed\" must be in range [" << 0.0f << "," << (std::numeric_limits<float>::max)() << "]." << std::endl << std::endl;
        retval = false;
    }
    this->fovHeight = this->limit<float>(this->fovHeight, 0.0f, (std::numeric_limits<float>::max)(), changed);
    if (changed) {
        std::cout << std::endl << "[WARNING] <TrackingUtilizer> Parameter \"fovHeight\" must be in range [" << 0.0f << "," << (std::numeric_limits<float>::max)() << "]." << std::endl << std::endl;
        retval = false;
    }
    this->fovWidth = this->limit<float>(this->fovWidth, 0.0f, (std::numeric_limits<float>::max)(), changed);
    if (changed) {
        std::cout << std::endl << "[WARNING] <TrackingUtilizer> Parameter \"fovWidth\" must be in range [" << 0.0f << "," << (std::numeric_limits<float>::max)() << "]." << std::endl << std::endl;
        retval = false;
    }
    this->fovHoriAngle = this->limit<float>(this->fovHoriAngle, 0.0f, 180.0f, changed);
    if (changed) {
        std::cout << std::endl << "[WARNING] <TrackingUtilizer> Parameter \"fovHoriAngle\" must be in range [" << 0.0f << "," << 180.0f << "]." << std::endl << std::endl;
        retval = false;
    }
    this->fovVertAngle = this->limit<float>(this->fovVertAngle, 0.0f, 180.0f, changed);
    if (changed) {
        std::cout << std::endl << "[WARNING] <TrackingUtilizer> Parameter \"fovVertAngle\" must be in range [" << 0.0f << "," << 180.0f << "]." << std::endl << std::endl;
        retval = false;
    }
    this->physicalHeight = this->limit<float>(this->physicalHeight, 0.0f, (std::numeric_limits<float>::max)(), changed);
    if (changed) {
        std::cout << std::endl << "[WARNING] <TrackingUtilizer> Parameter \"physicalHeight\" must be in range [" << 0.0f << "," << (std::numeric_limits<float>::max)() << "]." << std::endl << std::endl;
        retval = false;
    }
    this->physicalWidth = this->limit<float>(this->physicalWidth, 0.0f, (std::numeric_limits<float>::max)(), changed);
    if (changed) {
        std::cout << std::endl << "[WARNING] <TrackingUtilizer> Parameter \"physicalWidth\" must be in range [" << 0.0f << "," << (std::numeric_limits<float>::max)() << "]." << std::endl << std::endl;
        retval = false;
    }

    /// No check necessary for:
    //this->buttonDeviceName
    //this->rigidBodyName
    //this->invertRotate
    //this->invertTranslate
    //this->invertZoom
    //this->fovMode
    //this->fovAspectRatio
    // this->singleInteraction
    //this->physicalorigin
    //this->physicalxdir
    //this->physicalydir
    //this->calibrationorientation

    return retval;
}


bool tracking::TrackingUtilizer::GetRawData(tracking::ButtonMask& outBtnState, tracking::Vector3D& outPos, tracking::Quaternion& outOri) {

    bool stateRawData = false;

    // Request updated tracking data.
    if (this->updateTrackingData()) {

        outBtnState = this->curButtonStates;
        outPos      = this->curPosition;
        outOri      = this->curOrientation;

        stateRawData = true;
    }
    else {
        outBtnState = 0;
        outPos.Set((std::numeric_limits<float>::max)(), (std::numeric_limits<float>::max)(), (std::numeric_limits<float>::max)());
        outOri.Set(0.0f, 0.0f, 0.0f, 1.0f);
    }

    return (stateRawData);
}


bool tracking::TrackingUtilizer::GetSelectionState(bool& outSelect) {

    bool stateBtnChgs = false;

    // Request updated tracking data.
    if (this->updateTrackingData()) {

        // Check for current button interaction.
        stateBtnChgs = this->processButtonChanges();
    }

    if (stateBtnChgs) {
        outSelect = this->curSelecting;
    }
    else {
        outSelect = false;
    }

    return (stateBtnChgs);
}


bool tracking::TrackingUtilizer::GetIntersection(tracking::Point2D& outIntersect) {

    bool stateSrnIns = false;

    // Request updated tracking data.
    if (this->updateTrackingData()) {

        // Calculate new current intersection Point2D and fov rectangle.
        stateSrnIns = this->processScreenInteraction(false);
    }

    if (stateSrnIns) {
        outIntersect = this->curIntersection;
    }
    else {
        outIntersect.Set((std::numeric_limits<float>::max)(), (std::numeric_limits<float>::max)());
    }

    return (stateSrnIns);
}


bool tracking::TrackingUtilizer::GetFieldOfView(tracking::Rectangle& outFov) {

    bool stateSrnIns = false;

    // Request updated tracking data.
    if (this->updateTrackingData()) {

        // Calculate new current intersection point and fov rectangle.
        stateSrnIns = this->processScreenInteraction(true);
    }

    if (stateSrnIns) {
        outFov = this->curFOV;
    }
    else {
        outFov.left_top.Set((std::numeric_limits<float>::max)(), (std::numeric_limits<float>::max)());
        outFov.left_bottom.Set((std::numeric_limits<float>::max)(), (std::numeric_limits<float>::max)());
        outFov.right_top.Set((std::numeric_limits<float>::max)(), (std::numeric_limits<float>::max)());
        outFov.right_bottom.Set((std::numeric_limits<float>::max)(), (std::numeric_limits<float>::max)());
    }

    return (stateSrnIns);
}


bool tracking::TrackingUtilizer::GetUpdatedCamera(tracking::TrackingUtilizer::Dim dim, tracking::Vector3D& outCamPos, 
    tracking::Vector3D& outCamLookAt, tracking::Vector3D& outCamUp) {

    bool stateBtnChgs = false;
    bool stateCamTrs  = false;

    // Request updated tracking data.
    if (this->updateTrackingData()) {

        // Check for current button interaction.
        stateBtnChgs = this->processButtonChanges();

        // Apply camera transformations enabled by pressed buttons.
        switch (dim) {
            case(TrackingUtilizer::Dim::DIM_2D): stateCamTrs = this->processCameraTransformations2D(); break;
            case(TrackingUtilizer::Dim::DIM_3D): stateCamTrs = this->processCameraTransformations3D(); break;
            default: break;
        }
    }

    if (stateBtnChgs && stateCamTrs) {
        outCamPos    = this->curCameraPosition;
        outCamLookAt = this->curCameraLookAt;
        outCamUp     = this->curCameraUp;
    }
    else {
        outCamPos.Set((std::numeric_limits<float>::max)(), (std::numeric_limits<float>::max)(), (std::numeric_limits<float>::max)());
        outCamLookAt.Set((std::numeric_limits<float>::max)(), (std::numeric_limits<float>::max)(), (std::numeric_limits<float>::max)());
        outCamUp.Set((std::numeric_limits<float>::max)(), (std::numeric_limits<float>::max)(), (std::numeric_limits<float>::max)());
    }

    return (stateBtnChgs && stateCamTrs);
}


bool tracking::TrackingUtilizer::Calibration(void) {

    bool stateCalib = false;

    // Request updated tracking data.
    if (this->updateTrackingData()) {
        // Store current rigid body orientation as calibration orientation.
        this->calibrationOrientation = this->curOrientation;
        stateCalib = true;
    }
    else {
        this->calibrationOrientation.Set(0.0f, 0.0f, 0.0f, 1.0f);
    }

    std::cout << "[info] <TrackingUtilizer> >>> RIGID BODY \"" << this->rigidBodyName.c_str() << "\" CALIBRATION ORIENTATION " <<
        this->calibrationOrientation.X() << ";" << this->calibrationOrientation.Y() << ";" <<
        this->calibrationOrientation.Z() << ";" << this->calibrationOrientation.W() << std::endl;

    return stateCalib;
}


bool tracking::TrackingUtilizer::SetCurrentCamera(tracking::Vector3D inCamPos, tracking::Vector3D inCamLookAt, tracking::Vector3D inCamUp) {

    this->curCameraPosition = inCamPos;
    this->curCameraLookAt   = inCamLookAt;
    this->curCameraUp       = inCamUp;

    return true;
}


bool tracking::TrackingUtilizer::updateTrackingData(void) {

    if (this->tracker == nullptr) {
        std::cerr << std::endl << "[ERROR] <TrackingUtilizer> There is no tracker connected." << std::endl << std::endl;
        return false;
    }

    // Check for valid parameters
    this->paramsCheck();

    // Get fresh data from tracker
    bool retval = false;
    tracking::Tracker::TrackingData data;
    if (this->tracker->GetData(this->rigidBodyName, this->buttonDeviceName, data)) {
        this->curButtonStates = data.buttonState;
        this->curPosition     = data.rigidBody.position;
        this->curOrientation  = data.rigidBody.orientation;

#ifdef TRACKING_DEBUG_OUTPUT
        std::cout << "[debug] <TrackingUtilizer> Buffer Index = " << bufferIdx << " - Position Buffer Size = " << this->positionBuffer.size() << std::endl;
#endif

        // Add current position to position buffer
        const unsigned int bufferSize = 10; // ! Must be greater than 0 !
        this->positionBuffer.resize(bufferSize);
        this->positionBuffer[this->bufferIdx] = this->curPosition;
        this->bufferIdx = (++this->bufferIdx) % bufferSize;
        // Check if all positions are the same which indicates that rigid body is no longer tracked ...
        this->constPosition = true;
        for (size_t i = 0; i < this->positionBuffer.size() - 1; ++i) {
            if (this->positionBuffer[i] != this->positionBuffer[i+1]) {
                this->constPosition = false;
                break;
            }
        }
        retval = true;
    }

#ifdef TRACKING_DEBUG_OUTPUT
    std::cout << "[debug] <TrackingUtilizer> Position = (" << this->curPosition.X() << ", " << this->curPosition.Y() << ", " << this->curPosition.Z() << "), Orientation = (" <<
        this->curOrientation.X() << ", " << this->curOrientation.Y() << ", " << this->curOrientation.Z() << ", " << this->curOrientation.W() << ")." << std::endl;
#endif
      
    return retval;
}


bool tracking::TrackingUtilizer::processButtonChanges(void) {

    if (this->constPosition) {
#ifdef TRACKING_DEBUG_OUTPUT
        std::cout << "[debug] <TrackingUtilizer> Rigid body \"" << this->rigidBodyName.c_str() << "\" is not inside tracking area. " << std::endl;
#endif
        return false;
    }

    bool retval = true;
    if (this->lastButtonStates != this->curButtonStates) {

        // Detect reconfiguration of button mapping.
        this->isRotating    = false;
        this->isTranslating = false;
        this->isZooming     = false;
        this->curSelecting  = false;

        // Loop over all buttons 
        int button = 0;
        bool isPressed = false;

        for (int i = 0; i < 4; ++i) { 

            switch (i) {
            case(0): button = this->rotateButton;
                     isPressed = ((this->curButtonStates & (1 << button)) != 0)?(true):(false);
                     this->isRotating = isPressed;
                     std::cout << "[info] <TrackingUtilizer> Rotate Button " << button << " is " << ((this->isRotating) ? ("PRESSED") : ("released")) << "." << std::endl;
                     break;
            case(1): button = this->translateButton;
                     isPressed = ((this->curButtonStates & (1 << button)) != 0) ? (true) : (false);
                     this->isTranslating = isPressed;
                     std::cout << "[info] <TrackingUtilizer> Translate Button " << button << " is " << ((this->isTranslating) ? ("PRESSED") : ("released")) << "." << std::endl;
                     break;
            case(2): button = this->zoomButton;
                     isPressed = ((this->curButtonStates & (1 << button)) != 0) ? (true) : (false);
                     this->isZooming = isPressed;
                     std::cout << "[info] <TrackingUtilizer> Zoom Button " << button << " is " << ((this->isZooming) ? ("PRESSED") : ("released")) << "." << std::endl;
                     break;
            case(3): button = this->selectButton;
                     this->curSelecting = ((this->curButtonStates & (1 << button)) != 0) ? (true) : (false);
                     isPressed = false;  // Current configuration has not to be stored ....
                     std::cout << "[info] <TrackingUtilizer> Select Button " << button << " is " << ((this->curSelecting) ? ("PRESSED") : ("released")) << "." << std::endl;
                     break;
            }

            // Remember initial configuration and determine what to do.
            if (isPressed) {
                // Preserve the current camera system as we need it later to perform
                // incremental transformations.
                this->startCamLookAt   = this->curCameraLookAt;
                this->startCamPosition = this->curCameraPosition;
                this->startCamUp       = this->curCameraUp;

                // Relative orientation of the stick with respect to the current camera 
                // coordinate system. This is required to align the interaction device 
                // with the current view later on.

                auto q1 = this->xform(tracking::Vector3D(0, 0, 1), this->startCamPosition - this->startCamLookAt);
                auto q2 = this->xform(q1 * tracking::Vector3D(0, 1, 0), this->startCamUp);
                this->startRelativeOrientation = q2 * q1;

                this->startOrientation         = this->curOrientation;
                this->startPosition            = this->curPosition;
            }
        }

        this->lastButtonStates = this->curButtonStates;
    }

    return retval;
}


bool tracking::TrackingUtilizer::processCameraTransformations3D(void) {

    if (this->constPosition) {
#ifdef TRACKING_DEBUG_OUTPUT
        std::cout << "[debug] <TrackingUtilizer> Rigid body \"" << this->rigidBodyName.c_str() << "\" is not inside tracking area. " << std::endl;
#endif
        return false;
    }

    bool retval = false;
    int  xact   = 0;

    if (this->isRotating) {
        ++xact;
    }
    if (this->isTranslating) {
        ++xact;
    }
    if (this->isZooming) {
        ++xact;
    }
    if (this->singleInteraction && (xact > 1)) {
        xact = 0;
    }

    if (xact > 0) {

        if (this->isRotating) {
#ifdef TRACKING_DEBUG_OUTPUT
            std::cout << "[debug] <TrackingUtilizer> Apply 6DOF ROTATION." << std::endl;
#endif
            // Compute relative rotation since button was pressed.
            auto quat = this->curOrientation * this->startOrientation.Inverse();
            quat.Normalise();

            // Align interaction with the original camera system.
            auto relConj = this->startRelativeOrientation;
            relConj.Conjugate();
            quat = this->startRelativeOrientation * quat * relConj;

            // Optionally, apply the Reina factor.
            if (this->invertRotate) {
                quat.Invert();
            }
            else {
                quat.Normalise();
            }

            // Apply rotation.
            tracking::Vector3D startView = this->startCamPosition - this->startCamLookAt;
            tracking::Vector3D up        = quat * this->startCamUp;
            tracking::Vector3D view      = quat * startView;

            // Apply new view parameters.
            this->curCameraPosition = this->startCamLookAt + view;
            this->curCameraLookAt   = this->startCamLookAt;
            this->curCameraUp       = up;

            retval = true;
        }

        if (this->isTranslating) {
#ifdef TRACKING_DEBUG_OUTPUT
            std::cout << "[debug] <TrackingUtilizer> Apply 6DOF TRANSLATION." << std::endl;
#endif
            // Compute relative movement of tracker in physical space.
            auto delta = this->curPosition -this->startPosition;

            // Align interaction with the original camera system.
            delta = this->startRelativeOrientation * delta;

            // Scale into virtual space.
            delta *= this->translateSpeed;

            if (this->invertTranslate) {
                delta *= (-1.0f);
            }

            // Apply new translated position.
            this->curCameraPosition = this->startCamPosition + delta;
            this->curCameraLookAt   = this->startCamLookAt + delta;

            retval = true;
        }

        if (this->isZooming) {
#ifdef TRACKING_DEBUG_OUTPUT
            std::cout << "[debug] <TrackingUtilizer> Apply 6DOF ZOOM." << std::endl;
#endif
            // Compute relative movement of tracker in physical space.
            auto diff = this->curPosition - this->startPosition;

            // Compute the distance in virtual space that we move the camera.
            auto delta = diff.Z() * this->zoomSpeed;

            auto view = this->startCamPosition - this->startCamLookAt;
            view.Normalise();

            view *= delta;

            if (this->invertZoom) {
                view *= (-1.0f);
            }

            // Apply new zoomed position.
            this->curCameraPosition = this->startCamPosition + view;

            retval = true;
        }
    }

    return retval;
}


bool tracking::TrackingUtilizer::processCameraTransformations2D(void) {

    if (this->constPosition) {
#ifdef TRACKING_DEBUG_OUTPUT
        std::cout << "[debug] <TrackingUtilizer> Rigid body \"" << this->rigidBodyName.c_str() << "\" is not inside tracking area. " << std::endl;
#endif
        return false;
    }

    bool retval = false;
    int  xact = 0;

    if (this->isRotating) {
        ++xact;
    }
    if (this->isTranslating) {
        ++xact;
    }
    if (this->isZooming) {
        ++xact;
    }
    if (this->singleInteraction && (xact > 1)) {
        xact = 0;
    }

    if (xact > 0) {

        std::cerr << std::endl << "[ERROR] <TrackingUtilizer> 2D camera transformation is not yet implemented ...." << std::endl << std::endl;

/** TODO for 2D ...

        if (this->isRotating) {
#ifdef TRACKING_DEBUG_OUTPUT
            std::cout << "[debug] <TrackingUtilizer> Apply 6DOF ROTATION." << std::endl;
#endif
            // Compute relative rotation since button was pressed.
            auto quat = this->curOrientation * this->startOrientation.Inverse();
            quat.Normalise();

            // Align interaction with the original camera system.
            auto relConj = this->startRelativeOrientation;
            relConj.Conjugate();
            quat = this->startRelativeOrientation * quat * relConj;

            // Optionally, apply the Reina factor.
            if (this->invertRotate) {
                quat.Invert();
            }
            else {
                quat.Normalise();
            }

            // Apply rotation.
            tracking::Vector3D startView = this->startCamPosition - this->startCamLookAt;
            tracking::Vector3D up = quat * this->startCamUp;
            tracking::Vector3D view = quat * startView;

            // Apply new view parameters.
            this->curCameraPosition = this->startCamLookAt + view;
            this->curCameraLookAt = this->startCamLookAt;
            this->curCameraUp = up;

            retval = true;
        }

        if (this->isTranslating) {
#ifdef TRACKING_DEBUG_OUTPUT
            std::cout << "[debug] <TrackingUtilizer> Apply 6DOF TRANSLATION." << std::endl;
#endif
            // Compute relative movement of tracker in physical space.
            auto delta = this->curPosition - this->startPosition;

            // Align interaction with the original camera system.
            delta = this->startRelativeOrientation * delta;

            // Scale into virtual space.
            delta *= this->translateSpeed;

            if (this->invertTranslate) {
                delta *= (-1.0f);
            }

            // Apply new translated position.
            this->curCameraPosition = this->startCamPosition + delta;
            this->curCameraLookAt = this->startCamLookAt + delta;

            retval = true;
        }

        if (this->isZooming) {
#ifdef TRACKING_DEBUG_OUTPUT
            std::cout << "[debug] <TrackingUtilizer> Apply 6DOF ZOOM." << std::endl;
#endif
            // Compute relative movement of tracker in physical space.
            auto diff = this->curPosition - this->startPosition;

            // Compute the distance in virtual space that we move the camera.
            auto delta = diff.Z() * this->zoomSpeed;

            auto view = this->startCamPosition - this->startCamLookAt;
            view.Normalise();

            view *= delta;

            if (this->invertZoom) {
                view *= (-1.0f);
            }

            // Apply new zoomed position.
            this->curCameraPosition = this->startCamPosition + view;

            retval = true;
        }
*/

    }
    return retval;
}


bool tracking::TrackingUtilizer::processScreenInteraction(bool processFov) {

    if (this->constPosition) {
#ifdef TRACKING_DEBUG_OUTPUT
        std::cout << "[debug] <TrackingUtilizer> Rigid body \"" << this->rigidBodyName.c_str() << "\" is not inside tracking area. " << std::endl;
#endif
        return false;
    }

    const double PI = 3.1415926535897;

    bool retval = false;
    this->curIntersection.Set((std::numeric_limits<float>::max)(), (std::numeric_limits<float>::max)());
    this->curFOV.left_top.Set((std::numeric_limits<float>::max)(), (std::numeric_limits<float>::max)());
    this->curFOV.left_bottom.Set((std::numeric_limits<float>::max)(), (std::numeric_limits<float>::max)());
    this->curFOV.right_top.Set((std::numeric_limits<float>::max)(), (std::numeric_limits<float>::max)());
    this->curFOV.right_bottom.Set((std::numeric_limits<float>::max)(), (std::numeric_limits<float>::max)());

    // Variable PREFIX naming convention:
    // p  = powerwall
    // c  = calibration
    // rb = rigid body 

    // Variable INFIX naming convention:
    // W = width
    // H = height
    // O = origin 
    // D = look at direction
    // U = up direction
    // R = right direction

    // Variable POSTFIX naming convention:
    // v = Vector3D
    // c = calibration
    // f = float 
    // q = quaternion

    // Physical width and height of screen
    auto pHf = this->physicalHeight; // In meters
    auto pWf = this->physicalWidth; // In meters

    if ((pHf > 0.0f) && (pWf > 0.0f)) {

        // Get normal Point2Ding out of the screen
        auto pWv = this->physicalXDir;
        pWv.Normalise();
        auto pHv = this->physicalYDir;
        pHv.Normalise();
        auto pNv = pWv.Cross(pHv); // Physical screen normal of screen
        pNv.Normalise();

        // Absolut offset between origin of tracking coordinate system (= Vector3D<float,3>(0,0,0)) and lower left corner of screen.
        // (= distance Vector3D from tracking origin to lower left corner of screen).
        auto pOv = this->physicalOrigin; // In meters

        // Get calibration orientation of Point2Ding device.
        // This orientation corresponds to physical calibration direction.
        auto cOv = this->calibrationOrientation;
        auto cOq  = Quaternion(cOv.X(), cOv.Y(), cOv.Z(), cOv.W());

        // Calculate physical direction (look at) of rigid body 
        // (Physical calibration direction is opposite direction of screen normal)
        auto rbDv = this->curOrientation * cOq.Inverse() * ((-1.0f) * pNv);
        rbDv.Normalise();
       
#ifdef TRACKING_DEBUG_OUTPUT
        std::cout << "[debug] <TrackingUtilizer> Rigid body position (" <<  this->curPosition.X() << "," << this->curPosition.Y() << "," << this->curPosition.Z() << 
            ", orientation (" << this->curOrientation.X() << "," << this->curOrientation.Y() << "," << this->curOrientation.Z() << "," << this->curOrientation.W() << ")" << std::endl;
        std::cout << "[debug] <TrackingUtilizer> Rigid body Point2Ding direction (" << rbDv.X() << "," << rbDv.Y() << "," << rbDv.Z() << ")" << std::endl;
        auto urC = pOv + (pWv * pWf) + (pHv * pHf); // upper right corner
        std::cout << "[debug] <TrackingUtilizer> Screen spans from (" << pOv.X() << "," << pOv.Y() << "," << pOv.Z()<< " to " << urC.X() << "," << urC.Y() << "," << urC.Z() << ")" << std::endl;
        std::cout << "[debug] <TrackingUtilizer> Screen normal (" << pNv.X() << "," << pNv.Y() << "," << pNv.Z() << ")" << std::endl;
#endif
        // Intersection is only possible if screen normal and the Point2Ding direction are in opposite direction.
        if (pNv.Dot(rbDv) < 0.0f) { // pN.Dot(pDir) must be != 0 anyway ...

            // --- Calculate screen intersection ------------------------------

            auto rbRelPosv = this->curPosition - pOv; // Realtive position to origin of screen (lower left corner)
            auto delta = pNv.Dot(rbRelPosv) / pNv.Dot(((-1.0f) * rbDv));  //Using first intercept theorem ...
            auto rbIs = this->curPosition + delta * rbDv;
#ifdef TRACKING_DEBUG_OUTPUT
            std::cout << "[debug] <TrackingUtilizer> Intersects at (" << rbIs.X() << "," << rbIs.Y() << "," << rbIs.Z() << ")" << std::endl;
#endif
            // Intersection in regard to powerwall origin
            auto pIs = rbIs - pOv;
            // Scaling to relative coordinates in [0,1].
            float x = (pWv.Dot(pIs)) / pWf;
            float y = (pHv.Dot(pIs)) / pHf;
#ifdef TRACKING_DEBUG_OUTPUT
            std::cout << "[debug] <TrackingUtilizer> Relative intersection at (" << x << "," << y << ")" << std::endl;
#endif
            // Ensure that intersection lies on physical screen (inside of area spanned by pWv and pHv)
            // Recognise only intersections insides of powerwall screen 
            // (not recommanded because field of view disappears suddenly when intersection center lies outside of screen)
            if ((x >= 0.0f) && (y >= 0.0f) && (x <= 1.0f) && (y <= 1.0f)) {
                //VLTRACE(vislib::Trace::LEVEL_INFO, "Intersection inside of screen. \n");
                this->curIntersection.Set(x, y);
                retval = true;
            }

            // --- Calculate field of view square projected on screen ---------
            if (processFov) {
                float fov_w  = this->fovWidth;
                float fov_h  = this->fovHeight;
                float fov_ah = this->fovHoriAngle / 2.0f; // already using half angle!
                float fov_av = this->fovVertAngle / 2.0f; // already using half angle!
                float fov_r  = 1.0f;
                switch ((int)this->fovAspectRatio) {
                    case (0): fov_r = 2.35f;         break;
                    case (1): fov_r = 1.85f;         break;
                    case (2): fov_r = 16.0f / 9.0f;  break;
                    case (3): fov_r = 1.6f;          break;
                    case (4): fov_r = 4.0f / 3.0f;   break;
                    case (5): fov_r = 1.0f;          break;
                    default: break;
                }

                // Fix scaling for aspect ratio of powerwall screen resolution
                float pRatiof = pWf / pHf;

                // Rigid body up and right Vector3D.
                Vector3D rbUv, rbRv;
                float deltaR, deltaU;
                std::vector<Vector3D> fovVert;
                fovVert.clear();

                if ((this->fovMode == 0) || (this->fovMode == 1)) { 
                    // Width and Height - Width and Aspect Ratio
                                  
                    float xDelta = (fov_w / 2.0f);
                    float yDelta = (fov_h * pRatiof / 2.0f);
                    if (this->fovMode == 1) { /// DIFFERENCE for Width and Aspect Ratio
                        yDelta = (fov_w / fov_r * pRatiof / 2.0f);
                    }

                    fovVert.push_back((pIs - (pWv * xDelta)) + (pHv * yDelta)); // left top
                    fovVert.push_back((pIs - (pWv * xDelta)) - (pHv * yDelta)); // left bottom
                    fovVert.push_back((pIs + (pWv * xDelta)) + (pHv * yDelta)); // right top
                    fovVert.push_back((pIs + (pWv * xDelta)) - (pHv * yDelta)); // right bottom

                    // Calculate intersection of fov vertices with screen
                    for (int i = 0; i < fovVert.size(); ++i) {
                        // Scaling to relative coordinate in [0,1].
                        x = (pWv.Dot(fovVert[i])) / pWf;
                        y = (pHv.Dot(fovVert[i])) / pHf;
                        switch (i) {
                        case(0): // left top
                            this->curFOV.left_top.Set(x, y); break;
                        case(1): // left bottom
                            this->curFOV.left_bottom.Set(x, y); break;
                        case(2): // right top
                            this->curFOV.right_top.Set(x, y); break;
                        case(3): // right bottom
                            this->curFOV.right_bottom.Set(x, y); break;
                        }
                    }
                }
                else if ((this->fovMode == 2) || (this->fovMode == 3)) { 
                    // Horizontal Angle and Vertical Angle - Horizontal Angle and Aspect Ratio

                    // Current 'up' Vector3D of rigid body is equal to "heigth Vector3D" of powerwall.
                    rbUv = this->curOrientation * cOq.Inverse() * pHv;
                    rbUv.Normalise();
                    // Current 'right' Vector3D of rigid body is equal to "width Vector3D" of powerwall.
                    rbRv = this->curOrientation * cOq.Inverse() * pWv;
                    rbRv.Normalise();
                    // Calculate length of right Vector3D for given horizontal angle in distance of normalized looak at Vector3D.
                    deltaR = (float)std::tan((double)fov_ah / 180.0 * PI); // * 1.0f = Length of rbDv

                    // Calculate length of 'up' Vector3D for given vertical angle in distance of normalized looak at Vector3D.
                    deltaU = (float)std::tan((double)fov_av / 180.0 * PI); // * 1.0f = Length of rbDv
                    if (this->fovMode == 3) { /// DIFFERENCE for Horizontal Angle and Aspect Ratio
                        deltaU = deltaR / fov_r;
                    }

                    fovVert.push_back((rbDv - (rbRv * deltaR)) + (rbUv * deltaU)); // left top
                    fovVert.push_back((rbDv - (rbRv * deltaR)) - (rbUv * deltaU)); // left bottom
                    fovVert.push_back((rbDv + (rbRv * deltaR)) + (rbUv * deltaU)); // right top
                    fovVert.push_back((rbDv + (rbRv * deltaR)) - (rbUv * deltaU)); // right bottom

                    // Calculate intersection of fov vertices with screen
                    for (int i = 0; i < fovVert.size(); ++i) {
                        fovVert[i].Normalise();
                        //rbRelPosv didn't change ...
                        delta = pNv.Dot(rbRelPosv) / pNv.Dot((-1.0f * fovVert[i]));  //Using first intercept theorem ...
                                                                                     //ToFIX: delta switches to negativ values before(?) normal of screen and look at direction are orthogonal ...
                        delta = (delta < 0.0f) ? (500.0f) : (delta); // the 500 is empirical
                        rbIs = this->curPosition + delta * fovVert[i];
                        // Intersection in regard to powerwall origin
                        pIs = rbIs - pOv;
                        // Scaling to relative coordinate in [0,1].
                        x = (pWv.Dot(pIs)) / pWf;
                        y = (pHv.Dot(pIs)) / pHf;
                        switch (i) {
                        case(0): // left top
                            this->curFOV.left_top.Set(x, y); break;
                        case(1): // left bottom
                            this->curFOV.left_bottom.Set(x, y); break;
                        case(2): // right top
                            this->curFOV.right_top.Set(x, y); break;
                        case(3): // right bottom
                            this->curFOV.right_bottom.Set(x, y); break;
                        }
                    }
                }

                // Limit fov coords to dimensions of screen
                float min = 0.0f;
                float max = 1.0f;
                this->curFOV.left_top.SetX(this->limit<float>(this->curFOV.left_top.X(), min, max));
                this->curFOV.left_top.SetY(this->limit<float>(this->curFOV.left_top.Y(), min, max));
                this->curFOV.left_bottom.SetX(this->limit<float>(this->curFOV.left_bottom.X(), min, max));
                this->curFOV.left_bottom.SetY(this->limit<float>(this->curFOV.left_bottom.Y(), min, max));
                this->curFOV.right_top.SetX(this->limit<float>(this->curFOV.right_top.X(), min, max));
                this->curFOV.right_top.SetY(this->limit<float>(this->curFOV.right_top.Y(), min, max));
                this->curFOV.right_bottom.SetX(this->limit<float>(this->curFOV.right_bottom.X(), min, max));
                this->curFOV.right_bottom.SetY(this->limit<float>(this->curFOV.right_bottom.Y(), min, max));

                // Check if fov lies completely outside of screen
                retval = true;
                if (((this->curFOV.right_top.X() <= min) && (this->curFOV.right_bottom.X() <= min) && (this->curFOV.left_top.X() <= min) && (this->curFOV.left_bottom.X() <= min)) ||
                    ((this->curFOV.right_top.X() >= max) && (this->curFOV.right_bottom.X() >= max) && (this->curFOV.left_top.X() >= max) && (this->curFOV.left_bottom.X() >= max)) ||
                    ((this->curFOV.right_top.Y() <= min) && (this->curFOV.right_bottom.Y() <= min) && (this->curFOV.left_top.Y() <= min) && (this->curFOV.left_bottom.Y() <= min)) ||
                    ((this->curFOV.right_top.Y() >= max) && (this->curFOV.right_bottom.Y() >= max) && (this->curFOV.left_top.Y() >= max) && (this->curFOV.left_bottom.Y() >= max)))
                {
                    retval = false;
                    this->curFOV.left_top.Set(    (std::numeric_limits<float>::max)(), (std::numeric_limits<float>::max)());
                    this->curFOV.left_bottom.Set( (std::numeric_limits<float>::max)(), (std::numeric_limits<float>::max)());
                    this->curFOV.right_top.Set(   (std::numeric_limits<float>::max)(), (std::numeric_limits<float>::max)());
                    this->curFOV.right_bottom.Set((std::numeric_limits<float>::max)(), (std::numeric_limits<float>::max)());
                }

#ifdef TRACKING_DEBUG_OUTPUT
                std::cout << "[debug] <TrackingUtilizer> Relative FIELD OF VIEW coordinates: LEFT_TOP (" <<
                    this->curFOV.left_top.X() << "," << this->curFOV.left_top.Y() << ") | LEFT_BOTTOM (" <<
                    this->curFOV.left_bottom.X() << "," << this->curFOV.left_bottom.Y() << ") | RIGHT_TOP (" <<
                    this->curFOV.right_top.X() << "," << this->curFOV.right_top.Y() << ") | RIGHT_BOTTOM (" <<
                    this->curFOV.right_bottom.X() << "," << this->curFOV.right_bottom.Y() << ")" << std::endl;
#endif
            }
        }
    }

    return retval;
}


template<typename T>
T tracking::TrackingUtilizer::limit(T val, T min, T max) {

    T retval = val;

    retval = (val < min) ? (min) : (val);
    retval = (retval > max) ? (max) : (retval);

    return retval;
}


template<typename T>
T tracking::TrackingUtilizer::limit(T val, T min, T max, bool& changed) {

    T newval = limit<T>(val, min, max);
    changed = ((val == newval) ? (false) : (true));

    return newval;
}


tracking::Quaternion tracking::TrackingUtilizer::xform(const tracking::Vector3D& u, const tracking::Vector3D& v) {

    tracking::Quaternion retQuat;

    /// http://lolengine.net/blog/2013/09/18/beautiful-maths-quaternion-from-vectors
    auto w = u.Cross(v);
    auto q = tracking::Quaternion(w.X(), w.Y(), w.Z(), u.Dot(v));
    q.SetW(q.W() + q.Norm());
    q.Normalise();
    return q;

    return retQuat;
}


const std::string tracking::TrackingUtilizer::cn(void) const {
    std::string tmp = typeid(*this).name();
    return tmp.substr(tmp.find_last_of(':') + 1, tmp.length() - 1);
}
