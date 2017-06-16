/** @file
 * @author Daniel Chappuis
 * @copyright 2010-2016 Daniel Chappuis
 * @license BSD 3 clauses (see license file)
 */
#pragma once

// Libraries
#include <ephysics/collision/shapes/ConvexShape.hpp>
#include <ephysics/body/CollisionBody.hpp>
#include <ephysics/mathematics/mathematics.hpp>

/// ReactPhysics3D namespace
namespace ephysics {

// Class ConeShape
/**
 * This class represents a cone collision shape centered at the
 * origin and alligned with the Y axis. The cone is defined
 * by its height and by the radius of its base. The center of the
 * cone is at the half of the height. The "transform" of the
 * corresponding rigid body gives an orientation and a position
 * to the cone. This collision shape uses an extra margin distance around
 * it for collision detection purpose. The default margin is 4cm (if your
 * units are meters, which is recommended). In case, you want to simulate small
 * objects (smaller than the margin distance), you might want to reduce the margin
 * by specifying your own margin distance using the "margin" parameter in the
 * constructor of the cone shape. Otherwise, it is recommended to use the
 * default margin distance by not using the "margin" parameter in the constructor.
 */
class ConeShape : public ConvexShape {
	protected :
		/// Radius of the base
		float m_radius;
		/// Half height of the cone
		float m_halfHeight;
		/// sine of the semi angle at the apex point
		float m_sinTheta;
		/// Private copy-constructor
		ConeShape(const ConeShape& shape);

		/// Private assignment operator
		ConeShape& operator=(const ConeShape& shape);

		/// Return a local support point in a given direction without the object margin
		virtual vec3 getLocalSupportPointWithoutMargin(const vec3& direction,
														  void** cachedCollisionData) const;

		/// Return true if a point is inside the collision shape
		virtual bool testPointInside(const vec3& localPoint, ProxyShape* proxyShape) const;

		/// Raycast method with feedback information
		virtual bool raycast(const Ray& ray, RaycastInfo& raycastInfo, ProxyShape* proxyShape) const;

		/// Return the number of bytes used by the collision shape
		virtual size_t getSizeInBytes() const;
		
	public :

		// -------------------- Methods -------------------- //

		/// Constructor
		ConeShape(float m_radius, float height, float margin = OBJECT_MARGIN);

		/// Return the radius
		float getRadius() const;

		/// Return the height
		float getHeight() const;

		/// Set the scaling vector of the collision shape
		virtual void setLocalScaling(const vec3& scaling);

		/// Return the local bounds of the shape in x, y and z directions
		virtual void getLocalBounds(vec3& min, vec3& max) const;

		/// Return the local inertia tensor of the collision shape
		virtual void computeLocalInertiaTensor(etk::Matrix3x3& tensor, float mass) const;
};

// Return the radius
/**
 * @return Radius of the cone (in meters)
 */
inline float ConeShape::getRadius() const {
	return m_radius;
}

// Return the height
/**
 * @return Height of the cone (in meters)
 */
inline float ConeShape::getHeight() const {
	return float(2.0) * m_halfHeight;
}

// Set the scaling vector of the collision shape
inline void ConeShape::setLocalScaling(const vec3& scaling) {

	m_halfHeight = (m_halfHeight / m_scaling.y()) * scaling.y();
	m_radius = (m_radius / m_scaling.x()) * scaling.x();

	CollisionShape::setLocalScaling(scaling);
}

// Return the number of bytes used by the collision shape
inline size_t ConeShape::getSizeInBytes() const {
	return sizeof(ConeShape);
}

// Return the local bounds of the shape in x, y and z directions
/**
 * @param min The minimum bounds of the shape in local-space coordinates
 * @param max The maximum bounds of the shape in local-space coordinates
 */
inline void ConeShape::getLocalBounds(vec3& min, vec3& max) const {

	// Maximum bounds
	max.setX(m_radius + m_margin);
	max.setY(m_halfHeight + m_margin);
	max.setZ(max.x());

	// Minimum bounds
	min.setX(-max.x());
	min.setY(-max.y());
	min.setZ(min.x());
}

// Return the local inertia tensor of the collision shape
/**
 * @param[out] tensor The 3x3 inertia tensor matrix of the shape in local-space
 *					coordinates
 * @param mass Mass to use to compute the inertia tensor of the collision shape
 */
inline void ConeShape::computeLocalInertiaTensor(etk::Matrix3x3& tensor, float mass) const {
	float rSquare = m_radius * m_radius;
	float diagXZ = float(0.15) * mass * (rSquare + m_halfHeight);
	tensor.setValue(diagXZ, 0.0, 0.0,
						0.0, float(0.3) * mass * rSquare,
						0.0, 0.0, 0.0, diagXZ);
}

// Return true if a point is inside the collision shape
inline bool ConeShape::testPointInside(const vec3& localPoint, ProxyShape* proxyShape) const {
	const float radiusHeight = m_radius * (-localPoint.y() + m_halfHeight) /
										  (m_halfHeight * float(2.0));
	return (localPoint.y() < m_halfHeight && localPoint.y() > -m_halfHeight) &&
		   (localPoint.x() * localPoint.x() + localPoint.z() * localPoint.z() < radiusHeight *radiusHeight);
}

}