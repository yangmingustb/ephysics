/** @file
 * @author Daniel Chappuis
 * @copyright 2010-2016 Daniel Chappuis
 * @license BSD 3 clauses (see license file)
 */
#pragma once

// Libraries
#include <ephysics/collision/shapes/CollisionShape.hpp>

/// ReactPhysics3D namespace
namespace ephysics {

// Class ConvexShape
/**
 * This abstract class represents a convex collision shape associated with a
 * body that is used during the narrow-phase collision detection.
 */
class ConvexShape : public CollisionShape {

	protected :

		// -------------------- Attributes -------------------- //

		/// Margin used for the GJK collision detection algorithm
		float m_margin;

		// -------------------- Methods -------------------- //

		/// Private copy-constructor
		ConvexShape(const ConvexShape& shape);

		/// Private assignment operator
		ConvexShape& operator=(const ConvexShape& shape);

		// Return a local support point in a given direction with the object margin
		vec3 getLocalSupportPointWithMargin(const vec3& direction,
											   void** cachedCollisionData) const;

		/// Return a local support point in a given direction without the object margin
		virtual vec3 getLocalSupportPointWithoutMargin(const vec3& direction,
														  void** cachedCollisionData) const=0;

		/// Return true if a point is inside the collision shape
		virtual bool testPointInside(const vec3& worldPoint, ProxyShape* proxyShape) const=0;

	public :

		// -------------------- Methods -------------------- //

		/// Constructor
		ConvexShape(CollisionShapeType type, float margin);

		/// Destructor
		virtual ~ConvexShape();

		/// Return the current object margin
		float getMargin() const;

		/// Return true if the collision shape is convex, false if it is concave
		virtual bool isConvex() const;

		// -------------------- Friendship -------------------- //

		friend class GJKAlgorithm;
		friend class EPAAlgorithm;
};

/// Return true if the collision shape is convex, false if it is concave
bool ConvexShape::isConvex() const {
	return true;
}

// Return the current collision shape margin
/**
 * @return The margin (in meters) around the collision shape
 */
float ConvexShape::getMargin() const {
	return m_margin;
}

}


