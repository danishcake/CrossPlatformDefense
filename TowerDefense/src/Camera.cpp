#include "Camera.h"
#include "CommonDefs.h"
#include <GLES2/gl2.h>
#include "Log.h"

static const Quatf makeQuat(const float hdg, const float pitch, const float roll)
{
   return Quatf::fromAxisRot(Vector3f(static_cast<float>(std::sin(DEG2RAD(hdg))), 0, static_cast<float>(std::cos(DEG2RAD(hdg)))), pitch) *
          Quatf::fromAxisRot(Vector3f(0, 1, 0), hdg) *
          Quatf::fromAxisRot(Vector3f(1, 0, 0), roll);
}

const Quatf sQuats[] =
{
   makeQuat(0,    -45, 0),
   makeQuat(45,   -45, 0),
   makeQuat(90,   -45, 0),
   makeQuat(135,  -45, 0),
   makeQuat(180,  -45, 0),
   makeQuat(-135, -45, 0),
   makeQuat(-90,  -45, 0),
   makeQuat(-45,  -45, 0),
   makeQuat(0,    -88, 0),
   makeQuat(0,    -88, -90),
   makeQuat(0,    -88, -180),
   makeQuat(0,    -88, 90)
};

static float clamp(float v, float min, float max)
{
   return v < min ? min : v > max ? max : v;
}

static float smootherstep(float edge0, float edge1, float x)
{
    // Scale, and clamp x to 0..1 range
    x = clamp((x - edge0)/(edge1 - edge0), 0, 1);
    // Evaluate polynomial
    return x*x*x*(x*(x*6 - 15) + 10);
}

const float Camera::sTransitionTime = 0.3f;
const float Camera::sFOVNarrow = 45.0f;
const float Camera::sFOVWide = 90.0f;

Camera::Camera() :
   mTransitionTimer(0), mCameraPosition(0),
   mResolution(320, 240),
   mZoomTransitionTimer(0), mZoomedIn(false),
   mFOVStart(sFOVWide), mFOVEnd(sFOVWide)
{
   mCameraQuaternionStart = sQuats[mCameraPosition];
   mCameraQuaternionEnd   = sQuats[mCameraPosition];
   mCameraQuaternion      = sQuats[mCameraPosition];
}

void Camera::RotateLeft()
{
   mCameraPosition = (mCameraPosition + 1) % (sizeof(sQuats) / sizeof(Quatf));
   mCameraQuaternionStart = mCameraQuaternion;
   mCameraQuaternionEnd = sQuats[mCameraPosition];
   mTransitionTimer = 0;
}

void Camera::RotateRight()
{
   mCameraPosition = (mCameraPosition - 1);
   if(mCameraPosition < 0)
      mCameraPosition = sizeof(sQuats) / sizeof(Quatf) - 1;
   mCameraQuaternionStart = mCameraQuaternion;
   mCameraQuaternionEnd = sQuats[mCameraPosition];
   mTransitionTimer = 0;
   mFOV = sFOVWide;
}

void Camera::RotateAxis(float x, float y)
{
   Quatf rot = Quatf::fromAxisRot(mCameraQuaternion.transform() * Vector3f(0, -1, 0), x * 0.1f) *
               Quatf::fromAxisRot(mCameraQuaternion.transform() * Vector3f(0, 0, -1), y * 0.1f);
   //Quatf rot = Quatf::fromAxisRot(Vector3f(0, -1, 0), x * 0.1f) *
   //            Quatf::fromAxisRot(mCameraQuaternion.transform() * Vector3f(0, 0, -1), y * 0.1f);
   mCameraQuaternion = rot * mCameraQuaternion ;
   mCameraQuaternion.normalize();

   mCameraQuaternionStart = mCameraQuaternion;
   mCameraQuaternionEnd = mCameraQuaternion;
}

void Camera::ZoomIn()
{
   mFOVStart = mFOV;
   mFOVEnd = sFOVNarrow;
   mZoomTransitionTimer = 0;
   mZoomedIn = true;
}

void Camera::ZoomOut()
{
   mFOVStart = mFOV;
   mFOVEnd = sFOVNarrow;
   mZoomTransitionTimer = 0;
   mZoomedIn = false;
}

void Camera::ZoomToggle()
{
   mFOVStart = mFOV;
   mZoomTransitionTimer = 0;

   if(!mZoomedIn)
   {
      mZoomedIn = true;
      mFOVEnd = sFOVNarrow;
   } else if(mZoomedIn)
   {
      mZoomedIn = false;
      mFOVEnd = sFOVWide;
   }
}

void Camera::Tick(TickParameters &tp)
{
   mTransitionTimer += tp.timespan;
   if(mTransitionTimer >= sTransitionTime)
   {
      mCameraQuaternionStart = mCameraQuaternionEnd;
      mTransitionTimer = 0;
   }

   //Now just interpolate between the two to get the camera orientation
   float transition_factor = smootherstep(0, 1, mTransitionTimer / sTransitionTime);
   mCameraQuaternion = mCameraQuaternionStart.slerp(transition_factor, mCameraQuaternionEnd);

   //Now calculate camera position by taking a vector and rotating it by the quaternion
   Matrix4f transform = mCameraQuaternion.transform();
   mCameraVector = transform * Vector3f(-WORLD_WIDTH, 0, 0) + Vector3f(0.5f * WORLD_WIDTH, 0, 0.5f * WORLD_BREADTH);
   mCameraUp = transform * Vector3f(0, 1, 0);


   //Now handle zooming
   mZoomTransitionTimer += tp.timespan;

   float zoom_scalar = 1.0f;
   if(mZoomTransitionTimer >= sTransitionTime)
   {
      mFOVStart = mFOVEnd;
      mZoomTransitionTimer = 0;
   }

   zoom_scalar = smootherstep(0, 1, mZoomTransitionTimer / sTransitionTime);

   mFOV = mFOVStart + (mFOVEnd - mFOVStart) * zoom_scalar;

}

Matrix4f Camera::GetView()
{
   return Matrix4f::createLookAt(mCameraVector, Vector3f(0.5f * WORLD_WIDTH, 0, 0.5f * WORLD_BREADTH), mCameraUp);
}

Matrix4f Camera::GetProjection()
{
   return Matrix4f::createPerspective(mFOV, 1.0f, 0.01f, 10240.0f);
}

Matrix4f Camera::GetViewProjection()
{
   return GetProjection() * GetView();
}

void Camera::SetResolution(Vector2f resolution)
{
   mResolution = resolution;
}

Vector2f Camera::GetResolution() const
{
   return mResolution;
}
