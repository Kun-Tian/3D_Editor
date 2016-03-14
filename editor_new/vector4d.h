#ifndef VECTOR4D
#define VECTOR4D
#include"myMath.h"
template <class T>
	class vector4d  
	{
	public:

		vector4d(): X(0), Y(0), Z(0),W(0) {};
		vector4d(T nx, T ny, T nz,T nw) : X(nx), Y(ny), Z(nz),W(nw) {};
		vector4d(const vector4d<T>& other)	:X(other.X), Y(other.Y), Z(other.Z),W(other.W) {};

		// operators

		vector4d<T> operator-() const { return vector4d<T>(-X, -Y, -Z,-W);   }

		vector4d<T>& operator=(const vector4d<T>& other)	{ X = other.X; Y = other.Y; Z = other.Z; W = other.W; return *this; }

		vector4d<T> operator+(const vector4d<T>& other) const { return vector4d<T>(X + other.X, Y + other.Y, Z + other.Z, W + other.W);	}
		vector4d<T>& operator+=(const vector4d<T>& other)	{ X+=other.X; Y+=other.Y; Z+=other.Z; W+=other.W; return *this; }

		vector4d<T> operator-(const vector4d<T>& other) const { return vector4d<T>(X - other.X, Y - other.Y, Z - other.Z, W - other.W);	}
		vector4d<T>& operator-=(const vector4d<T>& other)	{ X-=other.X; Y-=other.Y; Z-=other.Z; W-=other.W; return *this; }

		vector4d<T> operator*(const vector4d<T>& other) const { return vector4d<T>(X * other.X, Y * other.Y, Z * other.Z, W * other.W);	}
		vector4d<T>& operator*=(const vector4d<T>& other)	{ X*=other.X; Y*=other.Y; Z*=other.Z; W*=other.W; return *this; }
		vector4d<T> operator*(const T v) const { return vector4d<T>(X * v, Y * v, Z * v, W * v);	}
		vector4d<T>& operator*=(const T v) { X*=v; Y*=v; Z*=v; W*=v; return *this; }

		vector4d<T> operator/(const vector4d<T>& other) const { return vector4d<T>(X / other.X, Y / other.Y, Z / other.Z, W / other.W);	}
		vector4d<T>& operator/=(const vector4d<T>& other)	{ X/=other.X; Y/=other.Y; Z/=other.Z; W/=other.W; return *this; }
		vector4d<T> operator/(const T v) const { T i=(T)1.0/v; return vector4d<T>(X * i, Y * i, Z * i, W * i);	}
		vector4d<T>& operator/=(const T v) { T i=(T)1.0/v; X*=i; Y*=i; Z*=i; W*=i; return *this; }

		

		

		// functions

	
		void set(const T nx, const T ny, const T nz, const T nw) {X=nx; Y=ny; Z=nz; W=nw; }
		void set(const vector4d<T>& p) { X=p.X; Y=p.Y; Z=p.Z; W=p.W;}

		//! Returns length of the vector.
		double getLength() const { return sqrt((double)(X*X + Y*Y + Z*Z + W*W)); }

		//! Returns squared length of the vector.
		/** This is useful because it is much faster then
		getLength(). */
		double getLengthSQ() const { return X*X + Y*Y + Z*Z + W*W; }

		//! Returns the dot product with another vector.
		T dotProduct(const vector4d<T>& other) const
		{
			return X*other.X + Y*other.Y + Z*other.Z + W*other.W;
		}

		//! Returns distance from an other point.
		/** Here, the vector is interpreted as point in 3 dimensional space. */
		double getDistanceFrom(const vector4d<T>& other) const
		{
			f64 vx = X - other.X; f64 vy = Y - other.Y; f64 vz = Z - other.Z; f64 vw = W -other.W;
			return sqrt(vx*vx + vy*vy + vz*vz + vw*vw);
		}

		//! Returns squared distance from an other point. 
		/** Here, the vector is interpreted as point in 3 dimensional space. */
		double getDistanceFromSQ(const vector4d<T>& other) const
		{
			f32 vx = X - other.X; f32 vy = Y - other.Y; f32 vz = Z - other.Z; f32 vw = W - other.W;
			return (vx*vx + vy*vy + vz*vz + vw*vw);
		}

		
		//! Returns if this vector interpreted as a point is on a line between two other points.
		/** It is assumed that the point is on the line. */
		bool isBetweenPoints(const vector4d<T>& begin, const vector4d<T>& end) const
		{
			double f = (double)(end - begin).getLengthSQ();
			return (double)getDistanceFromSQ(begin) < f && 
				(double)getDistanceFromSQ(end) < f;
		}

		//! Normalizes the vector.
		vector4d<T>& normalize()
		{
			T l = (T)getLength();
			if (l == 0)
				return *this;

			l = (T)1.0 / l;
			X *= l;
			Y *= l;
			Z *= l;
			W *= l;
			return *this;
		}

		//! Sets the lenght of the vector to a new value
		void setLength(T newlength)
		{
			normalize();
			*this *= newlength;
		}

		//! Inverts the vector.
		void invert()
		{
			X *= -1.0f;
			Y *= -1.0f;
			Z *= -1.0f;
		}

		

		//! Returns interpolated vector.
		/** \param other: other vector to interpolate between
		\param d: value between 0.0f and 1.0f. */
		vector4d<T> getInterpolated(const vector4d<T>& other, double d) const
		{
			double inv = 1.0f - d;
			return vector4d<T>(other.X*inv + X*d,
								other.Y*inv + Y*d,
								other.Z*inv + Z*d,
								other.W*inv + W*d);
		}

	
	


		// member variables

		T X, Y, Z,W;
	};


	//! Typedef for a f32 3d vector.
	typedef vector4d<double> vector4df;
	//! Typedef for an integer 3d vector.
	typedef vector4d<int> vector4di;
#endif