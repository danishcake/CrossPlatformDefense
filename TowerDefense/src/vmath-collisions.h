#pragma once

#include "vmath.h"
#include <cmath>
#include <cfloat>
#include <algorithm>

/* Sign extractor */
template <typename T> T sgn(T val) {
    return (T)((T(0) < val) - (val < T(0)));
}


/**
* Class for 2D collisions.
* @note If you use a Vector3 derivative then the z component is ignored
*/
template <class T>
class Collisions2
{
public:
	/**
	* Finds the square of the distance between two points
	* @param a Position of first point
	* @param b Position of second point
	*/
	static T DistanceSqr(const Vector2<T> a, const Vector2<T> b)
	{
		T x = a.x - b.x;
		T y = a.y - b.y;

		return x * x + y * y;
	}


	/**
	* Finds the square of the distance between two points
	* @param a Position of first point
	* @param b Position of second point
	*/
	static T DistanceSqr(const Vector3<T> a, const Vector3<T> b)
	{
		T x = a.x - b.x;
		T y = a.y - b.y;

		return x * x + y * y;
	}

	/**
	* Finds the distance between two points
	* @param a Position of first point
	* @param b Position of second point
	*/
	static T Distance(const Vector2<T> a, const Vector2<T> b)
	{
		T x = a.x - b.x;
		T y = a.y - b.y;

		return (T)std::sqrt((double)(x * x + y * y));
	}

	/**
	* Finds the distance between two points
	* @param a Position of first point
	* @param b Position of second point
	*/
	static T Distance(const Vector3<T> a, const Vector3<T> b)
	{
		T x = a.x - b.x;
		T y = a.y - b.y;

		return (T)std::sqrt((double)(x * x + y * y));
	}

	/**
	* Determines if a point is within a circle
	* @param a Point being tested
	* @param b Centre of circle
	* @param radius Radius of circle
	*/
	static bool PointInCircle(const Vector2<T> a, const Vector2<T> b, const T radius)
	{
		T x = a.x - b.x;
		T y = a.y - b.y;
		return x * x + y * y < radius *  radius;
	}

	/**
	* Determines if a point is within a circle
	* @param a Point being tested
	* @param b Centre of circle
	* @param radius Radius of circle
	*/
	static bool PointInCircle(const Vector3<T> a, const Vector3<T> b, const T radius)
	{
		T x = a.x - b.x;
		T y = a.y - b.y;
		return x * x + y * y < radius *  radius;
	}

	/**
	* Determines if a point is within a circle
	* @param a Centre of circle A
	* @param radius_a Radius of circle A
	* @param b Centre of circle B
	* @param radius_b Radius of circle B
	*/
	static bool CirclesIntersect(const Vector2<T> a, const T radius_a, const Vector2<T> b, const T radius_b)
	{
		T x = a.x - b.x;
		T y = a.y - b.y;
		T r = (radius_a + radius_b);

		return x * x + y * y < r * r;
	}

	/**
	* Determines if a point is within a circle
	* @param a Centre of circle A
	* @param radius_a Radius of circle A
	* @param b Centre of circle B
	* @param radius_b Radius of circle B
	*/
	static bool CirclesIntersect(const Vector3<T> a, const T radius_a, const Vector3<T> b, const T radius_b)
	{
		T x = a.x - b.x;
		T y = a.y - b.y;
		T r = (radius_a + radius_b);

		return x * x + y * y < r * r;
	}

	/**
	* Determines if line segment intersects with a triangle.
	* @param a_1 Point 1 in triangle
	* @param a_2 Point 2 in triangle
	* @param a_3 Point 3 in triangle
	* @param b_1 Start of line B
	* @param b_2 End of line B
	*/
	static bool LineTriangleIntersect(const Vector2<T> a_1, const Vector2<T> a_2, const Vector2<T> a_3, const Vector2<T> b_1, const Vector2<T> b_2)
	{
		Vector2<T> out; //Ignored
		if(Collisions2<T>::LineSegmentsIntersect(a_1, a_2, b_1, b_2, &out) ||
		   Collisions2<T>::LineSegmentsIntersect(a_1, a_3, b_1, b_2, &out) ||
		   Collisions2<T>::LineSegmentsIntersect(a_2, a_3, b_1, b_2, &out))
		   return true;
		return false;

	}
	
	/**
	* Determines if line segment intersects with a triangle.
	* @param a_1 Point 1 in triangle
	* @param a_2 Point 2 in triangle
	* @param a_3 Point 3 in triangle
	* @param b_1 Start of line B
	* @param b_2 End of line B
	* @param out Intersection point.
	*/
	
	static bool LineTriangleIntersect(const Vector3<T> a_1, const Vector3<T> a_2, const Vector3<T> a_3, const Vector3<T> b_1, const Vector3<T> b_2)
	{
		Vector3<T> out; //Ignored
		if(Collisions2<T>::LineSegmentsIntersect(a_1, a_2, b_1, b_2, out) ||
		   Collisions2<T>::LineSegmentsIntersect(a_1, a_3, b_1, b_2, out) ||
		   Collisions2<T>::LineSegmentsIntersect(a_2, a_3, b_1, b_2, out))
		   return true;
		return false;
	}


	/**
	* Determines if two line segments intersect. If true then out is set to intersection point
	* @param a_1 Start of line A
	* @param a_2 End of line A
	* @param b_1 Start of line B
	* @param b_2 End of line B
	* @param out Intersection point.
	*/
	static bool LineSegmentsIntersect(const Vector2<T> a_1, const Vector2<T> a_2, const Vector2<T> b_1, const Vector2<T> b_2, Vector2<T>& out)
	{
		Vector2<T> da = a_2 - a_1;
		Vector2<T> db = b_2 - b_1;
		T d = db.y * da.x - db.x * da.y;

		if(d != 0)
		{
			T u_a = ((db.x * (a_1.y - b_1.y)) - (db.y * (a_1.x - b_1.x))) / d;
			T u_b = ((da.x * (a_1.y - b_1.y)) - (da.y * (a_1.x - b_1.x))) / d;

			if(u_a < 1 && u_a > 0 && u_b > 0 && u_b < 1)
			{
				out.x = a_1.x + u_a * da.x;
				out.y = a_1.y + u_a * da.y;
				return true;
			}
			else
			{
				return false;
			}

		}
		else //Parallel lines
		{
			return false;
		}
	}
	/**
	* Determines if two line segments intersect. If true then out is set to intersection point
	* @param a_1 Start of line A
	* @param a_2 End of line A
	* @param b_1 Start of line B
	* @param b_2 End of line B
	* @param out Intersection point.
	*/
	static bool LineSegmentsIntersect(const Vector3<T> a_1, const Vector3<T> a_2, const Vector3<T> b_1, const Vector3<T> b_2, Vector3<T>& out)
	{
		Vector3<T> da = a_2 - a_1;
		Vector3<T> db = b_2 - b_1;
		T d = db.y * da.x - db.x * da.y;

		if(d != 0)
		{
			T u_a = ((db.x * (a_1.y - b_1.y)) - (db.y * (a_1.x - b_1.x))) / d;
			T u_b = ((da.x * (a_1.y - b_1.y)) - (da.y * (a_1.x - b_1.x))) / d;

			if(u_a < 1 && u_a > 0 && u_b > 0 && u_b < 1)
			{
				out.x = a_1.x + u_a * da.x;
				out.y = a_1.y + u_a * da.y;
				return true;
			}
			else
			{
				return false;
			}

		}
		else //Parallel lines
		{
			return false;
		}
	}

	/**
	* Determines if a point is with a triangle.
	* @param a Triangle point a
	* @param b Triangle point b
	* @param c Triangle point c
	* @param p The point to test
	* @note Using algorithm described at http://www.mathforum.org/library/drmath/view/54386.html
	*/
	static bool PointInTriangle(const Vector2<T> a, const Vector2<T> b, const Vector2<T> c, const Vector2<T> p)
	{
		T abp = TriangleAreaDouble(a,b,p);
		T bcp = TriangleAreaDouble(b,c,p);
		T cap = TriangleAreaDouble(c,a,p);
		return ((abp > 0) && (bcp > 0) && (cap >0)) || ((abp < 0) && (bcp < 0) && (cap < 0));
	}
	/**
	* Determines if a point is with a triangle.
	* @param a Triangle point a
	* @param b Triangle point b
	* @param c Triangle point c
	* @param p The point to test
	* @note Using algorithm described at http://www.mathforum.org/library/drmath/view/54386.html
	*/
	static bool PointInTriangle(const Vector3<T> a, const Vector3<T> b, const Vector3<T> c, const Vector3<T> p)
	{
		T abp = TriangleAreaDouble(a,b,p);
		T bcp = TriangleAreaDouble(b,c,p);
		T cap = TriangleAreaDouble(c,a,p);
		return ((abp > 0) && (bcp > 0) && (cap >0)) || ((abp < 0) && (bcp < 0) && (cap < 0));
	}
	/**
	* Determines if a point is with a triangle.
	* @param a Pointer to array of Vector2 describing a convex hull
	* @param num_points The number of Vector2 in a
	* @param p The point to be tested
	*/
	static bool PointInConvex(const Vector2<T>* a, const int num_points, const Vector2<T> p)
	{
		bool allPositive = true;
		bool allNegative = true;
		for(int i = 0; i < num_points-1; i++)
		{
			T aread = TriangleAreaDouble(a[i], a[i+1], p);
			if(aread > 0)
				allNegative = false;
			if(aread < 0)
				allNegative = false;
		}
		T aread2 = TriangleAreaDouble(a[num_points-1], a[0], p);
		if(aread2 > 0)
			allNegative = false;
		if(aread2 < 0)
			allNegative = false;
		return allNegative || allPositive;
	}
   /**
   * Determines if a line segment enters a circle
   * @param P1 The first point in the line segment
   * @param P2 The second point in the line segment
   * @param c The centre of the circle
   * @param radius The radius of the circle
   * @param out The point of closest approach
   */
   static bool LineInCircle(const Vector2<T> P1, const Vector2<T> P2, const Vector2<T> c, const T radius)
   {
      Vector2<T> L = P2 - P1;
	  Vector2<T> out;
      T beta = ((c.x - P1.x) * L.x - (P1.y - c.y) * L.y) / L.lengthSq();
      if(beta < 0)
         out = P1;
      else if (beta >1)
         out = P2;
      else
         out = P1 + beta * L;
      return DistanceSqr(out, c) < radius * radius;
   }

   /**
   * Determines if a line segment enters a circle
   * @param P1 The first point in the line segment
   * @param P2 The second point in the line segment
   * @param c The centre of the circle
   * @param radius The radius of the circle
   * @param out The point of closest approach
   */
   static bool LineInCircle(const Vector3<T> P1, const Vector3<T> P2, const Vector3<T> c, const T radius)
   {
      Vector3<T> L = P2 - P1;
	  Vector3<T> out;
      T beta = ((c.x - P1.x) * L.x - (P1.y - c.y) * L.y) / L.lengthSq();
      if(beta < 0)
         out = P1;
      else if (beta >1)
         out = P2;
      else
         out = P1 + L * beta;
      return DistanceSqr(out, c) < radius * radius;
   }


   /**
   * Determines if a line segment enters a circle
   * @param P1 The first point in the line segment
   * @param P2 The second point in the line segment
   * @param c The centre of the circle
   * @param radius The radius of the circle
   * @param out The point of closest approach. Only set if intersection occurs
   */
   static bool LineInCircle(const Vector2<T> P1, const Vector2<T> P2, const Vector2<T> centre, const T radius, Vector2<T>& out)
   {
      Vector2<T> start = P1 - centre;
      Vector2<T> v = P2 - P1;
      T a = v.lengthSq();
      T b = 2 * start.dotProduct(v);
      T c = start.lengthSq() -  radius * radius;

      T discriminant = b * b - 4 * a * c;
      if(discriminant > 0 && a != 0)
      {
         //Parameterised roots solved, line intersect first at smallest position T
         T dsqrt = std::sqrt(discriminant);
         T t1 = (-b + dsqrt) / (2 * a);
         T t2 = (-b - dsqrt) / (2 * a);
         if(t1 < t2 && t1 >= 0 && t1 <= 1)
         {
            out = P1 + v * t1;
            return true;
         } else if(t2 < t1 && t2 >= 0 && t2 <= 1)
         {
            out = P1 + v * t2;
            return true;
         }
      }
      return false;
   }

   /**
   * Determines if a line segment enters a circle
   * @param P1 The first point in the line segment
   * @param P2 The second point in the line segment
   * @param c The centre of the circle
   * @param radius The radius of the circle
   * @param out The point of closest approach
   */
   //TODO fix
   static bool LineInCircle(const Vector3<T> P1, const Vector3<T> P2, const Vector3<T> centre, const T radius, Vector3<T>& out)
   {
      Vector3<T> start = P1 - centre;
      Vector3<T> v = P2 - P1;
      T a = v.lengthSq();
      T b = 2 * start.dotProduct(v);
      T c = start.lengthSq() -  radius * radius;

      T discriminant = b * b - 4 * a * c;
      if(discriminant > 0 && a != 0)
      {
         //Parameterised roots solved, line intersect first at smallest position T
         T dsqrt = std::sqrt(discriminant);
         T t1 = (-b + dsqrt) / (2 * a);
         T t2 = (-b - dsqrt) / (2 * a);
         if(t1 < t2 && t1 >= 0 && t1 <= 1)
         {
            out = P1 + v * t1;
            return true;
         } else if(t2 < t1 && t2 >= 0 && t2 <= 1)
         {
            out = P1 + v * t2;
            return true;
         }
      }
      return false;
   }

   /**
     * Determines if a line segment intersects with a polygon. Returns on first encountered point
     * @param P1 The first point in the line segment
     * @param P2 The second point in the line segment
     * @param hull A pointer to an array of points forming the hull
     * @param num_points The number of poits in the hull
	 */
   static bool LineInPolygon(const Vector3<T> P1, const Vector3<T> P2, const Vector3<T>* hull, const int num_points)
   {
      Vector3<T> intersectionPoint;
      for(int i = 0; i < num_points-1; i++)
      {
         if(LineSegmentsIntersect(P1, P2, hull[i], hull[i+1], intersectionPoint))
            return true;
      }
      return false;
   }
   
      /**
     * Determines if a line segment intersects with a polygon. Returns on first encountered point
     * @param P1 The first point in the line segment
     * @param P2 The second point in the line segment
     * @param hull A pointer to an array of points forming the hull
     * @param num_points The number of poits in the hull
     */
   static bool LineInPolygon(const Vector2<T> P1, const Vector2<T> P2, const Vector2<T>* hull, const int num_points)
   {
      Vector2<T> intersectionPoint;
      for(int i = 0; i < num_points-1; i++)
      {
         if(LineSegmentsIntersect(P1, P2, hull[i], hull[i+1], intersectionPoint))
            return true;
      }
      return false;
   }
   
   /**
     * Determines if a line segment intersects with a polygon. Returns on after testing all points
     * @param P1 The first point in the line segment
     * @param P2 The second point in the line segment
     * @param hull A pointer to an array of points forming the hull
     * @param num_points The number of poits in the hull
     * @param out The first point along P1-P2
     */
   static bool LineInPolygon(const Vector3<T> P1, const Vector3<T> P2, const Vector3<T>* hull, const int num_points, Vector3<T>& out)
   {
      Vector3<T> intersectionPoint;
      float lengthSqOfIntersection = FLT_MAX;
      bool has_intersected = false;
      for(int i = 0; i < num_points; i++)
      {
         if(LineSegmentsIntersect(P1, P2, hull[i], hull[i==num_points-1? 0:i+1], intersectionPoint))
         {
            float lengthSq = (intersectionPoint - P1).lengthSq();
            if(lengthSq < lengthSqOfIntersection)
            {
               lengthSqOfIntersection = lengthSq;
               has_intersected = true;
               out = intersectionPoint;
            }
         }
      }
      return has_intersected;
   }

         /**
     * Determines if two polygon meshes specified as a list of triangles overlap. Returns on first encountered point
     * @param mesh_a The fist triangle list
     * @param mesh_b The second triangle list
     * @param num_triangles_a The number of triangles in the mesh
     * @param num_triangles_b The number of triangles in the mesh
     */
   static bool PolygonIntersectsPolygon(const Vector2<T>* mesh_a, const int num_triangles_a, const Vector2<T>* mesh_b, const int num_triangles_b)
   {
      Vector2<T> intersectionPoint;
      for(int i = 0; i < num_triangles_a; i++)
      {
         for(int j = 0; j < num_triangles_b; j++)
         {
             if(LineSegmentsIntersect(mesh_a[i * 3 + 0],   mesh_a[i * 3 + 1], mesh_b[j * 3 + 0],  mesh_b[j * 3 + 1], intersectionPoint) ||
                LineSegmentsIntersect(mesh_a[i * 3 + 1],   mesh_a[i * 3 + 2], mesh_b[j * 3 + 0],  mesh_b[j * 3 + 1], intersectionPoint) ||
                LineSegmentsIntersect(mesh_a[i * 3 + 0],   mesh_a[i * 3 + 2], mesh_b[j * 3 + 0],  mesh_b[j * 3 + 1], intersectionPoint) ||
                LineSegmentsIntersect(mesh_a[i * 3 + 0],   mesh_a[i * 3 + 1], mesh_b[j * 3 + 1],  mesh_b[j * 3 + 2], intersectionPoint) ||
                LineSegmentsIntersect(mesh_a[i * 3 + 1],   mesh_a[i * 3 + 2], mesh_b[j * 3 + 1],  mesh_b[j * 3 + 2], intersectionPoint) ||
                LineSegmentsIntersect(mesh_a[i * 3 + 0],   mesh_a[i * 3 + 2], mesh_b[j * 3 + 1],  mesh_b[j * 3 + 2], intersectionPoint) ||
                LineSegmentsIntersect(mesh_a[i * 3 + 0],   mesh_a[i * 3 + 1], mesh_b[j * 3 + 0],  mesh_b[j * 3 + 2], intersectionPoint) ||
                LineSegmentsIntersect(mesh_a[i * 3 + 1],   mesh_a[i * 3 + 2], mesh_b[j * 3 + 0],  mesh_b[j * 3 + 2], intersectionPoint) ||
                LineSegmentsIntersect(mesh_a[i * 3 + 0],   mesh_a[i * 3 + 2], mesh_b[j * 3 + 0],  mesh_b[j * 3 + 2], intersectionPoint))
                return true;
         }
      }

      return false;
   }
   
         /**
     * Determines if two polygon meshes specified as a list of triangles overlap. Returns on first encountered point
     * @param mesh_a The fist triangle list
     * @param mesh_b The second triangle list
     * @param num_triangles_a The number of triangles in the mesh
     * @param num_triangles_b The number of triangles in the mesh
     */
   static bool PolygonIntersectsPolygon(const Vector3<T>* mesh_a, const int num_triangles_a, const Vector3<T>* mesh_b, const int num_triangles_b)
   {
      Vector3<T> intersectionPoint;
      for(int i = 0; i < num_triangles_a; i++)
      {
         for(int j = 0; j < num_triangles_b; j++)
         {
             if(LineSegmentsIntersect(mesh_a[i * 3 + 0],   mesh_a[i * 3 + 1], mesh_b[j * 3 + 0],  mesh_b[j * 3 + 1], intersectionPoint) ||
                LineSegmentsIntersect(mesh_a[i * 3 + 1],   mesh_a[i * 3 + 2], mesh_b[j * 3 + 0],  mesh_b[j * 3 + 1], intersectionPoint) ||
                LineSegmentsIntersect(mesh_a[i * 3 + 0],   mesh_a[i * 3 + 2], mesh_b[j * 3 + 0],  mesh_b[j * 3 + 1], intersectionPoint) ||
                LineSegmentsIntersect(mesh_a[i * 3 + 0],   mesh_a[i * 3 + 1], mesh_b[j * 3 + 1],  mesh_b[j * 3 + 2], intersectionPoint) ||
                LineSegmentsIntersect(mesh_a[i * 3 + 1],   mesh_a[i * 3 + 2], mesh_b[j * 3 + 1],  mesh_b[j * 3 + 2], intersectionPoint) ||
                LineSegmentsIntersect(mesh_a[i * 3 + 0],   mesh_a[i * 3 + 2], mesh_b[j * 3 + 1],  mesh_b[j * 3 + 2], intersectionPoint) ||
                LineSegmentsIntersect(mesh_a[i * 3 + 0],   mesh_a[i * 3 + 1], mesh_b[j * 3 + 0],  mesh_b[j * 3 + 2], intersectionPoint) ||
                LineSegmentsIntersect(mesh_a[i * 3 + 1],   mesh_a[i * 3 + 2], mesh_b[j * 3 + 0],  mesh_b[j * 3 + 2], intersectionPoint) ||
                LineSegmentsIntersect(mesh_a[i * 3 + 0],   mesh_a[i * 3 + 2], mesh_b[j * 3 + 0],  mesh_b[j * 3 + 2], intersectionPoint))
                return true;
         }
      }

      return false;
   }
   
private:
	/**
	* Gets twice the area of a triangle using the determinant method.
	* @param a Triangle point a
	* @param b Triangle point b
	* @param c Triangle point c
	* @note If described CW then the result is positive, if CCW then negative
	*/
	static T TriangleAreaDouble(const Vector2<T> a, const Vector2<T> b, const Vector2<T> c)
	{
		return b.x * c.y - b.y * c.x -
			   a.x * c.y + a.y * c.x +
			   a.x * b.y - a.y * b.x;
	}

	/**
	* Gets twice the area of a triangle using the determinant method.
	* @param a Triangle point a
	* @param b Triangle point b
	* @param c Triangle point c
	* @note If described CW then the result is positive, if CCW then negative
	*/
	static T TriangleAreaDouble(const Vector3<T> a, const Vector3<T> b, const Vector3<T> c)
	{
		return b.x * c.y - b.y * c.x -
			   a.x * c.y + a.y * c.x +
			   a.x * b.y - a.y * b.x;
	}
};

typedef Collisions2<float> Collisions2f;
typedef Collisions2<double> Collisions2d;



/*
 * Class for 3D collisions
 * @note If you use a Vector4 derivative then the z component is ignored
 */
template <class T>
class Collisions3
{
public:
   /*
    * Finds the intersection between a ray and an AABB
    * Algorithm as per Realtime Collision Detection Page 181
    * Intersection point is only valid if true returned
    * Ditto normal
    * TODO: FLT_MAX init should account for double overload
    */
   static bool RayIntersectsAABB(const Vector3<T> ray_origin, const Vector3<T> ray_unit, 
                                 const Vector3<T> AABB_origin, const Vector3<T> AABB_size,
                                 Vector3<T>& intersection, T& intersection_distance,
                                 Vector3<T>& normal)
   {
      T t_min = 0;
      T t_max = FLT_MAX;
      Vector3<T> AABB_end = AABB_origin + AABB_size;

      for (int i = 0; i < 3; i++)
      {
         // Ray parallel to slab in this axis. Only possible to hit
         // if origin in slab
         if (std::abs(ray_unit[i]) < EPSILON)
         {
            if(ray_origin[i] < AABB_origin[i] || ray_origin[i] > AABB_end[i])
            {
               return false;
            }
         } else
         {
            T ood = 1.0f / ray_unit[i];
            T t1 = (AABB_origin[i] - ray_origin[i]) * ood;
            T t2 = (AABB_end[i]    - ray_origin[i]) * ood;

            // Ensure t1 is the intersection to the near plane and t2 with far plane
            if (t1 > t2)
            {
               std::swap(t1, t2);
            }

            // Expand the limits
            if(t1 > t_min)
            {
               normal = Vector3<T>();
               normal[i] = sgn<float>(ray_unit[i]);
               t_min = t1;
            }
            //t_min = std::max(t_min, t1);
            t_max = std::min(t_max, t2);

            // When the minimum ray length is greater than the maximum then a collision can't occur
            if (t_min > t_max) return false;
         }
      }
      // By here a collision is confirmed
      intersection = ray_origin + ray_unit * t_min;
      intersection_distance = t_min;
      return true;
   }
   /*
    * Find the intersection between a ray and a sphere
    * Algorithm as per Realtime Collision Detection Page 179
    */

   static bool RayIntersectsSphere(const Vector3<T> ray_origin, const Vector3<T> ray_unit,
                                   const Vector3<T> sphere_origin, T sphere_radius)
   {
      Vector3<T> m = ray_origin - sphere_origin;
      T c = m.dotProduct(m) - sphere_radius * sphere_radius;
      if (c <= (T)0.0) return true;

      T b = m.dotProduct(ray_unit);
      if (b > (T)0.0) return false;

      T disc = b * b - c;
      return disc >= (T)0.0;
   }
};

typedef Collisions3<float> Collisions3f;
typedef Collisions3<double> Collisions3d;
