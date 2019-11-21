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
    * - Raw tracking data (button states, position, orientation).
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
            tracking::ButtonMask              select_btn;            /** The button that must be pressed for selection (set to 0 to dissolve link to any button).      */
            tracking::ButtonMask              rotate_btn;            /** The button that must be pressed for rotation (set to 0 to dissolve link to any button).       */
            tracking::ButtonMask              translate_btn;         /** The button that must be pressed for translation (set to 0 to dissolve link to any button).    */
            tracking::ButtonMask              zoom_btn;              /** The button that must be pressed for dolly zoom (set to 0 to dissolve link to any button).     */
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
        bool Initialise(const tracking::TrackingUtilizer::Params& inParams, std::shared_ptr<tracking::Tracker> inTracker);

        /**********************************************************************/
        // GET

        /**
        *  Get the raw tracking data.
        *
        * @param btnState          Output the current button state of the given button device.
        * @param pos_(x,y,z)       Output the current position of the given rigid body.
        * @param orient_(x,y,z,w)  Output the current orientation of the given rigid body.
        *
        * @return True for success, false otherwise.
        */
        bool GetRawData(unsigned int& btn_state, float& pos_x, float& pos_y, float& pos_z, 
            float& orient_x, float& orient_y, float& orient_z, float& orient_w);

        /**
        *  Get the current state of the select button.
        *
        * @param select  Output the current selection state. True if select button is pressed, false otherwise.
        *
        * @return True for success, false otherwise.
        */
        bool GetSelectionState(bool& select);

        /**
        *  Get the current intersection with the screen.
        *
        * @param intersect_(x,y)  Output the relative 2D screen intersection coordinates (in range [0,1]).
        *
        * @return True for success, false otherwise.
        */
        bool GetIntersection(float& intersect_x, float& intersect_y);

        /**
        *  Get the current field of view.
        *
        * @param (lt,lb,rt,rb)_(x,y)  Output the relative 2D screen space filed of view vertices (in range [0,1]).
        *
        * @return True for success, false otherwise.
        */
        bool GetFieldOfView(float& lt_x, float& lt_y, float& lb_x, float& lb_y, float& rt_x, float& rt_y, float& rb_x, float& rb_y);

        /**
        * Get the updated camera vectors depending on pressed buttons.
        * 
        * Requires previous call of SetCurrentCamera(...).
        * 
        * @param dim                The current world space dimension.
        * 3D: Transformations are applied in three-dimensional space.
        * 2D: Transformations are applied in two-dimensional screen space.
        *
        * @param cam_pos_(x,y)      Output the camera position.
        * @param cam_lookat_(x,y)   Output the camera loook at position.
        * @param cam_up_(x,y)       Output the up direction of the camera.
        *
        * @return True for success, false otherwise.
        */
        bool GetUpdatedCamera(TrackingUtilizer::Dim dim, float& cam_pos_x, float& cam_pos_y, float& cam_pos_z, 
            float& cam_lookat_x, float& cam_lookat_y, float& cam_lookat_z, 
            float& cam_up_x, float& cam_up_y, float& cam_up_z);

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

        /**********************************************************************/
        // SET

        /**
        * Set the camera vectors to use for manipulation.
        *
        * @param inCamPos     The camera position.
        * @param inCamLookAt  The look at position of the camera.
        * @param inCamUp      The up direction of the camera.
        *
        * @return True for success, false otherwise.
        */
        bool SetCurrentCamera(float cam_pos_x, float cam_pos_y, float cam_pos_z, float cam_lookat_x, float cam_lookat_y, float cam_lookat_z, float cam_up_x, float cam_up_y, float cam_up_z);

        /**
        * Set the calibration orientation of the Pointing device. 
        * >>> Put rigid body somewhere Pointing vertically towards the powerwall screen and
        * >>> right- and up-Vector3D of rigid body must be parallel to x- and y-axis of powerwall screen.
        *
        * @return True for success, false otherwise.
        */
        bool Calibration(void);

    private:

        /***********************************************************************
        * variables
        **********************************************************************/

        bool initialised;

        /** The Pointer to the Tracker which provides the tracking data. */
        std::shared_ptr<tracking::Tracker> tracker;

        tracking::Vector3D                 curCameraPosition;
        tracking::Vector3D                 curCameraUp;
        tracking::Vector3D                 curCameraLookAt;
        tracking::Point2D                  curIntersection;
        tracking::Rectangle                curFOV;
        tracking::Quaternion               curOrientation;
        tracking::Vector3D                 curPosition;
        tracking::ButtonMask               curButtonStates;
        tracking::ButtonMask               lastButtonStates;
        bool                               curSelecting;
        std::vector<tracking::Vector3D>    positionBuffer;
        unsigned int                       bufferIdx;
        bool                               constPosition;
        tracking::Vector3D                 startCamLookAt;
        tracking::Vector3D                 startCamPosition;
        tracking::Vector3D                 startCamUp;
        tracking::Vector3D                 startPosition;
        tracking::Quaternion               startOrientation;
        tracking::Quaternion               startRelativeOrientation;
        bool                               isRotating;
        bool                               isTranslating;
        bool                               isZooming;

        /** parameters ********************************************************/

        std::string                        buttonDeviceName;
        std::string                        rigidBodyName;
        tracking::ButtonMask               selectButton;
        tracking::ButtonMask               rotateButton;
        tracking::ButtonMask               translateButton;
        tracking::ButtonMask               zoomButton;
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
        bool processScreenInteraction(bool processFov);

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