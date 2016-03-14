#ifndef LINE2D
#define LINE2D
#include"vector2d.h"
template <class T>
class line2d
{
	public:

		line2d(): start(0,0), end(1,1) {};
		line2d(T xa, T ya, T xb, T yb) : start(xa, ya), end(xb, yb) {};
		line2d(const vector2d<T>& start, const vector2d<T>& end) : start(start), end(end) {};
		line2d(const line2d<T>& other) :start(other.start), end(other.end) {};

		// operators

		line2d<T> operator+(const vector2d<T>& point) const { return line2d<T>(start + point, end + point); };
		line2d<T>& operator+=(const vector2d<T>& point) { start += point; end += point; return *this; };

		line2d<T> operator-(const vector2d<T>& point) const { return line2d<T>(start - point, end - point); };
		line2d<T>& operator-=(const vector2d<T>& point) { start -= point; end -= point; return *this; };

		bool operator==(const line2d<T>& other) const { return (start==other.start && end==other.end) || (end==other.start && start==other.end);};
		bool operator!=(const line2d<T>& other) const { return !(start==other.start && end==other.end) || (end==other.start && start==other.end);};

		// functions

		void setLine(const T& xa, const T& ya, const T& xb, const T& yb){start.set(xa, ya); end.set(xb, yb);}
		void setLine(const vector2d<T>& nstart, const vector2d<T>& nend){start.set(nstart); end.set(nend);}
		void setLine(const line2d<T>& line){start.set(line.start); end.set(line.end);}

		//! Returns length of line
		//! \return Returns length of line.
		double getLength() const { return start.getDistanceFrom(end); };

		//! Returns the vector of the line.
		//! \return Returns the vector of the line.
		vector2d<T> getVector() const { return vector2d<T>(end.X - start.X, end.Y - start.Y); };

		//! Tests if this line intersects with an other line.
		//! \param l: Other line to test intersection with.
		//! \param out: If there is an intersection, the location of the intersection will
		//! be stored in this vector.
		//! \return Returns true if there is an intersection, false if not.
		bool intersectWith(const line2d<T>& l, vector2d<T>& out)
		{
			bool found = getInterSectionOfLines(	start.X, start.Y, end.X-start.X, end.Y-start.Y,
													l.start.X, l.start.Y, l.end.X-l.start.X, l.end.Y-l.start.Y,
													out.X, out.Y);
			return (found
				&&	isPointBetweenPoints(out.X, out.Y, start.X, start.Y, end.X, end.Y)
				&&	isPointBetweenPoints(out.X, out.Y, l.start.X, l.start.Y, l.end.X, l.end.Y));
		}

		//! Returns unit vector of the line.
		//! \return Returns unit vector of this line.
		vector2d<T> getUnitVector()
		{
			T len = (T)1.0 / (T)getLength();
			return vector2d<T>((end.X - start.X) * len, (end.Y - start.Y) * len);
		}

		double getAngleWith(const line2d<T>& l)
		{
			vector2d<T> vect = getVector();
			vector2d<T> vect2 = l.getVector();
			return vectorAngle(vect.X, vect.Y, vect2.X, vect2.Y);
		}
		
		// member variables
		
		vector2d<T> start;
		vector2d<T> end;
};
#endif