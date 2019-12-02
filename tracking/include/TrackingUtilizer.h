/**
* TrackingUtilizer.h
*
* Copyright (C) 2018 by VISUS (Universitaet Stuttgart)
* Alle Rechte vorbehalten.
*/

#ifndef TRACKING_TRACKINGUTILIZER_H_INCLUDED
#define TRACKING_TRACKINGUTILIZER_H_INCLUDED

#ifdef TRACKING_EXPORTS  
#define TRACKING_API __declspec(dllexport)   
#else  
#define TRACKING_API __declspec(dllimport)   
#endif  

#include "stdafx.h"
#include "Tracker.h"


namespace tracking {

    /***************************************************************************
    *
    * Provides processed tracking data:
    * 
    * - Raw tracking data (button, position, orientation).
    * - Manipulated camera parameters depending on pressed button(s).
    * - Screen Intersection as point in relative 2D coordinates.
    * - Field of view as rectangle in relative 2D coordinates.
    * - State of button defined for general selection processing.
    *
    ***************************************************************************
    * TROUBLESHOOTING:
    * 
    * - If your program doesn't receive tracking data (though it should):
    *     Please check your firewall settings. For your program there must be 
    *     rules which allow incoming TCP and UDP traffic. This is used by 
    *     the NatNet client in NatNetDevicePool.
    *     (Windows might not pop up a firewall notification at the first start) 
    *     Allow incoming traffic for TCP port 3884 and UDP ports 1510 and 1511.
    *
    ***************************************************************************/
    class TRACKING_API TrackingUtilizer {

    public:

        enum Dim {
            DIM_2D = 0,
            DIM_3D = 1
        };

        enum FovMode {
            WIDTH_AND_HEIGHT                    = 0,
            WIDTH_AND_ASPECT_RATIO              = 1,
            HORIZONTAL_ANGLE_AND_VERTICAL_ANGLE = 2,
            HORIZONTAL_ANGLE_AND_ASPECT_RATIO   = 3
        };

        enum FovAspectRatio {
            AR_2_35__1 = 0,
            AR_1_85__1 = 1,
            AR_1_77__1 = 2, /** = 16:9  */
            AR_1_60__1 = 3, /** = 16:10 */
            AR_1_33__1 = 4, /** = 4:3   */
            AR_1__1    = 5
        };

        /** Data structure for setting parameters as batch. */
        struct Params {
            const char*                       btn_device_name;       /** Name of the button device to use.                                                             */
            size_t                            btn_device_name_len;
            const char*                       rigid_body_name;       /** Name of the rigid body to use.                                                                */
            size_t                            rigid_body_name_len;
            tracking::Button                  select_btn;            /** The button that must be pressed for selection (set to 0 to dissolve link to any button).      */
            tracking::Button                  rotate_btn;            /** The button that must be pressed for rotation (set to 0 to dissolve link to any button).       */
            tracking::Button                  translate_btn;         /** The button that must be pressed for translation (set to 0 to dissolve link to any button).    */
            tracking::Button                  zoom_btn;              /** The button that must be pressed for dolly zoom (set to 0 to dissolve link to any button).     */
            bool                              invert_rotate;         /** Inverts the rotation direction.                                                               */
            bool                              invert_translate;      /** Inverts the rotation direction.                                                               */
            bool                              invert_zoom;           /** Inverts the zoom direction.                                                                   */
            float                             translate_speed;       /** The translation speed.                                                                        */
            float                             zoom_speed;            /** Transformation of distance to zoom speed.                                                     */
            bool                              single_interaction;    /** Disables multiple interactions at the same time.                                              */
            TrackingUtilizer::FovMode         fov_mode;              /** Select the parameters to define field of view projected on screen.                            */
            float                             fov_height;            /** Set relative fixed width for fov on screen.                                                   */
            float                             fov_width;             /** Set relative fixed height for fov on screen.                                                  */
            float                             fov_horiz_angle;       /** Set fixed horizontal angle in degrees for fov.                                                */
            float                             fov_vert_angle;        /** Set fixed vertical angle in degrees for fov.                                                  */
            TrackingUtilizer::FovAspectRatio  fov_aspect_ratio;      /** Set fixed aspect ratio for fov for given angle.                                               */
        };

        ///////////////////////////////////////////////////////////////////////

        /**
        * CTOR
        */
        TrackingUtilizer(void);

        /**
        * DTOR
        */
        virtual ~TrackingUtilizer(void);

        /**
        * Initialisation.
        *
        * @return True for success, false otherwise.
        */
        bool Initialise(const tracking::TrackingUtilizer::Params& params, std::shared_ptr<tracking::Tracker> tracker);

        /**********************************************************************/
        // GET

        /**
        *  Get the raw tracking data.
        *
        * @param o_button                 Output the current button of the given button device.
        * @param o_position_(x,y,z)       Output the current position of the given rigid body.
        * @param o_orientation_(x,y,z,w)  Output the current orientation of the given rigid body.
        *
        * @return True for success, false otherwise.
        */
        bool GetRawData(unsigned int& o_button,
            float& o_position_x, float& o_position_y, float& o_position_z,
            float& o_orientation_x, float& o_orientation_y, float& o_orientation_z, float& o_orientation_w);

        /**
        *  Get the current state of the select button.
        *
        * @param o_selecttion  Output the current selection state. True if select button is pressed, false otherwise.
        *
        * @return True for success, false otherwise.
        */
        bool GetSelectionState(bool& o_selecttion);

        /**
        *  Get the current intersection with the screen.
        *
        * @param o_intersection_(x,y)  Output the relative 2D screen intersection coordinates (in range [0,1]).
        *
        * @return True for success, false otherwise.
        */
        bool GetIntersection(float& o_intersection_x, float& o_intersection_y);

        /**
        *  Get the current field of view.
        *
        * @param (lt,lb,rt,rb)_(x,y)  Output the relative 2D screen space field of view vertices (in range [0,1]).
        *
        * @return True for success, false otherwise.
        */
        bool GetFieldOfView(float& o_left_top_x, float& o_left_top_y, 
            float& o_left_bottom_x, float& o_left_bottom_y, 
            float& o_right_top_x, float& o_right_top_y, 
            float& o_right_bottom_x, float& o_right_bottom_y);

        /**
        * Get the updated camera vectors depending on pressed buttons.
        * 
        * Requires previous call of SetCurrentCamera(...).
        * 
        * @param _idim                 The current world space dimension.
        *                              3D: Transformations are applied in three-dimensional space.
        *                              2D: Transformations are applied in two-dimensional screen space.
        * @param i_distance_center     The distance along the view vector to the rotation center of the camera.
        * @param io_cam_position_(x,y) Input current camera position. Output of the updated camera position.
        * @param io_cam_view_(x,y)     Input current camera view direction. Output of the updated camera view direction.
        * @param io_cam_up_(x,y)       Input current camera up direction. Output of the updated camera up direction.
        *
        * @return True for success, false otherwise.
        */
        bool GetUpdatedCamera(TrackingUtilizer::Dim i_dim, 
            float i_distance_center,
            float& io_cam_position_x, float& io_cam_position_y, float& io_cam_position_z, 
            float& io_cam_view_x, float& io_cam_view_y, float& io_cam_view_z, 
            float& io_cam_up_x, float& io_cam_up_y, float& io_cam_up_z);

        /**
        * Get the button device name.
        *
        * @return The button device name.
        */
        inline const char* GetButtonDeviceName(void) const {
            return this->buttonDeviceName.c_str();
        }

        /**
        * Get the rigid body name.
        *
        * @return The rigid body name.
        */
        inline const char* GetRigidBodyName(void) const {
            return this->rigidBodyName.c_str();
        }

        /**
        * Detect the calibration orientation of the pointing device. 
        * >>> Put rigid body somewhere pointing vertically towards the powerwall screen and
        * >>> right- and up-Vector3D of rigid body must be parallel to x- and y-axis of powerwall screen.
        *
        * @return True for success, false otherwise.
        */
        bool Calibrate(void);

    private:

        /***********************************************************************
        * variables
        **********************************************************************/

        bool                               initialised;
        std::shared_ptr<tracking::Tracker> tracker;
        tracking::Vector3D                 curCameraPosition;
        tracking::Vector3D                 curCameraUp;
        tracking::Vector3D                 curCameraView;
        float                              curCameraCenterDist;
        tracking::Point2D                  curIntersection;
        tracking::Rectangle                curFOV;
        tracking::Quaternion               curOrientation;
        tracking::Vector3D                 curPosition;
        tracking::Button                   curButton;
        tracking::Button                   lastButton;
        bool                               curSelecting;
        std::vector<tracking::Vector3D>    positionBuffer;
        unsigned int                       bufferIdx;
        bool                               constPosition;
        tracking::Vector3D                 startCamView;
        tracking::Vector3D                 startCamPosition;
        tracking::Vector3D                 startCamUp;
        float                              startCamCenterDist;
        tracking::Vector3D                 startPosition;
        tracking::Quaternion               startOrientation;
        tracking::Quaternion               startRelativeOrientation;
        bool                               isRotating;
        bool                               isTranslating;
        bool                               isZooming;

        /** parameters ********************************************************/

        std::string                        buttonDeviceName;
        std::string                        rigidBodyName;
        unsigned int                       selectButton;
        unsigned int                       rotateButton;
        unsigned int                       translateButton;
        unsigned int                       zoomButton;
        bool                               invertRotate;
        bool                               invertTranslate;
        bool                               invertZoom;
        float                              translateSpeed;
        float                              zoomSpeed;
        bool                               singleInteraction;
        TrackingUtilizer::FovMode          fovMode;
        float                              fovHeight;
        float                              fovWidth;
        float                              fovHoriAngle;
        float                              fovVertAngle;
        TrackingUtilizer::FovAspectRatio   fovAspectRatio;

        float                              physicalHeight;
        float                              physicalWidth;
        tracking::Quaternion               calibrationOrientation;
        tracking::Vector3D                 physicalOrigin;
        tracking::Vector3D                 physicalXDir;
        tracking::Vector3D                 physicalYDir;

        /***********************************************************************
        * functions
        **********************************************************************/

        /** Print used parameter values. */
        void printParams(void);

        /** Read physical values from file. */
        bool readParamsFromFile(void);

        /**
        * Request updated tracking data.
        *
        * @return True for success, false otherwise.
        */
        bool updateTrackingData(void);

        /**
        * Process button changes.
        */
        bool processButtonChanges(void);

        /**
        * Apply camera 3D transformations.
        */
        bool processCameraTransformations3D(void);

        /**
        * Apply camera 2D transformations.
        */
        bool processCameraTransformations2D(void);

        /**
        * Process screen interaction.
        */
        bool processScreenInteraction(bool process_fov);

        /**
        * Limit value "val" to range [min, max]
        */
        template<typename T>
        T limit(T val, T min, T max);

        template<typename T>
        T limit(T val, T min, T max, bool& changed);

        /**
        * xform
        *
        * @return Answer the (normalised) rotation for matching 'u' on 'v'.
        */
        tracking::Quaternion xform(const tracking::Vector3D& u, const tracking::Vector3D& v);

    };

} /** end namespace tracking */

#endif /** TRACKING_TRACKINGUTILIZER_H_INCLUDED */