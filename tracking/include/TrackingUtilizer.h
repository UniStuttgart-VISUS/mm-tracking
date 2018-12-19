/**
* TrackingUtilizer.h
*
* Copyright (C) 2018 by VISUS (Universitaet Stuttgart)
* Alle Rechte vorbehalten.
*/

#ifndef TRACKING_TRACKINGUTILIZER_H_INCLUDED
#define TRACKING_TRACKINGUTILIZER_H_INCLUDED

#if (defined(_MSC_VER) && (_MSC_VER > 1000))
#pragma once
#endif /** (defined(_MSC_VER) && (_MSC_VER > 1000)) */

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
        typedef struct {
            std::string                       buttonDeviceName;        /** Name of the button device to use.                                                             */
            std::string                       rigidBodyName;           /** Name of the rigid body to use.                                                                */
            unsigned int                      selectButton;            /** The button that must be pressed for selection (set to -1 to dissolve link to any button).     */
            unsigned int                      rotateButton;            /** The button that must be pressed for rotation (set to -1 to dissolve link to any button).      */
            unsigned int                      translateButton;         /** The button that must be pressed for translation (set to -1 to dissolve link to any button).   */
            unsigned int                      zoomButton;              /** The button that must be pressed for dolly zoom (set to -1 to dissolve link to any button).    */
            bool                              invertRotate;            /** Inverts the rotation direction.                                                               */
            bool                              invertTranslate;         /** Inverts the rotation direction.                                                               */
            bool                              invertZoom;              /** Inverts the zoom direction.                                                                   */
            float                             translateSpeed;          /** The translation speed.                                                                        */
            float                             zoomSpeed;               /** Transformation of distance to zoom speed.                                                     */
            bool                              singleInteraction;       /** Disables multiple interactions at the same time.                                              */
            TrackingUtilizer::FovMode         fovMode;                 /** Select the parameters to define field of view projected on screen.                            */
            float                             fovHeight;               /** Set relative fixed width for fov on screen.                                                   */
            float                             fovWidth;                /** Set relative fixed height for fov on screen.                                                  */
            float                             fovHoriAngle;            /** Set fixed horizontal angle in degrees for fov.                                                */
            float                             fovVertAngle;            /** Set fixed vertical angle in degrees for fov.                                                  */
            TrackingUtilizer::FovAspectRatio  fovAspectRatio;          /** Set fixed aspect ratio for fov for given angle.                                               */
        } Params;

        ///////////////////////////////////////////////////////////////////////

        /**
        * CTOR
        */
        TrackingUtilizer(void);
        TrackingUtilizer(std::shared_ptr<tracking::Tracker> inTrackerPtr);
        TrackingUtilizer(TrackingUtilizer::Params& inUtilizerParams);
        TrackingUtilizer(tracking::Tracker::Params& inTrackerParams);
        TrackingUtilizer(TrackingUtilizer::Params& inUtilizerParams, tracking::Tracker::Params& inTrackerParams);
        TrackingUtilizer(TrackingUtilizer::Params& inUtilizerParams, std::shared_ptr<tracking::Tracker> inTrackerPtr);

        /**
        // Copy CTOR 
        TrackingUtilizer(const TrackingUtilizer& rhs);

        // Move  CTOR 
        TrackingUtilizer(TrackingUtilizer&& rhs);

        // Copy operator 
        TrackingUtilizer& operator=(const TrackingUtilizer& rhs);

        // Move operator
        TrackingUtilizer& operator=(TrackingUtilizer&& rhs);
        */

        /**
        * DTOR
        */
        virtual ~TrackingUtilizer(void);

        /**********************************************************************/
        // GET

        /**
        *  Get the raw tracking data.
        *
        * @param inoutBtnState  The current button state of the given button device.
        * @param inoutPos       The current position of the given rigid body.
        * @param inoutOri       The current orientation of the given rigid body.
        *
        * @return True for success, false otherwise.
        */
        bool GetRawData(tracking::ButtonMask& inoutBtnState, tracking::Vector3D& inoutPos, tracking::Quaternion& inoutOri);

        /**
        *  Get the current state of the select button.
        *
        * @param inoutSelect  True if select button is pressed, false otherwise.
        *
        * @return True for success, false otherwise.
        */
        bool GetSelectionState(bool& inoutSelect);

        /**
        *  Get the current intersection with the screen.
        *
        * @param inoutIntersect  The relative screen interaction.
        *
        * @return True for success, false otherwise.
        */
        bool GetIntersection(tracking::Point2D& inoutIntersect);

        /**
        *  Get the current field of view.
        *
        * @param inoutFov  The relative filed of view vertices (2D = screen space).
        *
        * @return True for success, false otherwise.
        */
        bool GetFieldOfView(tracking::Rectangle& inoutFov);

        /**
        * Get the updated camera vectors depending on pressed buttons.
        * 3D: Transformations are applied in three-dimensional space.
        * 2D: Transformations are applied in two-dimensional screen space.
        * 
        * @param dim             The camera position.
        * @param inoutCamPos     The camera position.
        * @param inoutCamLookAt  The camera loook at position.
        * @param inoutCamUp      The up direction of the camera.
        *
        * @return True for success, false otherwise.
        */
        bool GetUpdatedCamera(TrackingUtilizer::Dim dim, tracking::Vector3D& inoutCamPos, tracking::Vector3D& inoutCamLookAt, tracking::Vector3D& inoutCamUp);

        /**
        * Get the button device name.
        *
        * @return The current button device name.
        */
        inline std::string GetButtonDeviceName(void) const {
            return this->buttonDeviceName;
        }

        /**
        * Get the rigid body name.
        *
        * @return The current rigid body name.
        */
        inline std::string GetRigidBodyName(void) const {
            return this->rigidBodyName;
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
        bool SetCurrentCamera(tracking::Vector3D inCamPos, tracking::Vector3D inCamLookAt, tracking::Vector3D inCamUp);

        /**
        * Set the calibration orientation of the Point2Ding device. 
        * >>> Put rigid body somewhere Point2Ding vertically towards the powerwall screen and
        * >>> right- and up-Vector3D of rigid body must be parallel to x- and y-axis of powerwall screen.
        *
        * @return True for success, false otherwise.
        */
        bool Calibration(void);

    private:

        /***********************************************************************
        * variables
        **********************************************************************/

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

        bool                               curSelecting;

        std::vector<tracking::Vector3D>    positionBuffer;
        unsigned int                       bufferIdx;
        bool                               constPosition;

        tracking::ButtonMask               lastButtonStates;

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

        int                                selectButton; 
        int                                rotateButton;
        int                                translateButton;
        int                                zoomButton;

        bool                               invertRotate;
        bool                               invertTranslate;
        bool                               invertZoom;

        float                              translateSpeed;
        float                              zoomSpeed;

        bool                               singleInteraction;

        float                              physicalHeight;
        float                              physicalWidth;
        
        tracking::Quaternion               calibrationOrientation;
        tracking::Vector3D                 physicalOrigin;
        tracking::Vector3D                 physicalXDir;
        tracking::Vector3D                 physicalYDir;

        TrackingUtilizer::FovMode          fovMode;
        float                              fovHeight;
        float                              fovWidth;
        float                              fovHoriAngle;
        float                              fovVertAngle;
        TrackingUtilizer::FovAspectRatio   fovAspectRatio;
        
        /***********************************************************************
        * functions
        **********************************************************************/

        /** Initialize parameters. */
        void paramsInit(void);
        void paramsInit(TrackingUtilizer::Params& p);

        /** Print used parameter values. */
        void paramsPrint(void);

        /** Read physical values from file. */
        bool paramsRead(void);

        /** Print used parameter values. */
        bool paramsCheck(void);

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

        /**
        * Returns the class name. --- UNUSED ---
        */
        const std::string cn(void) const;
    };

} /** end namespace tracking */

#endif /** TRACKING_TRACKINGUTILIZER_H_INCLUDED */