/**
* TrackingUtilizer.cpp
*
* Copyright (C) 2018 by VISUS (Universitaet Stuttgart)
* Alle Rechte vorbehalten.
*/

#include "TrackingUtilizer.h"


tracking::TrackingUtilizer::TrackingUtilizer(void) 
    : initialised(false)
    , tracker(nullptr)
    , curCameraPosition()
    , curCameraUp()
    , curCameraView()
    , curCameraCenterDist()
    , curIntersection()
    , curFOV()
    , curOrientation()
    , curPosition()
    , curButton()
    , curSelecting(false)
    , positionBuffer()
    , bufferIdx(0)
    , constPosition(false)
    , lastButton(0)
    , startCamView()
    , startCamPosition()
    , startCamUp()
    , startCamCenterDist()
    , startPosition()
    , startOrientation()
    , startRelativeOrientation()
    , isRotating(false)
    , isTranslating(false)
    , isZooming(false)
    , buttonDeviceName("ControlBox")
    , rigidBodyName("Stick")
    , selectButton(0)
    , rotateButton(0)
    , translateButton(0)
    , zoomButton(0)
    , invertRotate(true)
    , invertTranslate(true)
    , invertZoom(true)
    , translateSpeed(10.0f)
    , zoomSpeed(20.0f)
    , singleInteraction(false)
    , fovMode(TrackingUtilizer::FovMode::WIDTH_AND_HEIGHT)
    , fovHeight(0.2f)
    , fovWidth(0.2f)
    , fovHoriAngle(60.0f)
    , fovVertAngle(30.0f)
    , fovAspectRatio(TrackingUtilizer::FovAspectRatio::AR_1_77__1) 
    , physicalHeight(2.4f)
    , physicalWidth(6.0f)
    , calibrationOrientation()
    , physicalOrigin(-3.0f, 0.3f, 0.0f)
    , physicalXDir(1.0f, 0.0f, 0.0f)
    , physicalYDir(0.0f, 1.0f, 0.0f) {

    // intentionally empty...
}


tracking::TrackingUtilizer::~TrackingUtilizer(void) {

    // nothing to do here ...
}


bool tracking::TrackingUtilizer::Initialise(const tracking::TrackingUtilizer::Params & params, std::shared_ptr<tracking::Tracker> tracker) {

    bool check = true;
    this->initialised = false;

    if (tracker == nullptr) {
        std::cerr << std::endl << "[ERROR] [TrackingUtilizer] Pointer to tracker is nullptr. " <<
            "[" << __FILE__ << ", " << __FUNCTION__ << ", line " << __LINE__ << "]" << std::endl << std::endl;
        return false;
    }
    this->tracker = tracker;

    std::string btn_device_name;
    try {
        btn_device_name = std::string(params.btn_device_name);
        if (btn_device_name.length() != params.btn_device_name_len) {
            std::cerr << std::endl << "[ERROR] [TrackingUtilizer] String \"btn_device_name\" has not expected length. " <<
                "[" << __FILE__ << ", " << __FUNCTION__ << ", line " << __LINE__ << "]" << std::endl << std::endl;
            check = false;
        }
        if (btn_device_name.empty()) {
            std::cerr << std::endl << "[ERROR] [TrackingUtilizer] Parameter \"btn_device_name\" must not be empty string. " <<
                "[" << __FILE__ << ", " << __FUNCTION__ << ", line " << __LINE__ << "]" << std::endl << std::endl;
            check = false;
        }
    }
    catch (const std::exception& e) {
        std::cerr << std::endl << "[ERROR] [TrackingUtilizer] Error reading string param 'btn_device_name': " << e.what() <<
            " [" << __FILE__ << ", " << __FUNCTION__ << ", line " << __LINE__ << "]" << std::endl << std::endl;
        check = false;
    }

    std::string rigid_body_name;
    try {
        rigid_body_name = std::string(params.rigid_body_name);
        if (rigid_body_name.length() != params.rigid_body_name_len) {
            std::cerr << std::endl << "[ERROR] [TrackingUtilizer] String \"rigid_body_name\" has not expected length. " <<
                "[" << __FILE__ << ", " << __FUNCTION__ << ", line " << __LINE__ << "]" << std::endl << std::endl;
            check = false;
        }
        if (rigid_body_name.empty()) {
            std::cerr << std::endl << "[ERROR] [TrackingUtilizer] Parameter \"rigid_body_name\" must not be empty string. " <<
                "[" << __FILE__ << ", " << __FUNCTION__ << ", line " << __LINE__ << "]" << std::endl << std::endl;
            check = false;
        }
    }
    catch (const std::exception& e) {
        std::cerr << std::endl << "[ERROR] [TrackingUtilizer] Error reading string param 'rigid_body_name': " << e.what() <<
            " [" << __FILE__ << ", " << __FUNCTION__ << ", line " << __LINE__ << "]" << std::endl << std::endl;
        check = false;
    }

    int  btn_min = -1;
    int  btn_max = 100;
    bool changed = false;

    this->limit<int>(params.select_btn, btn_min, btn_max, changed);
    if (changed) {
        std::cerr << std::endl << "[ERROR] [TrackingUtilizer] Parameter \"selectButton\" must be in range [" << btn_min << "," << btn_max << "]. " <<
            "[" << __FILE__ << ", " << __FUNCTION__ << ", line " << __LINE__ << "]" << std::endl << std::endl;
        check = false;
    }
    this->limit<int>(params.rotate_btn, btn_min, btn_max, changed);
    if (changed) {
        std::cerr << std::endl << "[ERROR] [TrackingUtilizer] Parameter \"rotate_btn\" must be in range [" << btn_min << "," << btn_max << "]. " <<
            "[" << __FILE__ << ", " << __FUNCTION__ << ", line " << __LINE__ << "]" << std::endl << std::endl;
        check = false;
    }
    this->limit<int>(params.translate_btn, btn_min, btn_max, changed);
    if (changed) {
        std::cerr << std::endl << "[ERROR] [TrackingUtilizer] Parameter \"translate_btn\" must be in range [" << btn_min << "," << btn_max << "]. " <<
            "[" << __FILE__ << ", " << __FUNCTION__ << ", line " << __LINE__ << "]" << std::endl << std::endl;
        check = false;
    }
    this->limit<int>(params.zoom_btn, btn_min, btn_max, changed);
    if (changed) {
        std::cerr << std::endl << "[ERROR] [TrackingUtilizer] Parameter \"zoom_btn\" must be in range [" << btn_min << "," << btn_max << "]. " <<
            "[" << __FILE__ << ", " << __FUNCTION__ << ", line " << __LINE__ << "]" << std::endl << std::endl;
        check = false;
    }
    this->limit<float>(params.translate_speed, 0.0f, (std::numeric_limits<float>::max)(), changed);
    if (changed) {
        std::cerr << std::endl << "[ERROR] [TrackingUtilizer] Parameter \"translate_speed\" must be in range [" << 0.0f << "," << (std::numeric_limits<float>::max)() << "]. " <<
            "[" << __FILE__ << ", " << __FUNCTION__ << ", line " << __LINE__ << "]" << std::endl << std::endl;
        check = false;
    }
    this->limit<float>(params.zoom_speed, 0.0f, (std::numeric_limits<float>::max)(), changed);
    if (changed) {
        std::cerr << std::endl << "[ERROR] [TrackingUtilizer] Parameter \"zoom_speed\" must be in range [" << 0.0f << "," << (std::numeric_limits<float>::max)() << "]. " <<
            "[" << __FILE__ << ", " << __FUNCTION__ << ", line " << __LINE__ << "]" << std::endl << std::endl;
        check = false;
    }
    this->limit<float>(params.fov_height, 0.0f, (std::numeric_limits<float>::max)(), changed);
    if (changed) {
        std::cerr << std::endl << "[ERROR] [TrackingUtilizer] Parameter \"fov_height\" must be in range [" << 0.0f << "," << (std::numeric_limits<float>::max)() << "]. " <<
            "[" << __FILE__ << ", " << __FUNCTION__ << ", line " << __LINE__ << "]" << std::endl << std::endl;
        check = false;
    }
    this->limit<float>(params.fov_width, 0.0f, (std::numeric_limits<float>::max)(), changed);
    if (changed) {
        std::cerr << std::endl << "[ERROR] [TrackingUtilizer] Parameter \"fov_width\" must be in range [" << 0.0f << "," << (std::numeric_limits<float>::max)() << "]. " <<
            "[" << __FILE__ << ", " << __FUNCTION__ << ", line " << __LINE__ << "]" << std::endl << std::endl;
        check = false;
    }
    this->limit<float>(params.fov_horiz_angle, 0.0f, 180.0f, changed);
    if (changed) {
        std::cerr << std::endl << "[ERROR] [TrackingUtilizer] Parameter \"fov_horiz_angle\" must be in range [" << 0.0f << "," << 180.0f << "]. " <<
            "[" << __FILE__ << ", " << __FUNCTION__ << ", line " << __LINE__ << "]" << std::endl << std::endl;
        check = false;
    }
    this->limit<float>(params.fov_vert_angle, 0.0f, 180.0f, changed);
    if (changed) {
        std::cerr << std::endl << "[ERROR] [TrackingUtilizer] Parameter \"fov_vert_angle\" must be in range [" << 0.0f << "," << 180.0f << "]. " <<
                "[" << __FILE__ << ", " << __FUNCTION__ << ", line " << __LINE__ << "]" << std::endl << std::endl;
        check = false;
    }

    this->limit<float>(this->physicalHeight, 0.0f, (std::numeric_limits<float>::max)(), changed);
    if (changed) {
        std::cerr << std::endl << "[ERROR] [TrackingUtilizer] Parameter \"physicalHeight\" must be in range [" << 0.0f << "," << (std::numeric_limits<float>::max)() << "]. " <<
            "[" << __FILE__ << ", " << __FUNCTION__ << ", line " << __LINE__ << "]" << std::endl << std::endl;
        check = false;
    }
    this->limit<float>(this->physicalWidth, 0.0f, (std::numeric_limits<float>::max)(), changed);
    if (changed) {
        std::cerr << std::endl << "[ERROR] [TrackingUtilizer] Parameter \"physicalWidth\" must be in range [" << 0.0f << "," << (std::numeric_limits<float>::max)() << "]. " <<
            "[" << __FILE__ << ", " << __FUNCTION__ << ", line " << __LINE__ << "]" << std::endl << std::endl;
        check = false;
    }

    if (check) {
        this->buttonDeviceName = btn_device_name;
        this->rigidBodyName = rigid_body_name;
        this->selectButton = params.select_btn;
        this->rotateButton = params.rotate_btn;
        this->translateButton = params.translate_btn;
        this->zoomButton = params.zoom_btn;
        this->invertRotate = params.invert_rotate;
        this->invertTranslate = params.invert_translate;
        this->invertZoom = params.invert_zoom;
        this->translateSpeed = params.translate_speed;
        this->zoomSpeed = params.zoom_speed;
        this->singleInteraction = params.single_interaction;
        this->fovMode = params.fov_mode;
        this->fovHeight = params.fov_height;
        this->fovWidth = params.fov_width;
        this->fovHoriAngle = params.fov_horiz_angle;
        this->fovVertAngle = params.fov_vert_angle;
        this->fovAspectRatio = params.fov_aspect_ratio;

        this->curPosition.Set((std::numeric_limits<float>::max)(), (std::numeric_limits<float>::max)(), (std::numeric_limits<float>::max)());
        this->curIntersection.Set((std::numeric_limits<float>::max)(), (std::numeric_limits<float>::max)());
        this->curFOV.left_top.Set((std::numeric_limits<float>::max)(), (std::numeric_limits<float>::max)());
        this->curFOV.left_bottom.Set((std::numeric_limits<float>::max)(), (std::numeric_limits<float>::max)());
        this->curFOV.right_top.Set((std::numeric_limits<float>::max)(), (std::numeric_limits<float>::max)());
        this->curFOV.right_bottom.Set((std::numeric_limits<float>::max)(), (std::numeric_limits<float>::max)());

        this->printParams();
        this->initialised = true;

        this->initialised = this->readParamsFromFile();
    }

    return this->initialised;
}



void tracking::TrackingUtilizer::printParams(void) {
    std::cout << "[PARAMETER] [TrackingUtilizer] Button Device Name:      " << this->buttonDeviceName.c_str() << std::endl;
    std::cout << "[PARAMETER] [TrackingUtilizer] Rigid Body Name:         " << this->rigidBodyName.c_str() << std::endl;
    std::cout << "[PARAMETER] [TrackingUtilizer] Select Button:           " << this->selectButton << std::endl;
    std::cout << "[PARAMETER] [TrackingUtilizer] Rotate Button:           " << this->rotateButton << std::endl;
    std::cout << "[PARAMETER] [TrackingUtilizer] Translate Button:        " << this->translateButton << std::endl;
    std::cout << "[PARAMETER] [TrackingUtilizer] Zoom Button:             " << this->zoomButton << std::endl;
    std::cout << "[PARAMETER] [TrackingUtilizer] Invert Rotate:           " << ((this->invertRotate)?("true"):("false")) << std::endl;
    std::cout << "[PARAMETER] [TrackingUtilizer] Invert Translate:        " << ((this->invertTranslate) ? ("true") : ("false")) << std::endl;
    std::cout << "[PARAMETER] [TrackingUtilizer] Invert Zoom:             " << ((this->invertZoom) ? ("true") : ("false")) << std::endl;
    std::cout << "[PARAMETER] [TrackingUtilizer] Translate Speed:         " << this->translateSpeed << std::endl;
    std::cout << "[PARAMETER] [TrackingUtilizer] Zoom Speed:              " << this->zoomSpeed << std::endl;
    std::cout << "[PARAMETER] [TrackingUtilizer] Single Interaction:      " << ((this->singleInteraction) ? ("true") : ("false")) << std::endl;
    std::cout << "[PARAMETER] [TrackingUtilizer] FOV Mode:                " << (int)this->fovMode << std::endl;
    std::cout << "[PARAMETER] [TrackingUtilizer] FOV Height:              " << this->fovHeight << std::endl;
    std::cout << "[PARAMETER] [TrackingUtilizer] FOV Width:               " << this->fovWidth << std::endl;
    std::cout << "[PARAMETER] [TrackingUtilizer] FOV Horizontal Angle:    " << this->fovHoriAngle << std::endl;
    std::cout << "[PARAMETER] [TrackingUtilizer] FOV Vertical Angle:      " << this->fovVertAngle << std::endl;
    std::cout << "[PARAMETER] [TrackingUtilizer] FOV Aspect Ratio:        " << (int)this->fovAspectRatio << std::endl;
    std::cout << "[PARAMETER] [TrackingUtilizer] Physical Height:         " << this->physicalHeight << std::endl;
    std::cout << "[PARAMETER] [TrackingUtilizer] Physical Width:          " << this->physicalWidth << std::endl;
    std::cout << "[PARAMETER] [TrackingUtilizer] Physical Origin:         (" << this->physicalOrigin.X() << "," << this->physicalOrigin.Y() << "," << this->physicalOrigin.Z() << ")" << std::endl;
    std::cout << "[PARAMETER] [TrackingUtilizer] Physical X Dir:          (" << this->physicalXDir.X() << "," << this->physicalXDir.Y() << "," << this->physicalXDir.Z() << ")" << std::endl;
    std::cout << "[PARAMETER] [TrackingUtilizer] Physical Y Dir:          (" << this->physicalYDir.X() << "," << this->physicalYDir.Y() << "," << this->physicalYDir.Z() << ")" << std::endl;
    std::cout << "[PARAMETER] [TrackingUtilizer] Calibration Orientation: (" << this->calibrationOrientation.X() << "," << this->calibrationOrientation.Y() << 
        "," << this->calibrationOrientation.Z() << "," << this->calibrationOrientation.W() << ")" << std::endl;
}


bool tracking::TrackingUtilizer::readParamsFromFile(void) {

    std::string line, tag, name;
    float x, y, z, w;
    bool isCalibrationAvailable = false;

    const std::string filename = "tracking.conf";
    std::ifstream file(filename);
    if (!file.good()) {
        std::cerr << std::endl << "[ERROR] [TrackingUtilizer] Failed to open \"" << filename.c_str() << "\" for reading. " <<
            "[" << __FILE__ << ", " << __FUNCTION__ << ", line " << __LINE__ << "]" << std::endl << std::endl;
    }

    unsigned int lineNmbr = 1;
    while (std::getline(file, line))
    {
        std::istringstream istream(line);
        if (!(istream >> tag)) { 
            std::cerr << std::endl << "[ERROR] [TrackingUtilizer] Can not read line tag in \"" << filename.c_str() << "\" in line number: " << lineNmbr << " " <<
                "[" << __FILE__ << ", " << __FUNCTION__ << ", line " << __LINE__ << "]" << std::endl << std::endl;
            break; 
        }

        if (tag[0] == '#') { // Ignore comment lines
        }
        else if (tag == "PHYSICAL_SCREEN_HEIGHT") {
            if (!(istream >> x)) {
                std::cerr << std::endl << "[ERROR] [TrackingUtilizer] Can not read value for PHYSICAL_SCREEN_HEIGHT in \"" << filename.c_str() << "\" in line number: " << lineNmbr << " " <<
                    "[" << __FILE__ << ", " << __FUNCTION__ << ", line " << __LINE__ << "]" << std::endl << std::endl;
                file.close();
                break;
            }
            this->physicalHeight = x;
        }
        else if (tag == "PHYSICAL_SCREEN_WIDTH") {
            if (!(istream >> x)) {
                std::cerr << std::endl << "[ERROR] [TrackingUtilizer] Can not read value for PHYSICAL_SCREEN_WIDTH in \"" << filename.c_str() << "\" in line number: " << lineNmbr << " " <<
                    "[" << __FILE__ << ", " << __FUNCTION__ << ", line " << __LINE__ << "]" << std::endl << std::endl;
                file.close();
                break;
            }
            this->physicalWidth = x;
        }
        else if (tag == "PHYSICAL_SCREEN_ORIGIN") {
            if (!(istream >> x >> y >> z)) {
                std::cerr << std::endl << "[ERROR] [TrackingUtilizer] Can not read values for PHYSICAL_SCREEN_ORIGIN in \"" << filename.c_str() << "\" in line number: " << lineNmbr << " " <<
                    "[" << __FILE__ << ", " << __FUNCTION__ << ", line " << __LINE__ << "]" << std::endl << std::endl;
                file.close();
                break;
            }
            this->physicalOrigin.Set(x, y, z);
        }
        else if (tag == "PHYSICAL_SCREEN_X_DIR") {
            if (!(istream >> x >> y >> z)) {
                std::cerr << std::endl << "[ERROR] [TrackingUtilizer] Can not read values for PHYSICAL_SCREEN_X_DIR in \"" << filename.c_str() << "\" in line number: " << lineNmbr << " " <<
                    "[" << __FILE__ << ", " << __FUNCTION__ << ", line " << __LINE__ << "]" << std::endl << std::endl;
                file.close();
                break;
            }
            this->physicalXDir.Set(x, y, z);
        }
        else if (tag == "PHYSICAL_SCREEN_Y_DIR") {
            if (!(istream >> x >> y >> z)) {
                std::cerr << std::endl << "[ERROR] [TrackingUtilizer] Can not read values for PHYSICAL_SCREEN_Y_DIR in \"" << filename.c_str() << "\" in line number: " << lineNmbr << " " <<
                    "[" << __FILE__ << ", " << __FUNCTION__ << ", line " << __LINE__ << "]" << std::endl << std::endl;
                file.close();
                break;
            }
            this->physicalYDir.Set(x, y, z);
        }
        else if (tag == "PHYSICAL_CALIBRATION") {
            if (!(istream >> name >> x >> y >> z >> w)) {
                std::cerr << std::endl << "[ERROR] [TrackingUtilizer] Can not read values for PHYSICAL_CALIBRATION in \"" << filename.c_str() << "\" in line number: " << lineNmbr <<  " " <<
                    "[" << __FILE__ << ", " << __FUNCTION__ << ", line " << __LINE__ << "]" << std::endl << std::endl;
                file.close();
                break;
            }
            if (name == this->rigidBodyName) {
                this->calibrationOrientation.Set(x, y, z, w);
                isCalibrationAvailable = true;
            }
        }
        else {
            std::cerr << std::endl << "[ERROR] [TrackingUtilizer] Unknown line tag in \"" << filename.c_str() << "\". " <<
                "[" << __FILE__ << ", " << __FUNCTION__ << ", line " << __LINE__ << "]" << std::endl << std::endl;
            file.close();
            break;
        }
        lineNmbr++;
    }

    // If no calibration orientation is available, use current orientation of rigid body ...
    if (!isCalibrationAvailable) {
        this->Calibrate();
        std::ofstream file(filename, std::ifstream::app);
        if (!file.good()) {
            std::cerr << std::endl << "[ERROR] [TrackingUtilizer] Failed to open \"" << filename.c_str() << "\" for writing. " <<
                "[" << __FILE__ << ", " << __FUNCTION__ << ", line " << __LINE__ << "]" << std::endl << std::endl;
        }
        file << "PHYSICAL_CALIBRATION " << this->rigidBodyName.c_str() << " " << this->calibrationOrientation.X() << " " <<
            this->calibrationOrientation.Y() << " " << this->calibrationOrientation.Z() << " " << this->calibrationOrientation.W() << std::endl;
        std::cout << "[INFO] [TrackingUtilizer] Wrote current calibration orientation to \"" << filename.c_str() << "\"" << std::endl;
    }

    file.close();

    return true;
}


bool tracking::TrackingUtilizer::GetRawData(unsigned int& o_button,
    float& o_position_x, float& o_position_y, float& o_position_z,
    float& o_orientation_x, float& o_orientation_y, float& o_orientation_z, float& o_orientation_w) {

    if (!this->initialised) {
        std::cerr << std::endl << "[ERROR] [TrackingUtilizer] Not initialised. " <<
            "[" << __FILE__ << ", " << __FUNCTION__ << ", line " << __LINE__ << "]" << std::endl << std::endl;
        return false;
    }

    bool stateRawData = false;

    // Request updated tracking data.
    if (this->updateTrackingData()) {
        o_button = this->curButton;
        o_position_x = this->curPosition.X();
        o_position_y = this->curPosition.Y();
        o_position_z = this->curPosition.Z();
        o_orientation_x = this->curOrientation.X();
        o_orientation_y = this->curOrientation.Y();
        o_orientation_z = this->curOrientation.Z();
        o_orientation_w = this->curOrientation.W();

        stateRawData = true;
    }
    else {
        o_button = 0;
        o_position_x = (std::numeric_limits<float>::max)();
        o_position_y = (std::numeric_limits<float>::max)();
        o_position_z = (std::numeric_limits<float>::max)();
        o_orientation_x = 0.0f;
        o_orientation_y = 0.0f;
        o_orientation_z = 0.0f;
        o_orientation_w = 1.0f;
    }

    return (stateRawData);
}


bool tracking::TrackingUtilizer::GetSelectionState(bool& o_selecttion) {

    if (!this->initialised) {
        std::cerr << std::endl << "[ERROR] [TrackingUtilizer] Not initialised. " <<
            "[" << __FILE__ << ", " << __FUNCTION__ << ", line " << __LINE__ << "]" << std::endl << std::endl;
        return false;
    }

    bool stateBtnChgs = false;

    // Request updated tracking data.
    if (this->updateTrackingData()) {

        // Check for current button interaction.
        stateBtnChgs = this->processButtonChanges();
    }

    if (stateBtnChgs) {
        o_selecttion = this->curSelecting;
    }
    else {
        o_selecttion = false;
    }

    return (stateBtnChgs);
}


bool tracking::TrackingUtilizer::GetIntersection(float& o_intersection_x, float& o_intersection_y) {

    if (!this->initialised) {
        std::cerr << std::endl << "[ERROR] [TrackingUtilizer] Not initialised. " <<
            "[" << __FILE__ << ", " << __FUNCTION__ << ", line " << __LINE__ << "]" << std::endl << std::endl;
        return false;
    }

    bool stateSrnIns = false;

    // Request updated tracking data.
    if (this->updateTrackingData()) {

        // Calculate new current intersection point and fov rectangle.
        stateSrnIns = this->processScreenInteraction(false);
    }

    if (stateSrnIns) {
        o_intersection_x = this->curIntersection.X();
        o_intersection_y = this->curIntersection.Y();
    }
    else {
        o_intersection_x = (std::numeric_limits<float>::max)();
        o_intersection_y = (std::numeric_limits<float>::max)();
    }

    return (stateSrnIns);
}


bool tracking::TrackingUtilizer::GetFieldOfView(float& o_left_top_x, float& o_left_top_y,
    float& o_left_bottom_x, float& o_left_bottom_y,
    float& o_right_top_x, float& o_right_top_y,
    float& o_right_bottom_x, float& o_right_bottom_y) {

    if (!this->initialised) {
        std::cerr << std::endl << "[ERROR] [TrackingUtilizer] Not initialised. " <<
            "[" << __FILE__ << ", " << __FUNCTION__ << ", line " << __LINE__ << "]" << std::endl << std::endl;
        return false;
    }

    bool stateSrnIns = false;

    // Request updated tracking data.
    if (this->updateTrackingData()) {

        // Calculate new current intersection point and fov rectangle.
        stateSrnIns = this->processScreenInteraction(true);
    }

    if (stateSrnIns) {
        o_left_top_x = this->curFOV.left_top.X();
        o_left_top_y = this->curFOV.left_top.Y();
        o_left_bottom_x = this->curFOV.left_bottom.X();
        o_left_bottom_y = this->curFOV.left_bottom.Y();
        o_right_top_x = this->curFOV.right_top.X();
        o_right_top_y = this->curFOV.right_top.Y();
        o_right_bottom_x = this->curFOV.right_bottom.X();
        o_right_bottom_y = this->curFOV.right_bottom.Y();
    }
    else {
        o_left_top_x = (std::numeric_limits<float>::max)();
        o_left_top_y = (std::numeric_limits<float>::max)();
        o_left_bottom_x = (std::numeric_limits<float>::max)();
        o_left_bottom_y = (std::numeric_limits<float>::max)();
        o_right_top_x = (std::numeric_limits<float>::max)();
        o_right_top_y = (std::numeric_limits<float>::max)();
        o_right_bottom_x = (std::numeric_limits<float>::max)();
        o_right_bottom_y = (std::numeric_limits<float>::max)();
    }

    return (stateSrnIns);
}


bool tracking::TrackingUtilizer::GetUpdatedCamera(TrackingUtilizer::Dim i_dim,
    float i_distance_center,
    float& io_cam_position_x, float& io_cam_position_y, float& io_cam_position_z,
    float& io_cam_view_x, float& io_cam_view_y, float& io_cam_view_z,
    float& io_cam_up_x, float& io_cam_up_y, float& io_cam_up_z) {

    if (!this->initialised) {
        std::cerr << std::endl << "[ERROR] [TrackingUtilizer] Not initialised. " <<
            "[" << __FILE__ << ", " << __FUNCTION__ << ", line " << __LINE__ << "]" << std::endl << std::endl;
        return false;
    }

    bool stateBtnChgs = false;
    bool stateCamTrs  = false;

    // Request updated tracking data.
    if (this->updateTrackingData()) {

        // Set current camera for start values if camera manipulation button is pressed
        this->curCameraPosition = tracking::Vector3D(io_cam_position_x, io_cam_position_y, io_cam_position_z);
        this->curCameraView = tracking::Vector3D(io_cam_view_x, io_cam_view_y, io_cam_view_z);
        this->curCameraUp = tracking::Vector3D(io_cam_up_x, io_cam_up_y, io_cam_up_z);
        this->curCameraCenterDist = i_distance_center;

        // Check for current button interaction.
        stateBtnChgs = this->processButtonChanges();

        // Apply camera transformations enabled by pressed buttons.
        switch (i_dim) {
            case(TrackingUtilizer::Dim::DIM_2D): stateCamTrs = this->processCameraTransformations2D(); break;
            case(TrackingUtilizer::Dim::DIM_3D): stateCamTrs = this->processCameraTransformations3D(); break;
            default: break;
        }
    }

    if (stateBtnChgs && stateCamTrs) {
        io_cam_position_x = this->curCameraPosition.X();
        io_cam_position_y = this->curCameraPosition.Y();
        io_cam_position_z = this->curCameraPosition.Z();
        io_cam_view_x = this->curCameraView.X();
        io_cam_view_y = this->curCameraView.Y();
        io_cam_view_z = this->curCameraView.Z();
        io_cam_up_x = this->curCameraUp.X();
        io_cam_up_y = this->curCameraUp.Y();
        io_cam_up_z = this->curCameraUp.Z();
    }
    else {
        io_cam_position_x = (std::numeric_limits<float>::max)();
        io_cam_position_y = (std::numeric_limits<float>::max)();
        io_cam_position_z = (std::numeric_limits<float>::max)();
        io_cam_view_x = (std::numeric_limits<float>::max)();
        io_cam_view_y = (std::numeric_limits<float>::max)();
        io_cam_view_z = (std::numeric_limits<float>::max)();
        io_cam_up_x = (std::numeric_limits<float>::max)();
        io_cam_up_y = (std::numeric_limits<float>::max)();
        io_cam_up_z = (std::numeric_limits<float>::max)();
    }

    return (stateBtnChgs && stateCamTrs);
}


bool tracking::TrackingUtilizer::Calibrate(void) {

    if (!this->initialised) {
        std::cerr << std::endl << "[ERROR] [TrackingUtilizer] Not initialised. " <<
            "[" << __FILE__ << ", " << __FUNCTION__ << ", line " << __LINE__ << "]" << std::endl << std::endl;
        return false;
    }

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

    std::cout << "[INFO] [TrackingUtilizer] >>> RIGID BODY \"" << this->rigidBodyName.c_str() << "\" CALIBRATION ORIENTATION " <<
        this->calibrationOrientation.X() << ";" << this->calibrationOrientation.Y() << ";" <<
        this->calibrationOrientation.Z() << ";" << this->calibrationOrientation.W() << std::endl;

    return stateCalib;
}


bool tracking::TrackingUtilizer::updateTrackingData(void) {

    if (this->tracker == nullptr) {
        std::cerr << std::endl << "[ERROR] [TrackingUtilizer] There is no tracker connected. " <<
            "[" << __FILE__ << ", " << __FUNCTION__ << ", line " << __LINE__ << "]" << std::endl << std::endl;
        return false;
    }

    // Get fresh data from tracker
    bool retval = false;
    tracking::Tracker::TrackingData data;
    if (this->tracker->GetData(this->rigidBodyName, this->buttonDeviceName, data)) {
        this->curButton       = data.button;
        this->curPosition     = data.rigid_body.position;
        this->curOrientation  = data.rigid_body.orientation;

#ifdef TRACKING_DEBUG_OUTPUT
        std::cout << "[DEBUG] [TrackingUtilizer] Buffer Index = " << bufferIdx << " - Position Buffer Size = " << this->positionBuffer.size() << std::endl;
#endif

        // Add current position to position buffer
        const unsigned int bufferSize = 5; // ! Must be greater than 0 !
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
    std::cout << "[DEBUG] [TrackingUtilizer] Position = (" << this->curPosition.X() << ", " << this->curPosition.Y() << ", " << this->curPosition.Z() << "), Orientation = (" <<
        this->curOrientation.X() << ", " << this->curOrientation.Y() << ", " << this->curOrientation.Z() << ", " << this->curOrientation.W() << ")." << std::endl;
#endif
      
    return retval;
}


bool tracking::TrackingUtilizer::processButtonChanges(void) {

    if (this->constPosition) {
#ifdef TRACKING_DEBUG_OUTPUT
        std::cout << "[DEBUG] [TrackingUtilizer] Rigid body \"" << this->rigidBodyName.c_str() << "\" is not inside tracking area. " << std::endl;
#endif
        return false;
    }

    bool retval = true;
    if (this->lastButton != this->curButton) {

        // Detect reconfiguration of button mapping.
        this->isRotating    = false;
        this->isTranslating = false;
        this->isZooming     = false;
        this->curSelecting  = false;

        // Loop over all buttons 
        bool isPressed = false;
        for (int i = 0; i < 4; ++i) { 

            switch (i) {
            case(0): {
                if (this->rotateButton != 0) {
                    isPressed = (this->curButton == this->rotateButton);
                    this->isRotating = isPressed;
#ifdef TRACKING_DEBUG_OUTPUT
                    std::cout << "[DEBUG] [TrackingUtilizer] Rotate Button is " << ((this->isRotating) ? ("PRESSED") : ("released")) << "." << std::endl;
#endif
                }
            } break;
            case(1): {
                if (this->translateButton != 0) {
                    isPressed = (this->curButton == this->translateButton);
                    this->isTranslating = isPressed;
#ifdef TRACKING_DEBUG_OUTPUT
                    std::cout << "[DEBUG] [TrackingUtilizer] Translate Button is " << ((this->isTranslating) ? ("PRESSED") : ("released")) << "." << std::endl;
#endif
                }
            } break;
            case(2): {
                if (this->zoomButton != 0) {
                    isPressed = (this->curButton == this->zoomButton);
                    this->isZooming = isPressed;
#ifdef TRACKING_DEBUG_OUTPUT
                    std::cout << "[DEBUG] [TrackingUtilizer] Zoom Button is " << ((this->isZooming) ? ("PRESSED") : ("released")) << "." << std::endl;
#endif
                }
            } break;
            case(3): {
                if (this->selectButton != 0) {
                    this->curSelecting = (this->curButton == this->selectButton);
                    isPressed = false;  // Current configuration has not to be stored ....
#ifdef TRACKING_DEBUG_OUTPUT
                    std::cout << "[DEBUG] [TrackingUtilizer] Select Button is " << ((this->curSelecting) ? ("PRESSED") : ("released")) << "." << std::endl;
#endif
                }
            } break;
            }

            // Remember initial configuration and determine what to do.
            if (isPressed) {
                // Preserve the current camera system as we need it later to perform
                // incremental transformations.
                this->startCamView          = this->curCameraView;
                this->startCamPosition      = this->curCameraPosition;
                this->startCamUp            = this->curCameraUp;
                this->startCamCenterDist = this->curCameraCenterDist;

                // Relative orientation of the stick with respect to the current camera 
                // coordinate system. This is required to align the interaction device 
                // with the current view later on.
                auto q1 = this->xform(tracking::Vector3D(0.0f, 0.0f, 1.0f), this->startCamView);
                auto q2 = this->xform(q1 * tracking::Vector3D(0.0f, 1.0f, 0.0f), this->startCamUp);

                this->startRelativeOrientation = q2 * q1;
                this->startOrientation         = this->curOrientation;
                this->startPosition            = this->curPosition;
            }
        }

        this->lastButton = this->curButton;
    }

    return retval;
}


bool tracking::TrackingUtilizer::processCameraTransformations3D(void) {

    if (this->constPosition) {
#ifdef TRACKING_DEBUG_OUTPUT
        std::cout << "[DEBUG] [TrackingUtilizer] Rigid body \"" << this->rigidBodyName.c_str() << "\" is not inside tracking area. " << std::endl;
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
            std::cout << "[DEBUG] [TrackingUtilizer] Apply 6DOF ROTATION." << std::endl;
#endif
            // Compute relative rotation since button was pressed.
            auto quat = this->curOrientation * this->startOrientation.Inverse();
            quat.Normalise();

            // Align interaction with the original camera system.
            auto startRelativeOrientation_Conj = this->startRelativeOrientation;
            startRelativeOrientation_Conj.Conjugate();
            quat = this->startRelativeOrientation * quat * startRelativeOrientation_Conj;
            quat.Normalise();
            if (this->invertRotate) {
                quat.Invert();
            }

            // Apply new view parameters.
            this->startCamView.Normalise();
            this->startCamUp.Normalise();
            auto rotationCenter = this->startCamPosition + this->startCamView * this->startCamCenterDist;

            this->curCameraView     = quat * this->startCamView;
            this->curCameraUp       = quat * this->startCamUp;
            this->curCameraPosition = rotationCenter - (this->curCameraView * this->startCamCenterDist);

            retval = true;
        }

        if (this->isTranslating) {
#ifdef TRACKING_DEBUG_OUTPUT
            std::cout << "[DEBUG] [TrackingUtilizer] Apply 6DOF TRANSLATION." << std::endl;
#endif
            // Compute relative movement of tracker in physical space.
            auto pos_delta = this->curPosition - this->startPosition;
            //pos_delta = this->startRelativeOrientation * pos_delta;

            pos_delta *= this->translateSpeed;
            if (this->invertTranslate) {
                pos_delta *= (-1.0f);
            }

            // Apply new translated position.
            this->curCameraPosition = this->startCamPosition + pos_delta;
            this->curCameraView     = this->startCamView;
            this->curCameraUp       = this->startCamUp;

            retval = true;
        }

        if (this->isZooming) {
#ifdef TRACKING_DEBUG_OUTPUT
            std::cout << "[DEBUG] [TrackingUtilizer] Apply 6DOF ZOOM." << std::endl;
#endif
            // Compute relative movement of tracker in physical space.
            auto pos_diff = this->curPosition - this->startPosition;

            auto delta = pos_diff.Z() * this->zoomSpeed;
            auto pos_delta = this->startCamView;
            pos_delta.Normalise();
            pos_delta *= delta;
            if (this->invertZoom) {
                pos_delta *= (-1.0f);
            }

            // Apply new zoomed position.
            this->curCameraPosition = this->startCamPosition + pos_delta;
            this->curCameraView     = this->startCamView;
            this->curCameraUp       = this->startCamUp;

            retval = true;
        }
    }

    return retval;
}


bool tracking::TrackingUtilizer::processCameraTransformations2D(void) {

    if (this->constPosition) {
#ifdef TRACKING_DEBUG_OUTPUT
        std::cout << "[DEBUG] [TrackingUtilizer] Rigid body \"" << this->rigidBodyName.c_str() << "\" is not inside tracking area. " << std::endl;
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

        std::cerr << std::endl << "[ERROR] [TrackingUtilizer] 2D camera transformation is not yet implemented.  " <<
            "[" << __FILE__ << ", " << __FUNCTION__ << ", line " << __LINE__ << "]" << std::endl << std::endl;

// TODO implementation for 2D ...
/*

        if (this->isRotating) {
#ifdef TRACKING_DEBUG_OUTPUT
            std::cout << "[DEBUG] [TrackingUtilizer] Apply 6DOF ROTATION." << std::endl;
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
            tracking::Vector3D up   = quat * this->startCamUp;
            tracking::Vector3D view = quat * this->startCamView;

            // Apply new view parameters.
            //this->curCameraPosition = this->startCamPosition;
            this->curCameraView = view;
            this->curCameraUp   = up;

            retval = true;
        }

        if (this->isTranslating) {
#ifdef TRACKING_DEBUG_OUTPUT
            std::cout << "[DEBUG] [TrackingUtilizer] Apply 6DOF TRANSLATION." << std::endl;
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

            retval = true;
        }

        if (this->isZooming) {
#ifdef TRACKING_DEBUG_OUTPUT
            std::cout << "[DEBUG] [TrackingUtilizer] Apply 6DOF ZOOM." << std::endl;
#endif
            // Compute relative movement of tracker in physical space.
            auto diff = this->curPosition - this->startPosition;

            // Compute the distance in virtual space that we move the camera.
            auto delta = diff.Z() * this->zoomSpeed;

            auto view = this->startCamView;
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


bool tracking::TrackingUtilizer::processScreenInteraction(bool process_fov) {

    if (this->constPosition) {
#ifdef TRACKING_DEBUG_OUTPUT
        std::cout << "[DEBUG] [TrackingUtilizer] Rigid body \"" << this->rigidBodyName.c_str() << "\" is not inside tracking area. " << std::endl;
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
    // D = view direction
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

        // Get normal Pointing out of the screen
        auto pWv = this->physicalXDir;
        pWv.Normalise();
        auto pHv = this->physicalYDir;
        pHv.Normalise();
        auto pNv = pWv.Cross(pHv); // Physical screen normal of screen
        pNv.Normalise();

        // Absolut offset between origin of tracking coordinate system (= Vector3D<float,3>(0,0,0)) and lower left corner of screen.
        // (= distance Vector3D from tracking origin to lower left corner of screen).
        auto pOv = this->physicalOrigin; // In meters

        // Get calibration orientation of Pointing device.
        // This orientation corresponds to physical calibration direction.
        auto cOv = this->calibrationOrientation;
        auto cOq  = Quaternion(cOv.X(), cOv.Y(), cOv.Z(), cOv.W());

        // Calculate physical direction (view) of rigid body 
        // (Physical calibration direction is opposite direction of screen normal)
        auto rbDv = this->curOrientation * cOq.Inverse() * ((-1.0f) * pNv);
        rbDv.Normalise();
       
#ifdef TRACKING_DEBUG_OUTPUT
        std::cout << "[DEBUG] [TrackingUtilizer] Rigid body position (" <<  this->curPosition.X() << "," << this->curPosition.Y() << "," << this->curPosition.Z() << 
            ", orientation (" << this->curOrientation.X() << "," << this->curOrientation.Y() << "," << this->curOrientation.Z() << "," << this->curOrientation.W() << ")" << std::endl;
        std::cout << "[DEBUG] [TrackingUtilizer] Rigid body Pointing direction (" << rbDv.X() << "," << rbDv.Y() << "," << rbDv.Z() << ")" << std::endl;
        auto urC = pOv + (pWv * pWf) + (pHv * pHf); // upper right corner
        std::cout << "[DEBUG] [TrackingUtilizer] Screen spans from (" << pOv.X() << "," << pOv.Y() << "," << pOv.Z()<< " to " << urC.X() << "," << urC.Y() << "," << urC.Z() << ")" << std::endl;
        std::cout << "[DEBUG] [TrackingUtilizer] Screen normal (" << pNv.X() << "," << pNv.Y() << "," << pNv.Z() << ")" << std::endl;
#endif
        // Intersection is only possible if screen normal and the Pointing direction are in opposite direction.
        if (pNv.Dot(rbDv) < 0.0f) { // pN.Dot(pDir) must be != 0 anyway ...

            // --- Calculate screen intersection ------------------------------

            auto rbRelPosv = this->curPosition - pOv; // Realtive position to origin of screen (lower left corner)
            auto delta = pNv.Dot(rbRelPosv) / pNv.Dot(((-1.0f) * rbDv));  //Using first intercept theorem ...
            auto rbIs = this->curPosition + delta * rbDv;
#ifdef TRACKING_DEBUG_OUTPUT
            std::cout << "[DEBUG] [TrackingUtilizer] Intersects at (" << rbIs.X() << "," << rbIs.Y() << "," << rbIs.Z() << ")" << std::endl;
#endif
            // Intersection in regard to powerwall origin
            auto pIs = rbIs - pOv;
            // Scaling to relative coordinates in [0,1].
            float x = (pWv.Dot(pIs)) / pWf;
            float y = (pHv.Dot(pIs)) / pHf;
#ifdef TRACKING_DEBUG_OUTPUT
            std::cout << "[DEBUG] [TrackingUtilizer] Relative intersection at (" << x << "," << y << ")" << std::endl;
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
            if (process_fov) {
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

                // Rigid body up and right Vector3D.
                Vector3D rbUv, rbRv;
                float deltaR, deltaU;
                std::vector<Vector3D> fovVert;
                fovVert.clear();

                if ((this->fovMode == FovMode::WIDTH_AND_HEIGHT) || (this->fovMode == FovMode::WIDTH_AND_ASPECT_RATIO)) {
                                  
                    float xDelta = (fov_w  / 2.0f);
                    float yDelta = (fov_h  / 2.0f);
                    if (this->fovMode == FovMode::WIDTH_AND_ASPECT_RATIO) {
                        yDelta = (fov_w / fov_r / 2.0f);
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
                else if ((this->fovMode == FovMode::HORIZONTAL_ANGLE_AND_VERTICAL_ANGLE) || (this->fovMode == FovMode::HORIZONTAL_ANGLE_AND_ASPECT_RATIO)) {
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
                    if (this->fovMode == FovMode::HORIZONTAL_ANGLE_AND_ASPECT_RATIO) { /// DIFFERENCE for Horizontal Angle and Aspect Ratio
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
                        delta = pNv.Dot(rbRelPosv) / pNv.Dot(((-1.0f) * fovVert[i]));  //Using first intercept theorem ...
                        ///XXX: delta switches to negativ values before(?) normal of screen and view direction are orthogonal ...
                        delta = (delta < 0.0f) ? (delta * (-1.0f)) : (delta); // the 500 is empirical
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
                std::cout << "[DEBUG] [TrackingUtilizer] Relative FIELD OF VIEW coordinates: LEFT_TOP (" <<
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
