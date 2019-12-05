/**
* TrackingUtilizer.cpp
*
* Copyright (C) 2018 by VISUS (Universitaet Stuttgart)
* Alle Rechte vorbehalten.
*/

#include "TrackingUtilizer.h"

#define TRACKING_FLOAT_MAX ((std::numeric_limits<float>::max)())


tracking::TrackingUtilizer::TrackingUtilizer(void) 
    : m_initialised(false)
    , m_tracker(nullptr)
    , m_current_cam_position()
    , m_current_cam_up()
    , m_current_cam_view()
    , m_current_cam_center_dist()
    , m_current_intersection()
    , m_current_fov()
    , m_current_orientation()
    , m_current_position()
    , m_current_button()
    , m_current_selecting(false)
    , m_last_button(0)
    , m_buffer_positions()
    , m_buffer_idx(0)
    , m_const_position(false)
    , m_start_cam_view()
    , m_start_cam_position()
    , m_start_cam_up()
    , m_start_cam_center_dist()
    , m_start_position()
    , m_start_orientation()
    , m_start_relative_orientation()
    , m_is_rotating(false)
    , m_is_translating(false)
    , m_is_zooming(false)
    , m_button_device_name("ControlBox")
    , m_rigid_body_name("Stick")
    , m_select_button(0)
    , m_rotate_button(0)
    , m_translate_button(0)
    , m_zoom_button(0)
    , m_invert_rotate(true)
    , m_invert_translate(true)
    , m_invert_zoom(true)
    , m_translate_speed(10.0f)
    , m_zoom_speed(20.0f)
    , m_single_interaction(false)
    , m_fov_mode(TrackingUtilizer::FovMode::WIDTH_AND_HEIGHT)
    , m_fov_height(0.2f)
    , m_fov_width(0.2f)
    , m_fov_hori_angle(60.0f)
    , m_fov_vert_angle(30.0f)
    , m_fov_aspect_ratio(TrackingUtilizer::FovAspectRatio::AR_1_77__1) 
    , m_physical_height(2.4f)
    , m_physical_width(6.0f)
    , m_calibration_orientation()
    , m_physical_origin(-3.0f, 0.3f, 0.0f)
    , m_physical_x_dir(1.0f, 0.0f, 0.0f)
    , m_physical_y_dir(0.0f, 1.0f, 0.0f) {

    // intentionally empty...
}


tracking::TrackingUtilizer::~TrackingUtilizer(void) {

    // nothing to do here ...
}


bool tracking::TrackingUtilizer::Initialise(const tracking::TrackingUtilizer::Params & params, std::shared_ptr<tracking::Tracker> m_tracker) {

    bool check = true;
    this->m_initialised = false;

    if (m_tracker == nullptr) {
        std::cerr << std::endl << "[ERROR] [TrackingUtilizer] Pointer to m_tracker is nullptr. " <<
            "[" << __FILE__ << ", " << __FUNCTION__ << ", line " << __LINE__ << "]" << std::endl << std::endl;
        return false;
    }
    this->m_tracker = m_tracker;

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
        std::cerr << std::endl << "[ERROR] [TrackingUtilizer] Parameter \"m_select_button\" must be in range [" << btn_min << "," << btn_max << "]. " <<
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
    this->limit<float>(params.translate_speed, 0.0f, TRACKING_FLOAT_MAX, changed);
    if (changed) {
        std::cerr << std::endl << "[ERROR] [TrackingUtilizer] Parameter \"translate_speed\" must be in range [" << 0.0f << "," << TRACKING_FLOAT_MAX << "]. " <<
            "[" << __FILE__ << ", " << __FUNCTION__ << ", line " << __LINE__ << "]" << std::endl << std::endl;
        check = false;
    }
    this->limit<float>(params.zoom_speed, 0.0f, TRACKING_FLOAT_MAX, changed);
    if (changed) {
        std::cerr << std::endl << "[ERROR] [TrackingUtilizer] Parameter \"zoom_speed\" must be in range [" << 0.0f << "," << TRACKING_FLOAT_MAX << "]. " <<
            "[" << __FILE__ << ", " << __FUNCTION__ << ", line " << __LINE__ << "]" << std::endl << std::endl;
        check = false;
    }
    this->limit<float>(params.fov_height, 0.0f, TRACKING_FLOAT_MAX, changed);
    if (changed) {
        std::cerr << std::endl << "[ERROR] [TrackingUtilizer] Parameter \"fov_height\" must be in range [" << 0.0f << "," << TRACKING_FLOAT_MAX << "]. " <<
            "[" << __FILE__ << ", " << __FUNCTION__ << ", line " << __LINE__ << "]" << std::endl << std::endl;
        check = false;
    }
    this->limit<float>(params.fov_width, 0.0f, TRACKING_FLOAT_MAX, changed);
    if (changed) {
        std::cerr << std::endl << "[ERROR] [TrackingUtilizer] Parameter \"fov_width\" must be in range [" << 0.0f << "," << TRACKING_FLOAT_MAX << "]. " <<
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

    this->limit<float>(this->m_physical_height, 0.0f, TRACKING_FLOAT_MAX, changed);
    if (changed) {
        std::cerr << std::endl << "[ERROR] [TrackingUtilizer] Parameter \"m_physical_height\" must be in range [" << 0.0f << "," << TRACKING_FLOAT_MAX << "]. " <<
            "[" << __FILE__ << ", " << __FUNCTION__ << ", line " << __LINE__ << "]" << std::endl << std::endl;
        check = false;
    }
    this->limit<float>(this->m_physical_width, 0.0f, TRACKING_FLOAT_MAX, changed);
    if (changed) {
        std::cerr << std::endl << "[ERROR] [TrackingUtilizer] Parameter \"m_physical_width\" must be in range [" << 0.0f << "," << TRACKING_FLOAT_MAX << "]. " <<
            "[" << __FILE__ << ", " << __FUNCTION__ << ", line " << __LINE__ << "]" << std::endl << std::endl;
        check = false;
    }

    if (check) {
        this->m_button_device_name = btn_device_name;
        this->m_rigid_body_name = rigid_body_name;
        this->m_select_button = params.select_btn;
        this->m_rotate_button = params.rotate_btn;
        this->m_translate_button = params.translate_btn;
        this->m_zoom_button = params.zoom_btn;
        this->m_invert_rotate = params.invert_rotate;
        this->m_invert_translate = params.invert_translate;
        this->m_invert_zoom = params.invert_zoom;
        this->m_translate_speed = params.translate_speed;
        this->m_zoom_speed = params.zoom_speed;
        this->m_single_interaction = params.single_interaction;
        this->m_fov_mode = params.fov_mode;
        this->m_fov_height = params.fov_height;
        this->m_fov_width = params.fov_width;
        this->m_fov_hori_angle = params.fov_horiz_angle;
        this->m_fov_vert_angle = params.fov_vert_angle;
        this->m_fov_aspect_ratio = params.fov_aspect_ratio;

        this->m_current_position.Set(TRACKING_FLOAT_MAX, TRACKING_FLOAT_MAX, TRACKING_FLOAT_MAX);
        this->m_current_intersection.Set(TRACKING_FLOAT_MAX, TRACKING_FLOAT_MAX);
        this->m_current_fov.left_top.Set(TRACKING_FLOAT_MAX, TRACKING_FLOAT_MAX);
        this->m_current_fov.left_bottom.Set(TRACKING_FLOAT_MAX, TRACKING_FLOAT_MAX);
        this->m_current_fov.right_top.Set(TRACKING_FLOAT_MAX, TRACKING_FLOAT_MAX);
        this->m_current_fov.right_bottom.Set(TRACKING_FLOAT_MAX, TRACKING_FLOAT_MAX);

        this->printParams();
        this->m_initialised = true;

        this->m_initialised = this->readParamsFromFile();
    }

    return this->m_initialised;
}



void tracking::TrackingUtilizer::printParams(void) {
    std::cout << "[PARAMETER] [TrackingUtilizer] Button Device Name:      " << this->m_button_device_name.c_str() << std::endl;
    std::cout << "[PARAMETER] [TrackingUtilizer] Rigid Body Name:         " << this->m_rigid_body_name.c_str() << std::endl;
    std::cout << "[PARAMETER] [TrackingUtilizer] Select Button:           " << this->m_select_button << std::endl;
    std::cout << "[PARAMETER] [TrackingUtilizer] Rotate Button:           " << this->m_rotate_button << std::endl;
    std::cout << "[PARAMETER] [TrackingUtilizer] Translate Button:        " << this->m_translate_button << std::endl;
    std::cout << "[PARAMETER] [TrackingUtilizer] Zoom Button:             " << this->m_zoom_button << std::endl;
    std::cout << "[PARAMETER] [TrackingUtilizer] Invert Rotate:           " << ((this->m_invert_rotate)?("true"):("false")) << std::endl;
    std::cout << "[PARAMETER] [TrackingUtilizer] Invert Translate:        " << ((this->m_invert_translate) ? ("true") : ("false")) << std::endl;
    std::cout << "[PARAMETER] [TrackingUtilizer] Invert Zoom:             " << ((this->m_invert_zoom) ? ("true") : ("false")) << std::endl;
    std::cout << "[PARAMETER] [TrackingUtilizer] Translate Speed:         " << this->m_translate_speed << std::endl;
    std::cout << "[PARAMETER] [TrackingUtilizer] Zoom Speed:              " << this->m_zoom_speed << std::endl;
    std::cout << "[PARAMETER] [TrackingUtilizer] Single Interaction:      " << ((this->m_single_interaction) ? ("true") : ("false")) << std::endl;
    std::cout << "[PARAMETER] [TrackingUtilizer] FOV Mode:                " << (int)this->m_fov_mode << std::endl;
    std::cout << "[PARAMETER] [TrackingUtilizer] FOV Height:              " << this->m_fov_height << std::endl;
    std::cout << "[PARAMETER] [TrackingUtilizer] FOV Width:               " << this->m_fov_width << std::endl;
    std::cout << "[PARAMETER] [TrackingUtilizer] FOV Horizontal Angle:    " << this->m_fov_hori_angle << std::endl;
    std::cout << "[PARAMETER] [TrackingUtilizer] FOV Vertical Angle:      " << this->m_fov_vert_angle << std::endl;
    std::cout << "[PARAMETER] [TrackingUtilizer] FOV Aspect Ratio:        " << (int)this->m_fov_aspect_ratio << std::endl;
    std::cout << "[PARAMETER] [TrackingUtilizer] Physical Height:         " << this->m_physical_height << std::endl;
    std::cout << "[PARAMETER] [TrackingUtilizer] Physical Width:          " << this->m_physical_width << std::endl;
    std::cout << "[PARAMETER] [TrackingUtilizer] Physical Origin:         (" << this->m_physical_origin.X() << "," << this->m_physical_origin.Y() << "," << this->m_physical_origin.Z() << ")" << std::endl;
    std::cout << "[PARAMETER] [TrackingUtilizer] Physical X Dir:          (" << this->m_physical_x_dir.X() << "," << this->m_physical_x_dir.Y() << "," << this->m_physical_x_dir.Z() << ")" << std::endl;
    std::cout << "[PARAMETER] [TrackingUtilizer] Physical Y Dir:          (" << this->m_physical_y_dir.X() << "," << this->m_physical_y_dir.Y() << "," << this->m_physical_y_dir.Z() << ")" << std::endl;
    std::cout << "[PARAMETER] [TrackingUtilizer] Calibration Orientation: (" << this->m_calibration_orientation.X() << "," << this->m_calibration_orientation.Y() << 
        "," << this->m_calibration_orientation.Z() << "," << this->m_calibration_orientation.W() << ")" << std::endl;
}


bool tracking::TrackingUtilizer::readParamsFromFile(void) {

    std::string line, tag, name;
    float x, y, z, w;
    bool is_calibration_available = false;

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
            this->m_physical_height = x;
        }
        else if (tag == "PHYSICAL_SCREEN_WIDTH") {
            if (!(istream >> x)) {
                std::cerr << std::endl << "[ERROR] [TrackingUtilizer] Can not read value for PHYSICAL_SCREEN_WIDTH in \"" << filename.c_str() << "\" in line number: " << lineNmbr << " " <<
                    "[" << __FILE__ << ", " << __FUNCTION__ << ", line " << __LINE__ << "]" << std::endl << std::endl;
                file.close();
                break;
            }
            this->m_physical_width = x;
        }
        else if (tag == "PHYSICAL_SCREEN_ORIGIN") {
            if (!(istream >> x >> y >> z)) {
                std::cerr << std::endl << "[ERROR] [TrackingUtilizer] Can not read values for PHYSICAL_SCREEN_ORIGIN in \"" << filename.c_str() << "\" in line number: " << lineNmbr << " " <<
                    "[" << __FILE__ << ", " << __FUNCTION__ << ", line " << __LINE__ << "]" << std::endl << std::endl;
                file.close();
                break;
            }
            this->m_physical_origin.Set(x, y, z);
        }
        else if (tag == "PHYSICAL_SCREEN_X_DIR") {
            if (!(istream >> x >> y >> z)) {
                std::cerr << std::endl << "[ERROR] [TrackingUtilizer] Can not read values for PHYSICAL_SCREEN_X_DIR in \"" << filename.c_str() << "\" in line number: " << lineNmbr << " " <<
                    "[" << __FILE__ << ", " << __FUNCTION__ << ", line " << __LINE__ << "]" << std::endl << std::endl;
                file.close();
                break;
            }
            this->m_physical_x_dir.Set(x, y, z);
        }
        else if (tag == "PHYSICAL_SCREEN_Y_DIR") {
            if (!(istream >> x >> y >> z)) {
                std::cerr << std::endl << "[ERROR] [TrackingUtilizer] Can not read values for PHYSICAL_SCREEN_Y_DIR in \"" << filename.c_str() << "\" in line number: " << lineNmbr << " " <<
                    "[" << __FILE__ << ", " << __FUNCTION__ << ", line " << __LINE__ << "]" << std::endl << std::endl;
                file.close();
                break;
            }
            this->m_physical_y_dir.Set(x, y, z);
        }
        else if (tag == "PHYSICAL_CALIBRATION") {
            if (!(istream >> name >> x >> y >> z >> w)) {
                std::cerr << std::endl << "[ERROR] [TrackingUtilizer] Can not read values for PHYSICAL_CALIBRATION in \"" << filename.c_str() << "\" in line number: " << lineNmbr <<  " " <<
                    "[" << __FILE__ << ", " << __FUNCTION__ << ", line " << __LINE__ << "]" << std::endl << std::endl;
                file.close();
                break;
            }
            if (name == this->m_rigid_body_name) {
                this->m_calibration_orientation.Set(x, y, z, w);
                is_calibration_available = true;
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
    if (!is_calibration_available) {
        this->Calibrate();
        std::ofstream file(filename, std::ifstream::app);
        if (!file.good()) {
            std::cerr << std::endl << "[ERROR] [TrackingUtilizer] Failed to open \"" << filename.c_str() << "\" for writing. " <<
                "[" << __FILE__ << ", " << __FUNCTION__ << ", line " << __LINE__ << "]" << std::endl << std::endl;
        }
        file << "PHYSICAL_CALIBRATION " << this->m_rigid_body_name.c_str() << " " << this->m_calibration_orientation.X() << " " <<
            this->m_calibration_orientation.Y() << " " << this->m_calibration_orientation.Z() << " " << this->m_calibration_orientation.W() << std::endl;
        std::cout << "[INFO] [TrackingUtilizer] Wrote current calibration orientation to \"" << filename.c_str() << "\"" << std::endl;
    }

    file.close();

    return true;
}


bool tracking::TrackingUtilizer::GetRawData(unsigned int& o_button,
    float& o_position_x, float& o_position_y, float& o_position_z,
    float& o_orientation_x, float& o_orientation_y, float& o_orientation_z, float& o_orientation_w) {

    if (!this->m_initialised) {
        std::cerr << std::endl << "[ERROR] [TrackingUtilizer] Not m_initialised. " <<
            "[" << __FILE__ << ", " << __FUNCTION__ << ", line " << __LINE__ << "]" << std::endl << std::endl;
        return false;
    }

    bool state_rawdata = false;

    // Request updated tracking data.
    if (this->updateTrackingData()) {
        o_button = this->m_current_button;
        o_position_x = this->m_current_position.X();
        o_position_y = this->m_current_position.Y();
        o_position_z = this->m_current_position.Z();
        o_orientation_x = this->m_current_orientation.X();
        o_orientation_y = this->m_current_orientation.Y();
        o_orientation_z = this->m_current_orientation.Z();
        o_orientation_w = this->m_current_orientation.W();

        state_rawdata = true;
    }
    else {
        o_button = 0;
        o_position_x = TRACKING_FLOAT_MAX;
        o_position_y = TRACKING_FLOAT_MAX;
        o_position_z = TRACKING_FLOAT_MAX;
        o_orientation_x = 0.0f;
        o_orientation_y = 0.0f;
        o_orientation_z = 0.0f;
        o_orientation_w = 1.0f;
    }

    return (state_rawdata);
}


bool tracking::TrackingUtilizer::GetSelectionState(bool& o_selecttion) {

    if (!this->m_initialised) {
        std::cerr << std::endl << "[ERROR] [TrackingUtilizer] Not m_initialised. " <<
            "[" << __FILE__ << ", " << __FUNCTION__ << ", line " << __LINE__ << "]" << std::endl << std::endl;
        return false;
    }

    bool state_button = false;

    // Request updated tracking data.
    if (this->updateTrackingData()) {

        // Check for current button interaction.
        state_button = this->processButtonChanges();
    }

    if (state_button) {
        o_selecttion = this->m_current_selecting;
    }
    else {
        o_selecttion = false;
    }

    return (state_button);
}


bool tracking::TrackingUtilizer::GetIntersection(float& o_intersection_x, float& o_intersection_y) {

    if (!this->m_initialised) {
        std::cerr << std::endl << "[ERROR] [TrackingUtilizer] Not m_initialised. " <<
            "[" << __FILE__ << ", " << __FUNCTION__ << ", line " << __LINE__ << "]" << std::endl << std::endl;
        return false;
    }

    bool state_intersection = false;

    // Request updated tracking data.
    if (this->updateTrackingData()) {

        // Calculate new current intersection point and fov rectangle.
        state_intersection = this->processScreenInteraction(false);
    }

    if (state_intersection) {
        o_intersection_x = this->m_current_intersection.X();
        o_intersection_y = this->m_current_intersection.Y();
    }
    else {
        o_intersection_x = TRACKING_FLOAT_MAX;
        o_intersection_y = TRACKING_FLOAT_MAX;
    }

    return (state_intersection);
}


bool tracking::TrackingUtilizer::GetFieldOfView(float& o_left_top_x, float& o_left_top_y,
    float& o_left_bottom_x, float& o_left_bottom_y,
    float& o_right_top_x, float& o_right_top_y,
    float& o_right_bottom_x, float& o_right_bottom_y) {

    if (!this->m_initialised) {
        std::cerr << std::endl << "[ERROR] [TrackingUtilizer] Not m_initialised. " <<
            "[" << __FILE__ << ", " << __FUNCTION__ << ", line " << __LINE__ << "]" << std::endl << std::endl;
        return false;
    }

    bool state_fov = false;

    // Request updated tracking data.
    if (this->updateTrackingData()) {

        // Calculate new current intersection point and fov rectangle.
        state_fov = this->processScreenInteraction(true);
    }

    if (state_fov) {
        o_left_top_x = this->m_current_fov.left_top.X();
        o_left_top_y = this->m_current_fov.left_top.Y();
        o_left_bottom_x = this->m_current_fov.left_bottom.X();
        o_left_bottom_y = this->m_current_fov.left_bottom.Y();
        o_right_top_x = this->m_current_fov.right_top.X();
        o_right_top_y = this->m_current_fov.right_top.Y();
        o_right_bottom_x = this->m_current_fov.right_bottom.X();
        o_right_bottom_y = this->m_current_fov.right_bottom.Y();
    }
    else {
        o_left_top_x = TRACKING_FLOAT_MAX;
        o_left_top_y = TRACKING_FLOAT_MAX;
        o_left_bottom_x = TRACKING_FLOAT_MAX;
        o_left_bottom_y = TRACKING_FLOAT_MAX;
        o_right_top_x = TRACKING_FLOAT_MAX;
        o_right_top_y = TRACKING_FLOAT_MAX;
        o_right_bottom_x = TRACKING_FLOAT_MAX;
        o_right_bottom_y = TRACKING_FLOAT_MAX;
    }

    return (state_fov);
}


bool tracking::TrackingUtilizer::GetUpdatedCamera(TrackingUtilizer::Dim i_dim,
    float i_distance_center,
    float& io_cam_position_x, float& io_cam_position_y, float& io_cam_position_z,
    float& io_cam_view_x, float& io_cam_view_y, float& io_cam_view_z,
    float& io_cam_up_x, float& io_cam_up_y, float& io_cam_up_z) {

    if (!this->m_initialised) {
        std::cerr << std::endl << "[ERROR] [TrackingUtilizer] Not m_initialised. " <<
            "[" << __FILE__ << ", " << __FUNCTION__ << ", line " << __LINE__ << "]" << std::endl << std::endl;
        return false;
    }

    bool state_button = false;
    bool state_cam_transform  = false;

    // Request updated tracking data.
    if (this->updateTrackingData()) {

        // Set current camera for start values if camera manipulation button is pressed
        this->m_current_cam_position = tracking::Vector3D(io_cam_position_x, io_cam_position_y, io_cam_position_z);
        this->m_current_cam_view = tracking::Vector3D(io_cam_view_x, io_cam_view_y, io_cam_view_z);
        this->m_current_cam_up = tracking::Vector3D(io_cam_up_x, io_cam_up_y, io_cam_up_z);
        this->m_current_cam_center_dist = i_distance_center;

        // Check for current button interaction.
        state_button = this->processButtonChanges();

        // Apply camera transformations enabled by pressed buttons.
        switch (i_dim) {
            case(TrackingUtilizer::Dim::DIM_2D): state_cam_transform = this->processCameraTransformations2D(); break;
            case(TrackingUtilizer::Dim::DIM_3D): state_cam_transform = this->processCameraTransformations3D(); break;
            default: break;
        }
    }

    if (state_button && state_cam_transform) {
        io_cam_position_x = this->m_current_cam_position.X();
        io_cam_position_y = this->m_current_cam_position.Y();
        io_cam_position_z = this->m_current_cam_position.Z();
        io_cam_view_x = this->m_current_cam_view.X();
        io_cam_view_y = this->m_current_cam_view.Y();
        io_cam_view_z = this->m_current_cam_view.Z();
        io_cam_up_x = this->m_current_cam_up.X();
        io_cam_up_y = this->m_current_cam_up.Y();
        io_cam_up_z = this->m_current_cam_up.Z();
    }
    else {
        io_cam_position_x = TRACKING_FLOAT_MAX;
        io_cam_position_y = TRACKING_FLOAT_MAX;
        io_cam_position_z = TRACKING_FLOAT_MAX;
        io_cam_view_x = TRACKING_FLOAT_MAX;
        io_cam_view_y = TRACKING_FLOAT_MAX;
        io_cam_view_z = TRACKING_FLOAT_MAX;
        io_cam_up_x = TRACKING_FLOAT_MAX;
        io_cam_up_y = TRACKING_FLOAT_MAX;
        io_cam_up_z = TRACKING_FLOAT_MAX;
    }

    return (state_button && state_cam_transform);
}


bool tracking::TrackingUtilizer::Calibrate(void) {

    if (!this->m_initialised) {
        std::cerr << std::endl << "[ERROR] [TrackingUtilizer] Not m_initialised. " <<
            "[" << __FILE__ << ", " << __FUNCTION__ << ", line " << __LINE__ << "]" << std::endl << std::endl;
        return false;
    }

    bool state_calibration = false;

    // Request updated tracking data.
    if (this->updateTrackingData()) {
        // Store current rigid body orientation as calibration orientation.
        this->m_calibration_orientation = this->m_current_orientation;
        state_calibration = true;
    }
    else {
        this->m_calibration_orientation.Set(0.0f, 0.0f, 0.0f, 1.0f);
    }

    std::cout << "[INFO] [TrackingUtilizer] >>> RIGID BODY \"" << this->m_rigid_body_name.c_str() << "\" CALIBRATION ORIENTATION " <<
        this->m_calibration_orientation.X() << ";" << this->m_calibration_orientation.Y() << ";" <<
        this->m_calibration_orientation.Z() << ";" << this->m_calibration_orientation.W() << std::endl;

    return state_calibration;
}


bool tracking::TrackingUtilizer::updateTrackingData(void) {

    if (this->m_tracker == nullptr) {
        std::cerr << std::endl << "[ERROR] [TrackingUtilizer] There is no m_tracker connected. " <<
            "[" << __FILE__ << ", " << __FUNCTION__ << ", line " << __LINE__ << "]" << std::endl << std::endl;
        return false;
    }

    // Get fresh data from m_tracker
    bool retval = false;
    tracking::Tracker::TrackingData data;
    if (this->m_tracker->GetData(this->m_rigid_body_name, this->m_button_device_name, data)) {
        this->m_current_button       = data.button;
        this->m_current_position     = data.rigid_body.position;
        this->m_current_orientation  = data.rigid_body.orientation;

#ifdef TRACKING_DEBUG_OUTPUT
        std::cout << "[DEBUG] [TrackingUtilizer] Buffer Index = " << m_buffer_idx << " - Position Buffer Size = " << this->m_buffer_positions.size() << std::endl;
#endif

        // Add current position to position buffer
        const unsigned int bufferSize = 5; // ! Must be greater than 0 !
        this->m_buffer_positions.resize(bufferSize);
        this->m_buffer_positions[this->m_buffer_idx] = this->m_current_position;
        this->m_buffer_idx = (++this->m_buffer_idx) % bufferSize;
        // Check if all positions are the same which indicates that rigid body is no longer tracked ...
        this->m_const_position = true;
        for (size_t i = 0; i < this->m_buffer_positions.size() - 1; ++i) {
            if (this->m_buffer_positions[i] != this->m_buffer_positions[i+1]) {
                this->m_const_position = false;
                break;
            }
        }
        retval = true;
    }

#ifdef TRACKING_DEBUG_OUTPUT
    std::cout << "[DEBUG] [TrackingUtilizer] Position = (" << this->m_current_position.X() << ", " << this->m_current_position.Y() << ", " << this->m_current_position.Z() << "), Orientation = (" <<
        this->m_current_orientation.X() << ", " << this->m_current_orientation.Y() << ", " << this->m_current_orientation.Z() << ", " << this->m_current_orientation.W() << ")." << std::endl;
#endif
      
    return retval;
}


bool tracking::TrackingUtilizer::processButtonChanges(void) {

    if (this->m_const_position) {
#ifdef TRACKING_DEBUG_OUTPUT
        std::cout << "[DEBUG] [TrackingUtilizer] Rigid body \"" << this->m_rigid_body_name.c_str() << "\" is not inside tracking area. " << std::endl;
#endif
        return false;
    }

    bool retval = true;
    if (this->m_last_button != this->m_current_button) {

        // Detect reconfiguration of button mapping.
        this->m_is_rotating    = false;
        this->m_is_translating = false;
        this->m_is_zooming     = false;
        this->m_current_selecting  = false;

        // Loop over all buttons 
        bool isPressed = false;
        for (int i = 0; i < 4; ++i) { 

            switch (i) {
            case(0): {
                if (this->m_rotate_button != 0) {
                    isPressed = (this->m_current_button == this->m_rotate_button);
                    this->m_is_rotating = isPressed;
#ifdef TRACKING_DEBUG_OUTPUT
                    std::cout << "[DEBUG] [TrackingUtilizer] Rotate Button is " << ((this->m_is_rotating) ? ("PRESSED") : ("released")) << "." << std::endl;
#endif
                }
            } break;
            case(1): {
                if (this->m_translate_button != 0) {
                    isPressed = (this->m_current_button == this->m_translate_button);
                    this->m_is_translating = isPressed;
#ifdef TRACKING_DEBUG_OUTPUT
                    std::cout << "[DEBUG] [TrackingUtilizer] Translate Button is " << ((this->m_is_translating) ? ("PRESSED") : ("released")) << "." << std::endl;
#endif
                }
            } break;
            case(2): {
                if (this->m_zoom_button != 0) {
                    isPressed = (this->m_current_button == this->m_zoom_button);
                    this->m_is_zooming = isPressed;
#ifdef TRACKING_DEBUG_OUTPUT
                    std::cout << "[DEBUG] [TrackingUtilizer] Zoom Button is " << ((this->m_is_zooming) ? ("PRESSED") : ("released")) << "." << std::endl;
#endif
                }
            } break;
            case(3): {
                if (this->m_select_button != 0) {
                    this->m_current_selecting = (this->m_current_button == this->m_select_button);
                    isPressed = false;  // Current configuration has not to be stored ....
#ifdef TRACKING_DEBUG_OUTPUT
                    std::cout << "[DEBUG] [TrackingUtilizer] Select Button is " << ((this->m_current_selecting) ? ("PRESSED") : ("released")) << "." << std::endl;
#endif
                }
            } break;
            }

            // Remember initial configuration and determine what to do.
            if (isPressed) {
                // Preserve the current camera system as we need it later to perform
                // incremental transformations.
                this->m_start_cam_view        = this->m_current_cam_view;
                this->m_start_cam_position    = this->m_current_cam_position;
                this->m_start_cam_up          = this->m_current_cam_up;
                this->m_start_cam_center_dist = this->m_current_cam_center_dist;

                // Relative orientation of the stick with respect to the current camera 
                // coordinate system. This is required to align the interaction device 
                // with the current view later on.
                /// NB: z-Value of view vector depends on right- or left-handed camera system!
                /// Assuming right-handed here.
                auto q1 = this->xform(tracking::Vector3D(0.0f, 0.0f, -1.0f), this->m_start_cam_view);
                auto q2 = this->xform(q1 * tracking::Vector3D(0.0f, 1.0f, 0.0f), this->m_start_cam_up);

                this->m_start_relative_orientation = q2 * q1;
                this->m_start_orientation          = this->m_current_orientation;
                this->m_start_position             = this->m_current_position;
            }
        }

        this->m_last_button = this->m_current_button;
    }

    return retval;
}


bool tracking::TrackingUtilizer::processCameraTransformations3D(void) {

    if (this->m_const_position) {
#ifdef TRACKING_DEBUG_OUTPUT
        std::cout << "[DEBUG] [TrackingUtilizer] Rigid body \"" << this->m_rigid_body_name.c_str() << "\" is not inside tracking area. " << std::endl;
#endif
        return false;
    }

    bool retval = false;

    int  xact   = 0;
    if (this->m_is_rotating) {
        ++xact;
    }
    if (this->m_is_translating) {
        ++xact;
    }
    if (this->m_is_zooming) {
        ++xact;
    }
    if (this->m_single_interaction && (xact > 1)) {
        xact = 0;
    }

    if (xact > 0) {

        if (this->m_is_rotating) {
#ifdef TRACKING_DEBUG_OUTPUT
            std::cout << "[DEBUG] [TrackingUtilizer] Apply 6DOF ROTATION." << std::endl;
#endif
            // Compute relative rotation since button was pressed.
            auto quat = this->m_current_orientation * this->m_start_orientation.Inverse();
            quat.Normalise();

            // Align interaction with the original camera system.
            auto m_start_relative_orientation_Conj = this->m_start_relative_orientation;
            m_start_relative_orientation_Conj.Conjugate();
            quat = this->m_start_relative_orientation * quat * m_start_relative_orientation_Conj;
            quat.Normalise();
            if (this->m_invert_rotate) {
                quat.Invert();
            }

            this->m_start_cam_view.Normalise();
            this->m_start_cam_up.Normalise();
            auto rotationCenter = this->m_start_cam_position + this->m_start_cam_view * this->m_start_cam_center_dist;

            this->m_current_cam_view     = quat * this->m_start_cam_view;
            this->m_current_cam_up       = quat * this->m_start_cam_up;
            this->m_current_cam_position = rotationCenter - (this->m_current_cam_view * this->m_start_cam_center_dist);

            retval = true;
        }

        if (this->m_is_translating) {
#ifdef TRACKING_DEBUG_OUTPUT
            std::cout << "[DEBUG] [TrackingUtilizer] Apply 6DOF TRANSLATION." << std::endl;
#endif
            // Compute relative movement of m_tracker in physical space.
            auto pos_delta = this->m_current_position - this->m_start_position;

            this->m_start_cam_up.Normalise();
            auto right_vector = this->m_start_cam_view.Cross(this->m_start_cam_up);
            right_vector.Normalise();

            float right_component = right_vector.Dot(pos_delta);
            float up_component = this->m_start_cam_up.Dot(pos_delta);
            pos_delta = right_vector * right_component + this->m_start_cam_up * up_component;

            pos_delta *= this->m_translate_speed;
            if (this->m_invert_translate) {
                pos_delta *= (-1.0f);
            }

            this->m_current_cam_position = this->m_start_cam_position + pos_delta;
            this->m_current_cam_view     = this->m_start_cam_view;
            this->m_current_cam_up       = this->m_start_cam_up;

            retval = true;
        }

        if (this->m_is_zooming) {
#ifdef TRACKING_DEBUG_OUTPUT
            std::cout << "[DEBUG] [TrackingUtilizer] Apply 6DOF ZOOM." << std::endl;
#endif
            // Compute relative movement of m_tracker in physical space.
            auto pos_diff = this->m_current_position - this->m_start_position;

            auto delta = pos_diff.Z() * this->m_zoom_speed;
            auto pos_delta = this->m_start_cam_view;
            pos_delta.Normalise();
            pos_delta *= delta;
            if (this->m_invert_zoom) {
                pos_delta *= (-1.0f);
            }

            this->m_current_cam_position = this->m_start_cam_position + pos_delta;
            this->m_current_cam_view     = this->m_start_cam_view;
            this->m_current_cam_up       = this->m_start_cam_up;

            retval = true;
        }
    }

    return retval;
}


bool tracking::TrackingUtilizer::processCameraTransformations2D(void) {

    if (this->m_const_position) {
#ifdef TRACKING_DEBUG_OUTPUT
        std::cout << "[DEBUG] [TrackingUtilizer] Rigid body \"" << this->m_rigid_body_name.c_str() << "\" is not inside tracking area. " << std::endl;
#endif
        return false;
    }

    bool retval = false;
    int  xact = 0;

    if (this->m_is_rotating) {
        ++xact;
    }
    if (this->m_is_translating) {
        ++xact;
    }
    if (this->m_is_zooming) {
        ++xact;
    }
    if (this->m_single_interaction && (xact > 1)) {
        xact = 0;
    }

    if (xact > 0) {

        std::cerr << std::endl << "[ERROR] [TrackingUtilizer] 2D camera transformation is not yet implemented.  " <<
            "[" << __FILE__ << ", " << __FUNCTION__ << ", line " << __LINE__ << "]" << std::endl << std::endl;

// TODO implementation for 2D ...
/*
        if (this->m_is_rotating) {
#ifdef TRACKING_DEBUG_OUTPUT
            std::cout << "[DEBUG] [TrackingUtilizer] Apply 6DOF ROTATION." << std::endl;
#endif

            retval = true;
        }

        if (this->m_is_translating) {
#ifdef TRACKING_DEBUG_OUTPUT
            std::cout << "[DEBUG] [TrackingUtilizer] Apply 6DOF TRANSLATION." << std::endl;
#endif

            retval = true;
        }

        if (this->m_is_zooming) {
#ifdef TRACKING_DEBUG_OUTPUT
            std::cout << "[DEBUG] [TrackingUtilizer] Apply 6DOF ZOOM." << std::endl;
#endif
            retval = true;
        }
*/

    }
    return retval;
}


bool tracking::TrackingUtilizer::processScreenInteraction(bool process_fov) {

    if (this->m_const_position) {
#ifdef TRACKING_DEBUG_OUTPUT
        std::cout << "[DEBUG] [TrackingUtilizer] Rigid body \"" << this->m_rigid_body_name.c_str() << "\" is not inside tracking area. " << std::endl;
#endif
        return false;
    }

    const double PI = 3.1415926535897;

    bool retval = false;
    this->m_current_intersection.Set(TRACKING_FLOAT_MAX, TRACKING_FLOAT_MAX);
    this->m_current_fov.left_top.Set(TRACKING_FLOAT_MAX, TRACKING_FLOAT_MAX);
    this->m_current_fov.left_bottom.Set(TRACKING_FLOAT_MAX, TRACKING_FLOAT_MAX);
    this->m_current_fov.right_top.Set(TRACKING_FLOAT_MAX, TRACKING_FLOAT_MAX);
    this->m_current_fov.right_bottom.Set(TRACKING_FLOAT_MAX, TRACKING_FLOAT_MAX);

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
    auto pHf = this->m_physical_height; // In meters
    auto pWf = this->m_physical_width; // In meters

    if ((pHf > 0.0f) && (pWf > 0.0f)) {

        // Get normal Pointing out of the screen
        auto pWv = this->m_physical_x_dir;
        pWv.Normalise();
        auto pHv = this->m_physical_y_dir;
        pHv.Normalise();
        auto pNv = pWv.Cross(pHv); // Physical screen normal of screen
        pNv.Normalise();

        // Absolut offset between origin of tracking coordinate system (= Vector3D<float,3>(0,0,0)) and lower left corner of screen.
        // (= distance Vector3D from tracking origin to lower left corner of screen).
        auto pOv = this->m_physical_origin; // In meters

        // Get calibration orientation of Pointing device.
        // This orientation corresponds to physical calibration direction.
        auto cOv = this->m_calibration_orientation;
        auto cOq  = Quaternion(cOv.X(), cOv.Y(), cOv.Z(), cOv.W());

        // Calculate physical direction (view) of rigid body 
        // (Physical calibration direction is opposite direction of screen normal)
        auto rbDv = this->m_current_orientation * cOq.Inverse() * ((-1.0f) * pNv);
        rbDv.Normalise();
       
#ifdef TRACKING_DEBUG_OUTPUT
        std::cout << "[DEBUG] [TrackingUtilizer] Rigid body position (" <<  this->m_current_position.X() << "," << this->m_current_position.Y() << "," << this->m_current_position.Z() << 
            ", orientation (" << this->m_current_orientation.X() << "," << this->m_current_orientation.Y() << "," << this->m_current_orientation.Z() << "," << this->m_current_orientation.W() << ")" << std::endl;
        std::cout << "[DEBUG] [TrackingUtilizer] Rigid body Pointing direction (" << rbDv.X() << "," << rbDv.Y() << "," << rbDv.Z() << ")" << std::endl;
        auto urC = pOv + (pWv * pWf) + (pHv * pHf); // upper right corner
        std::cout << "[DEBUG] [TrackingUtilizer] Screen spans from (" << pOv.X() << "," << pOv.Y() << "," << pOv.Z()<< " to " << urC.X() << "," << urC.Y() << "," << urC.Z() << ")" << std::endl;
        std::cout << "[DEBUG] [TrackingUtilizer] Screen normal (" << pNv.X() << "," << pNv.Y() << "," << pNv.Z() << ")" << std::endl;
#endif
        // Intersection is only possible if screen normal and the Pointing direction are in opposite direction.
        if (pNv.Dot(rbDv) < 0.0f) { // pN.Dot(pDir) must be != 0 anyway ...

            // --- Calculate screen intersection ------------------------------

            auto rbRelPosv = this->m_current_position - pOv; // Realtive position to origin of screen (lower left corner)
            auto delta = pNv.Dot(rbRelPosv) / pNv.Dot(((-1.0f) * rbDv));  //Using first intercept theorem ...
            auto rbIs = this->m_current_position + delta * rbDv;
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
                this->m_current_intersection.Set(x, y);
                retval = true;
            }

            // --- Calculate field of view square projected on screen ---------
            if (process_fov) {
                float fov_w  = this->m_fov_width;
                float fov_h  = this->m_fov_height;
                float fov_ah = this->m_fov_hori_angle / 2.0f; // already using half angle!
                float fov_av = this->m_fov_vert_angle / 2.0f; // already using half angle!
                float fov_r  = 1.0f;
                switch ((int)this->m_fov_aspect_ratio) {
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

                if ((this->m_fov_mode == FovMode::WIDTH_AND_HEIGHT) || (this->m_fov_mode == FovMode::WIDTH_AND_ASPECT_RATIO)) {
                                  
                    float xDelta = (fov_w  / 2.0f);
                    float yDelta = (fov_h  / 2.0f);
                    if (this->m_fov_mode == FovMode::WIDTH_AND_ASPECT_RATIO) {
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
                            this->m_current_fov.left_top.Set(x, y); break;
                        case(1): // left bottom
                            this->m_current_fov.left_bottom.Set(x, y); break;
                        case(2): // right top
                            this->m_current_fov.right_top.Set(x, y); break;
                        case(3): // right bottom
                            this->m_current_fov.right_bottom.Set(x, y); break;
                        }
                    }
                }
                else if ((this->m_fov_mode == FovMode::HORIZONTAL_ANGLE_AND_VERTICAL_ANGLE) || (this->m_fov_mode == FovMode::HORIZONTAL_ANGLE_AND_ASPECT_RATIO)) {
                    // Horizontal Angle and Vertical Angle - Horizontal Angle and Aspect Ratio

                    // Current 'up' Vector3D of rigid body is equal to "heigth Vector3D" of powerwall.
                    rbUv = this->m_current_orientation * cOq.Inverse() * pHv;
                    rbUv.Normalise();
                    // Current 'right' Vector3D of rigid body is equal to "width Vector3D" of powerwall.
                    rbRv = this->m_current_orientation * cOq.Inverse() * pWv;
                    rbRv.Normalise();
                    // Calculate length of right Vector3D for given horizontal angle in distance of normalized looak at Vector3D.
                    deltaR = (float)std::tan((double)fov_ah / 180.0 * PI); // * 1.0f = Length of rbDv

                    // Calculate length of 'up' Vector3D for given vertical angle in distance of normalized looak at Vector3D.
                    deltaU = (float)std::tan((double)fov_av / 180.0 * PI); // * 1.0f = Length of rbDv
                    if (this->m_fov_mode == FovMode::HORIZONTAL_ANGLE_AND_ASPECT_RATIO) { /// DIFFERENCE for Horizontal Angle and Aspect Ratio
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
                        rbIs = this->m_current_position + delta * fovVert[i];
                        // Intersection in regard to powerwall origin
                        pIs = rbIs - pOv;
                        // Scaling to relative coordinate in [0,1].
                        x = (pWv.Dot(pIs)) / pWf;
                        y = (pHv.Dot(pIs)) / pHf;
                        switch (i) {
                        case(0): // left top
                            this->m_current_fov.left_top.Set(x, y); break;
                        case(1): // left bottom
                            this->m_current_fov.left_bottom.Set(x, y); break;
                        case(2): // right top
                            this->m_current_fov.right_top.Set(x, y); break;
                        case(3): // right bottom
                            this->m_current_fov.right_bottom.Set(x, y); break;
                        }
                    }
                }

                // Limit fov coords to dimensions of screen
                float min = 0.0f;
                float max = 1.0f;
                this->m_current_fov.left_top.SetX(this->limit<float>(this->m_current_fov.left_top.X(), min, max));
                this->m_current_fov.left_top.SetY(this->limit<float>(this->m_current_fov.left_top.Y(), min, max));
                this->m_current_fov.left_bottom.SetX(this->limit<float>(this->m_current_fov.left_bottom.X(), min, max));
                this->m_current_fov.left_bottom.SetY(this->limit<float>(this->m_current_fov.left_bottom.Y(), min, max));
                this->m_current_fov.right_top.SetX(this->limit<float>(this->m_current_fov.right_top.X(), min, max));
                this->m_current_fov.right_top.SetY(this->limit<float>(this->m_current_fov.right_top.Y(), min, max));
                this->m_current_fov.right_bottom.SetX(this->limit<float>(this->m_current_fov.right_bottom.X(), min, max));
                this->m_current_fov.right_bottom.SetY(this->limit<float>(this->m_current_fov.right_bottom.Y(), min, max));

                // Check if fov lies completely outside of screen
                retval = true;
                if (((this->m_current_fov.right_top.X() <= min) && (this->m_current_fov.right_bottom.X() <= min) && (this->m_current_fov.left_top.X() <= min) && (this->m_current_fov.left_bottom.X() <= min)) ||
                    ((this->m_current_fov.right_top.X() >= max) && (this->m_current_fov.right_bottom.X() >= max) && (this->m_current_fov.left_top.X() >= max) && (this->m_current_fov.left_bottom.X() >= max)) ||
                    ((this->m_current_fov.right_top.Y() <= min) && (this->m_current_fov.right_bottom.Y() <= min) && (this->m_current_fov.left_top.Y() <= min) && (this->m_current_fov.left_bottom.Y() <= min)) ||
                    ((this->m_current_fov.right_top.Y() >= max) && (this->m_current_fov.right_bottom.Y() >= max) && (this->m_current_fov.left_top.Y() >= max) && (this->m_current_fov.left_bottom.Y() >= max)))
                {
                    retval = false;
                    this->m_current_fov.left_top.Set(    TRACKING_FLOAT_MAX, TRACKING_FLOAT_MAX);
                    this->m_current_fov.left_bottom.Set( TRACKING_FLOAT_MAX, TRACKING_FLOAT_MAX);
                    this->m_current_fov.right_top.Set(   TRACKING_FLOAT_MAX, TRACKING_FLOAT_MAX);
                    this->m_current_fov.right_bottom.Set(TRACKING_FLOAT_MAX, TRACKING_FLOAT_MAX);
                }

#ifdef TRACKING_DEBUG_OUTPUT
                std::cout << "[DEBUG] [TrackingUtilizer] Relative FIELD OF VIEW coordinates: LEFT_TOP (" <<
                    this->m_current_fov.left_top.X() << "," << this->m_current_fov.left_top.Y() << ") | LEFT_BOTTOM (" <<
                    this->m_current_fov.left_bottom.X() << "," << this->m_current_fov.left_bottom.Y() << ") | RIGHT_TOP (" <<
                    this->m_current_fov.right_top.X() << "," << this->m_current_fov.right_top.Y() << ") | RIGHT_BOTTOM (" <<
                    this->m_current_fov.right_bottom.X() << "," << this->m_current_fov.right_bottom.Y() << ")" << std::endl;
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
