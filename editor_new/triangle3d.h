#ifndef TRIANGLE3D
#define TRIANGLE3D
#include "vector3d.h"
#include "line3d.h"
template <class T>
	class triangle3d  
	{
	public:

	
		bool operator==(const triangle3d<T>& other) const { return other.pointA==pointA && other.pointB==pointB && other.pointC==pointC; }
		bool operator!=(const triangle3d<T>& other) const { return other.pointA!=pointA || other.pointB!=pointB || other.pointC!=pointC; }

		//! Returns the closest point on a triangle to a point on the same plane.
		//! \param p: Point which must be on the same plane as the triangle.
		vector3df closestPointOnTriangle(const vector3df& p) const
		{
			vector3df rab = line3d<double>(pointA, pointB).getClosestPoint(p);
			vector3df rbc = line3d<double>(pointB, pointC).getClosestPoint(p);
			vector3df rca = line3d<double>(pointC, pointA).getClosestPoint(p);

			T d1 = (T)rab.getDistanceFrom(p);
			T d2 = (T)rbc.getDistanceFrom(p);
			T d3 = (T)rca.getDistanceFrom(p);

			if (d1 < d2)
				return d1 < d3 ? rab : rca;
            
			return d2 < d3 ? rbc : rca;
		}

		//! Returns if a point is inside the triangle
		//! \param p: Point to test. Assumes that this point is already on the plane
		//! of the triangle.
		//! \return Returns true if the point is inside the triangle, otherwise false.
		bool isPointInside(const vector3d<T>& p) const
		{
			return (isOnSameSide(p, pointA, pointB, pointC) &&
				isOnSameSide(p, pointB, pointA, pointC) &&
				isOnSameSide(p, pointC, pointA, pointB));
		}

		//! Returns if a point is inside the triangle. This method is an implementation
		//! of the example used in a paper by Kasper Fauerby original written
		//! by Keidy from Mr-Gamemaker.
		//! \param p: Point to test. Assumes that this point is already on the plane
		//! of the triangle.
		//! \return Returns true if the point is inside the triangle, otherwise false.
		bool isPointInsideFast(const vector3d<T>& p) const
		{
			vector3d<T> f = pointB - pointA;
			vector3d<T> g = pointC - pointA;

			double a = f.dotProduct(f);
			double b = f.dotProduct(g);
			double c = g.dotProduct(g);

            double ac_bb = (a*c)-(b*b);
			vector3d<T> vp = p - pointA;

			double d = vp.dotProduct(f);
			double e = vp.dotProduct(g);
			double x = (d*c)-(e*b);
			double y = (e*a)-(d*b);
			double z = x+y-ac_bb;

			return (( ((unsigned int&)z)& ~(((unsigned int&)x)|((unsigned int&)y))) & 0x80000000)!=0;
		}


		bool isOnSameSide(const vector3d<T>& p1, const vector3d<T>& p2, 
			const vector3d<T>& a, const vector3d<T>& b) const
		{
			vector3d<T> bminusa = b - a;
			vector3d<T> cp1 = bminusa.crossProduct(p1 - a);
			vector3d<T> cp2 = bminusa.crossProduct(p2 - a);
			return (cp1.dotProduct(cp2) > 0.0f);
		}


		//! Returns an intersection with a 3d line.
		//! \param lineVect: Vector of the line to intersect with.
		//! \param linePoint: Point of the line to intersect with.
		//! \param outIntersection: Place to store the intersection point, if there is one.
		//! \return Returns true if there was an intersection, false if there was not.
		bool getIntersectionWithLimitedLine(const line3d<T>& line,
			vector3d<T>& outIntersection) const
		{
			return getIntersectionWithLine(line.start,
				line.getVector(), outIntersection) &&
				outIntersection.isBetweenPoints(line.start, line.end);
		}


		//! Returns an intersection with a 3d line.
		//! Please note that also points are returned as intersection, which
		//! are on the line, but not between the start and end point of the line.
		//! If you want the returned point be between start and end, please
		//! use getIntersectionWithLimitedLine().
		//! \param lineVect: Vector of the line to intersect with.
		//! \param linePoint: Point of the line to intersect with.
		//! \param outIntersection: Place to store the intersection point, if there is one.
		//! \return Returns true if there was an intersection, false if there was not.
		bool getIntersectionWithLine(const vector3d<T>& linePoint,
			const vector3d<T>& lineVect, vector3d<T>& outIntersection) const
		{
			if (getIntersectionOfPlaneWithLine(linePoint, lineVect, outIntersection))
				return isPointInside(outIntersection);

			return false;			
		}


		//! Calculates the intersection between a 3d line and 
		//! the plane the triangle is on.
		//! \param lineVect: Vector of the line to intersect with.
		//! \param linePoint: Point of the line to intersect with.
		//! \param outIntersection: Place to store the intersection point, if there is one.
		//! \return Returns true if there was an intersection, false if there was not.
		bool getIntersectionOfPlaneWithLine(const vector3d<T>& linePoint,
			const vector3d<T>& lineVect, vector3d<T>& outIntersection) const
		{
			vector3d<T> normal = getNormal();
			T t2 = normal.dotProduct(lineVect);
            
			if (t2 == 0.0f)
				return false;

            T d = pointA.dotProduct(normal);
			T t =- (normal.dotProduct(linePoint) - d) / t2;
			outIntersection = linePoint + (lineVect * t);
			return true;
		}

		
		//! Returns the normal of the triangle.
		//! Please note: The normal is not normalized.
		vector3d<T> getNormal() const
		{
			return (pointB - pointA).crossProduct(pointC - pointA);
		}

		//! Returns if the triangle is front of backfacing.
		//! \param lookDirection: Look direction.
		//! \return Returns true if the plane is front facing, which mean it would
		//! be visible, and false if it is backfacing.
		bool isFrontFacing(const vector3d<T>& lookDirection) const
		{
			vector3d<T> n = getNormal();
			n.normalize();
			return n.dotProduct(lookDirection) <= 0.0f;
		}

	
		void set(const vector3d<T>& a, const vector3d<T>& b, const vector3d<T>& c)
		{
			pointA = a;
			pointB = b;
			pointC = c;
		}

		//! the three points of the triangle
		vector3d<T> pointA; 
		vector3d<T> pointB; 
		vector3d<T> pointC; 
	};


	//! Typedef for a double 3d triangle.
	typedef triangle3d<double> triangle3df;

	//! Typedef for an integer 3d triangle.
	typedef triangle3d<int> triangle3di;
#endif