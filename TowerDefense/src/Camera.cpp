#include "Camera.h"
#include "CommonDefs.h"
#include <GLES2/gl2.h>
#include "Log.h"
#include <algorithm>

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
   mResolution(320, 240), mTransitionTimer(0),
   mCameraPosition(0),
   mZoomTransitionTimer(0), mZoomedIn(false),
   mFOVStart(sFOVWide), mFOVEnd(sFOVWide),
   mCameraOriginStart(WORLD_WIDTH / 2, 0, WORLD_BREADTH / 2),
   mCameraOriginEnd(WORLD_WIDTH / 2, 0, WORLD_BREADTH / 2),
   mPanCamera(false)
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
   //Quatf rot = Quatf::fromAxisRot(mCameraQuaternion.transform() * Vector3f(0, -1, 0), x * 0.1f) *
   //            Quatf::fromAxisRot(mCameraQuaternion.transform() * Vector3f(0, 0, -1), y * 0.1f);
   Quatf rot = Quatf::fromAxisRot(Vector3f(0, -1, 0), x * 0.1f) *
               Quatf::fromAxisRot(mCameraQuaternion.transform() * Vector3f(0, 0, -1), y * 0.1f);
   mCameraQuaternion = rot * mCameraQuaternion ;
   mCameraQuaternion.normalize();

   mCameraQuaternionStart = mCameraQuaternion;
   mCameraQuaternionEnd = mCameraQuaternion;
}

void Camera::Pan(float x, float y, float dx, float dy)
{
   Vector3f origin;
   Vector3f unit;

   GetRay(Vector2i(static_cast<int>(x), static_cast<int>(y)), origin, unit);
   Vector3f tap_intersection = origin - unit * (origin.y / unit.y);

   GetRay(Vector2i(static_cast<int>(x - dx), static_cast<int>(y - dy)), origin, unit);
   Vector3f drag_intersection = origin - unit * (origin.y / unit.y);


   Vector3f delta = drag_intersection - tap_intersection;

   mCameraOriginEnd += delta;
   mCameraOriginStart += delta;
   mCameraOriginStart.x = std::max(mCameraOriginStart.x, 0.0f);
   mCameraOriginStart.x = std::min(mCameraOriginStart.x, (float)WORLD_WIDTH);
   mCameraOriginStart.z = std::max(mCameraOriginStart.z, 0.0f);
   mCameraOriginStart.z = std::min(mCameraOriginStart.z , (float)WORLD_BREADTH);

   mCameraOriginEnd.x = std::max(mCameraOriginEnd.x, 0.0f);
   mCameraOriginEnd.x = std::min(mCameraOriginEnd.x, (float)WORLD_WIDTH);
   mCameraOriginEnd.z = std::max(mCameraOriginEnd.z, 0.0f);
   mCameraOriginEnd.z = std::min(mCameraOriginEnd.z , (float)WORLD_BREADTH);
}

void Camera::PanOrRotate(float x, float y, float dx, float dy)
{
   if(mPanCamera)
   {
      Pan(x, y, dx, dy);
   } else
   {
      RotateAxis(dx, dy);
   }
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
   mZoomTransitionTimer = 0;
   mFOVStart = mFOV;
   mCameraOriginStart = mCameraOrigin;
   

   if(!mZoomedIn)
   {
      mZoomedIn = true;
      mFOVEnd = sFOVNarrow;
      mCameraOriginEnd = mCameraOrigin;
   } else if(mZoomedIn)
   {
      mZoomedIn = false;
      mFOVEnd = sFOVWide;
      mCameraOriginEnd = Vector3f();
   }
}

void Camera::ZoomScale(float zoom)
{
   mFOVEnd += zoom;
   mFOVStart += zoom;
   mFOVEnd = clamp(mFOVEnd, sFOVNarrow, sFOVWide);
   mFOVStart = clamp(mFOVStart, sFOVNarrow, sFOVWide);
}

bool Camera::IsZoomed()
{
   return mZoomedIn;
}

void Camera::PanRotateToggle()
{
	mPanCamera = !mPanCamera;
}

bool Camera::IsPanning()
{
   return mPanCamera;
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

   //Now handle zooming
   mZoomTransitionTimer += tp.timespan;

   float zoom_scalar = 1.0f;
   if(mZoomTransitionTimer >= sTransitionTime)
   {
      mFOVStart = mFOVEnd;
      mCameraOriginStart = mCameraOriginEnd;
      mZoomTransitionTimer = 0;
   }

   zoom_scalar = smootherstep(0, 1, mZoomTransitionTimer / sTransitionTime);

   mCameraOrigin = mCameraOriginStart + (mCameraOriginEnd - mCameraOriginStart) * zoom_scalar;
   mFOV = mFOVStart + (mFOVEnd - mFOVStart) * zoom_scalar;

   //Now calculate camera position by taking a vector and rotating it by the quaternion
   Matrix4f transform = mCameraQuaternion.transform();
   mCameraVector = transform * Vector3f(-WORLD_WIDTH, 0, 0) + mCameraOrigin;
   mCameraUp = transform * Vector3f(0, 1, 0);
}

Matrix4f Camera::GetView()
{
   return Matrix4f::createLookAt(mCameraVector, mCameraOrigin, mCameraUp);
}

Matrix4f Camera::GetProjection()
{
   return Matrix4f::createTranslation(-(GetAspect() - 1.0f) / 2.0f, 0, 0) *
          Matrix4f::createPerspective(mFOV, GetAspect(), 0.01f, 10240.0f);
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

float Camera::GetAspect() const
{
   return static_cast<float>(mResolution.x) / static_cast<float>(mResolution.y);
}

void Camera::GetRay(Vector2i tap_position, Vector3f& ray_origin, Vector3f& ray_unit)
{
   Matrix4f inv_vp = GetViewProjection().inverse();
   Vector4f tap_pos_screen = Vector4f((tap_position.x - mResolution.x * 0.5f) / (mResolution.x * 0.5f), 
                                     -(tap_position.y - mResolution.y * 0.5f) / (mResolution.y * 0.5f),
                                      1.0f, 1.0f);
   Vector4f tap_pos_world = inv_vp * tap_pos_screen;
   // Reverse the perspective divide
   tap_pos_world /= tap_pos_world.w;

   Vector4f tap_ray = tap_pos_world - mCameraVector;
   Vector3f tap_unit = Vector3f(tap_ray.x, tap_ray.y, tap_ray.z);
   tap_unit.normalize();

   ray_origin = mCameraVector;
   ray_unit = tap_unit;
}

