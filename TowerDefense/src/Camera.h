#pragma once

#include "TickParameters.h"
#include "vmath.h"

/* 
 * Camera controls the moding of the camera. Initially it will 
 * only be able to display five aspects corresponding to the 
 * corners of the play area and directly overhead
 */
class Camera
{
private:
   Vector2i mResolution;
   static const float sTransitionTime;
   static const float sFOVNarrow;
   static const float sFOVWide;

   //Rotation members
   float mTransitionTimer;
   Quatf mCameraQuaternionStart; //Start and end points
   Quatf mCameraQuaternionEnd;
   Quatf mCameraQuaternion;
   int mCameraPosition;

   //Zoom members
   float mZoomTransitionTimer;
   bool mZoomedIn;
   float mFOVStart;
   float mFOVEnd;
   float mFOV;

   Vector3f mCameraVector;
   Vector3f mCameraUp;

   Vector3f mCameraOriginStart;
   Vector3f mCameraOriginEnd;
   Vector3f mCameraOrigin;

   bool mPanCamera; // If false then will rotate instead

public:
   Camera();
   void Tick(TickParameters& tp);
   void RotateLeft();
   void RotateRight();
   void RotateAxis(float x, float y);
   void Pan(float x, float y, float dx, float dy);
   void PanOrRotate(float x, float y, float dx, float dy);

   void ZoomIn();
   void ZoomOut();
   void ZoomToggle();
   bool IsZoomed();

   void PanRotateToggle();
   bool IsPanning();

   Matrix4f GetView();
   Matrix4f GetProjection();
   Matrix4f GetViewProjection();

   void SetResolution(Vector2f resolution);
   Vector2f GetResolution() const;
};
