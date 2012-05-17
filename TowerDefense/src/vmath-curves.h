#pragma once

#include "vmath.h"
#include <vector>

template <class POINT_TYPE>
struct TimePosition
{
public:
   TimePosition()
   {
   }

   POINT_TYPE position;
   float time;
};


/*
 * Represents a Cubic Hermite spline that can be automatically interpolated in a
 * variety of ways, in a templated fashion
 */
template <class INTERPOLATOR_TYPE, class POINT_TYPE>
class CubicSpline
{
private:
   INTERPOLATOR_TYPE mInterpolator;
   std::vector<TimePosition<POINT_TYPE> > mPoints;
public:

   POINT_TYPE GetValue(float time)
   {
      // Find interval
      float t0 = 0.0f;
      float t1 = 0.0f;
      int   k0 = -1;
      int   k1 = -1;
      POINT_TYPE p0;
      POINT_TYPE p1;


      for (std::vector<TimePosition<POINT_TYPE> >::iterator it = mPoints.begin(); it != mPoints.end(); ++it)
      {
         if (it->time >= time)
         {
            t1 = it->time;
            p1 = it->position;
            k1 = it - mPoints.begin();
            break;
         }
         if (it->time < time)
         {
            t0 = it->time;
            p0 = it->position;
            k0 = it - mPoints.begin();
         }
      }

      // If at the end or only one point
      if (k1 == -1)
      {
         return p0;
      }
      if (k0 == -1)
      {
         return p1;
      }

      // Produce scaled interpolant
      float t = (time - t0) / (t1 - t0);

      float h00 =  2 * t*t*t - 3*t*t + 1;
      float h10 =      t*t*t - 2*t*t + t;
      float h01 = -2 * t*t*t + 3*t*t;
      float h11 =      t*t*t - t*t;


      return p0 * h00 + mInterpolator(k0, mPoints) * h10 +
             p1 * h01 + mInterpolator(k1, mPoints) * h11;
   }

   /* Gets a reference to the interpolator, exposing its state */
   INTERPOLATOR_TYPE& Interpolator()
   {
      return mInterpolator;
   }

   std::vector<TimePosition<POINT_TYPE> >& Points()
   {
      return mPoints;
   }


   /* Adds a point. Must be called in order with cumulative time */
   void AddPoint(POINT_TYPE point, float time)
   {
      TimePosition<POINT_TYPE> tp;
      tp.position = point;
      tp.time = time;
      mPoints.push_back(tp);
   }
};

template <class POINT_TYPE>
class CardinalInterpolator
{
private:
   float mOneMinusTension;

public:
   CardinalInterpolator() : 
      mOneMinusTension(0.5f)
   {}

   void SetTension(float tension)
   {
      mOneMinusTension = std::max(0.0f, 1.0f - tension);
      mOneMinusTension = std::min(1.0f, mOneMinusTension);
   }

   float GetTension() const
   {
      return 1.0f - mOneMinusTension;
   }

   // Gets the Catmull Rom tangents
   POINT_TYPE operator()(int k, std::vector<TimePosition<POINT_TYPE> >& points)
   {
      int kp1 = std::max(k - 1, 0);
      int km1 = std::min(k + 1, (int)points.size() - 1);

      POINT_TYPE pm = points[kp1].position - points[km1].position;
      float      dt = points[kp1].time     - points[km1].time;
      return pm * (mOneMinusTension / dt);
   }
};

typedef CubicSpline<CardinalInterpolator<Vector4f>, Vector4f> CardinalSpline4f;
typedef CubicSpline<CardinalInterpolator<Vector4d>, Vector4d> CardinalSpline4d;

typedef CubicSpline<CardinalInterpolator<Vector3f>, Vector3f> CardinalSpline3f;
typedef CubicSpline<CardinalInterpolator<Vector3d>, Vector3d> CardinalSpline3d;

typedef CubicSpline<CardinalInterpolator<Vector2f>, Vector2f> CardinalSpline2f;
typedef CubicSpline<CardinalInterpolator<Vector2d>, Vector2d> CardinalSpline2d;

typedef CubicSpline<CardinalInterpolator<float>, float> CardinalSplinef;
typedef CubicSpline<CardinalInterpolator<double>, double> CardinalSplined;

