#ifndef MYMATH
#define MYMATH
//! Rounding error constant often used when comparing f32 values.
#define ROUNDING_ERROR	1e-10

	//! Constant for PI.
#define  PI				3.14159f

	//! Constant for converting bettween degrees and radiants.
#define GRAD_PI		 (180.0/3.1415926535897932384626433832795)

	//! Constant for converting bettween degrees and radiants.
#define GRAD_PI2		 (3.1415926535897932384626433832795/180.0)

	//! returns minimum of two values. Own implementation to get rid of the STL.
	template<class T>
	inline const T min_(const T a, const T b)
	{
		return a < b ? a : b;
	}
	
	//! returns minimum of two values. Own implementation to get rid of the STL
	template<class T>
	inline T max_(const T a, const T b)
	{
		return a < b ? b : a;
	}

	//! returns if a float equals the other one, taking floating 
	//! point rounding errors into account
	inline bool equals(double a, double b)
	{
		return (a + ROUNDING_ERROR > b) && (a - ROUNDING_ERROR < b);
	}

#endif