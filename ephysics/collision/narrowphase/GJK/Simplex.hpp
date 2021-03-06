/** @file
 * Original ReactPhysics3D C++ library by Daniel Chappuis <http://www.reactphysics3d.com/> This code is re-licensed with permission from ReactPhysics3D author.
 * @author Daniel CHAPPUIS
 * @author Edouard DUPIN
 * @copyright 2010-2016, Daniel Chappuis
 * @copyright 2017, Edouard DUPIN
 * @license MPL v2.0 (see license file)
 */
#pragma once

// Libraries
#include <ephysics/mathematics/mathematics.hpp>
#include <etk/Vector.hpp>

/// ReactPhysics3D namespace
namespace ephysics {

// Type definitions
typedef uint32_t Bits;

// Class Simplex
/**
 * This class represents a simplex which is a set of 3D points. This
 * class is used in the GJK algorithm. This implementation is based on
 * the implementation discussed in the book "Collision Detection in 3D
 * Environments". This class implements the Johnson's algorithm for
 * computing the point of a simplex that is closest to the origin and also
 * the smallest simplex needed to represent that closest point.
 */
class Simplex {

	private:

		// -------------------- Attributes -------------------- //

		/// Current points
		vec3 mPoints[4];

		/// pointsLengthSquare[i] = (points[i].length)^2
		float mPointsLengthSquare[4];

		/// Maximum length of pointsLengthSquare[i]
		float mMaxLengthSquare;

		/// Support points of object A in local coordinates
		vec3 mSuppPointsA[4];

		/// Support points of object B in local coordinates
		vec3 mSuppPointsB[4];

		/// diff[i][j] contains points[i] - points[j]
		vec3 mDiffLength[4][4];

		/// Cached determinant values
		float mDet[16][4];

		/// norm[i][j] = (diff[i][j].length())^2
		float mNormSquare[4][4];

		/// 4 bits that identify the current points of the simplex
		/// For instance, 0101 means that points[1] and points[3] are in the simplex
		Bits mBitsCurrentSimplex;

		/// Number between 1 and 4 that identify the last found support point
		Bits mLastFound;

		/// Position of the last found support point (lastFoundBit = 0x1 << lastFound)
		Bits mLastFoundBit;

		/// allBits = bitsCurrentSimplex | lastFoundBit;
		Bits mAllBits;

		// -------------------- Methods -------------------- //

		/// Private copy-constructor
		Simplex(const Simplex& simplex);

		/// Private assignment operator
		Simplex& operator=(const Simplex& simplex);

		/// Return true if some bits of "a" overlap with bits of "b"
		bool overlap(Bits a, Bits b) const;

		/// Return true if the bits of "b" is a subset of the bits of "a"
		bool isSubset(Bits a, Bits b) const;

		/// Return true if the subset is a valid one for the closest point computation.
		bool isValidSubset(Bits subset) const;

		/// Return true if the subset is a proper subset.
		bool isProperSubset(Bits subset) const;

		/// Update the cached values used during the GJK algorithm
		void updateCache();

		/// Compute the cached determinant values
		void computeDeterminants();

		/// Return the closest point "v" in the convex hull of a subset of points
		vec3 computeClosestPointForSubset(Bits subset);

	public:

		// -------------------- Methods -------------------- //

		/// Constructor
		Simplex();

		/// Return true if the simplex contains 4 points
		bool isFull() const;

		/// Return true if the simplex is empty
		bool isEmpty() const;

		/// Return the points of the simplex
		uint32_t getSimplex(vec3* mSuppPointsA, vec3* mSuppPointsB,
								vec3* mPoints) const;

		/// Return the maximum squared length of a point
		float getMaxLengthSquareOfAPoint() const;

		/// Add a new support point of (A-B) int32_to the simplex.
		void addPoint(const vec3& point, const vec3& suppPointA, const vec3& suppPointB);

		/// Return true if the point is in the simplex
		bool isPointInSimplex(const vec3& point) const;

		/// Return true if the set is affinely dependent
		bool isAffinelyDependent() const;

		/// Backup the closest point
		void backupClosestPointInSimplex(vec3& point);

		/// Compute the closest points "pA" and "pB" of object A and B.
		void computeClosestPointsOfAandB(vec3& pA, vec3& pB) const;

		/// Compute the closest point to the origin of the current simplex.
		bool computeClosestPoint(vec3& v);
};

// Return true if some bits of "a" overlap with bits of "b"
inline bool Simplex::overlap(Bits a, Bits b) const {
	return ((a & b) != 0x0);
}

// Return true if the bits of "b" is a subset of the bits of "a"
inline bool Simplex::isSubset(Bits a, Bits b) const {
	return ((a & b) == a);
}

// Return true if the simplex contains 4 points
inline bool Simplex::isFull() const {
	return (mBitsCurrentSimplex == 0xf);
}

// Return true if the simple is empty
inline bool Simplex::isEmpty() const {
	return (mBitsCurrentSimplex == 0x0);
}

// Return the maximum squared length of a point
inline float Simplex::getMaxLengthSquareOfAPoint() const {
	return mMaxLengthSquare;
}

}

