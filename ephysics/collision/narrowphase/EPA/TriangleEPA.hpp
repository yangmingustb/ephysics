/** @file
 * @author Daniel Chappuis
 * @copyright 2010-2016 Daniel Chappuis
 * @license BSD 3 clauses (see license file)
 */
#pragma once

// Libraries
#include <ephysics/mathematics/mathematics.hpp>
#include <ephysics/configuration.hpp>
#include <ephysics/collision/narrowphase/EPA/EdgeEPA.hpp>
#include <cassert>

/// ReactPhysics3D namespace
namespace ephysics {

// Prototypes
bool link(const EdgeEPA& edge0, const EdgeEPA& edge1);
void halfLink(const EdgeEPA& edge0, const EdgeEPA& edge1);


// Class TriangleEPA
/**
 * This class represents a triangle face of the current polytope in the EPA algorithm.
 */
class TriangleEPA {

	private:

		// -------------------- Attributes -------------------- //

		/// Indices of the vertices y_i of the triangle
		uint32_t mIndicesVertices[3];

		/// Three adjacent edges of the triangle (edges of other triangles)
		EdgeEPA mAdjacentEdges[3];

		/// True if the triangle face is visible from the new support point
		bool mIsObsolete;

		/// Determinant
		float mDet;

		/// Point v closest to the origin on the affine hull of the triangle
		vec3 mClosestPoint;

		/// Lambda1 value such that v = lambda0 * y_0 + lambda1 * y_1 + lambda2 * y_2
		float mLambda1;

		/// Lambda1 value such that v = lambda0 * y_0 + lambda1 * y_1 + lambda2 * y_2
		float mLambda2;

		/// Square distance of the point closest point v to the origin
		float mDistSquare;

		// -------------------- Methods -------------------- //

		/// Private copy-constructor
		TriangleEPA(const TriangleEPA& triangle);

		/// Private assignment operator
		TriangleEPA& operator=(const TriangleEPA& triangle);

	public:

		// -------------------- Methods -------------------- //

		/// Constructor
		TriangleEPA();

		/// Constructor
		TriangleEPA(uint32_t v1, uint32_t v2, uint32_t v3);

		/// Destructor
		~TriangleEPA();

		/// Return an adjacent edge of the triangle
		EdgeEPA& getAdjacentEdge(int32_t index);

		/// Set an adjacent edge of the triangle
		void setAdjacentEdge(int32_t index, EdgeEPA& edge);

		/// Return the square distance of the closest point to origin
		float getDistSquare() const;

		/// Set the isObsolete value
		void setIsObsolete(bool isObsolete);

		/// Return true if the triangle face is obsolete
		bool getIsObsolete() const;

		/// Return the point closest to the origin
		const vec3& getClosestPoint() const;

		// Return true if the closest point on affine hull is inside the triangle
		bool isClosestPointInternalToTriangle() const;

		/// Return true if the triangle is visible from a given vertex
		bool isVisibleFromVertex(const vec3* vertices, uint32_t index) const;

		/// Compute the point v closest to the origin of this triangle
		bool computeClosestPoint(const vec3* vertices);

		/// Compute the point of an object closest to the origin
		vec3 computeClosestPointOfObject(const vec3* supportPointsOfObject) const;

		/// Execute the recursive silhouette algorithm from this triangle face.
		bool computeSilhouette(const vec3* vertices, uint32_t index, TrianglesStore& triangleStore);

		/// Access operator
		uint32_t operator[](int32_t i) const;

		/// Associate two edges
		friend bool link(const EdgeEPA& edge0, const EdgeEPA& edge1);

		/// Make a half-link between two edges
		friend void halfLink(const EdgeEPA& edge0, const EdgeEPA& edge1);
};

// Return an edge of the triangle
inline EdgeEPA& TriangleEPA::getAdjacentEdge(int32_t index) {
	assert(index >= 0 && index < 3);
	return mAdjacentEdges[index];
}

// Set an adjacent edge of the triangle
inline void TriangleEPA::setAdjacentEdge(int32_t index, EdgeEPA& edge) {
	assert(index >=0 && index < 3);
	mAdjacentEdges[index] = edge;
}

// Return the square distance  of the closest point to origin
inline float TriangleEPA::getDistSquare() const {
	return mDistSquare;
}

// Set the isObsolete value
inline void TriangleEPA::setIsObsolete(bool isObsolete) {
	mIsObsolete = isObsolete;
}

// Return true if the triangle face is obsolete
inline bool TriangleEPA::getIsObsolete() const {
	return mIsObsolete;
}

// Return the point closest to the origin
inline const vec3& TriangleEPA::getClosestPoint() const {
	return mClosestPoint;
}

// Return true if the closest point on affine hull is inside the triangle
inline bool TriangleEPA::isClosestPointInternalToTriangle() const {
	return (mLambda1 >= 0.0 && mLambda2 >= 0.0 && (mLambda1 + mLambda2) <= mDet);
}

// Return true if the triangle is visible from a given vertex
inline bool TriangleEPA::isVisibleFromVertex(const vec3* vertices, uint32_t index) const {
	vec3 closestToVert = vertices[index] - mClosestPoint;
	return (mClosestPoint.dot(closestToVert) > 0.0);
}

// Compute the point of an object closest to the origin
inline vec3 TriangleEPA::computeClosestPointOfObject(const vec3* supportPointsOfObject) const{
	const vec3& p0 = supportPointsOfObject[mIndicesVertices[0]];
	return p0 + 1.0f/mDet * (mLambda1 * (supportPointsOfObject[mIndicesVertices[1]] - p0) +
						   mLambda2 * (supportPointsOfObject[mIndicesVertices[2]] - p0));
}

// Access operator
inline uint32_t TriangleEPA::operator[](int32_t i) const {
	assert(i >= 0 && i <3);
	return mIndicesVertices[i];
}

}