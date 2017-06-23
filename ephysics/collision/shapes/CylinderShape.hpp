/** @file
 * @author Daniel Chappuis
 * @copyright 2010-2016 Daniel Chappuis
 * @license BSD 3 clauses (see license file)
 */
#pragma once

#include <ephysics/collision/shapes/ConvexShape.hpp>
#include <ephysics/body/CollisionBody.hpp>
#include <ephysics/mathematics/mathematics.hpp>

namespace ephysics {
/**
 * @brief It represents a cylinder collision shape around the Y axis
 * and centered at the origin. The cylinder is defined by its height
 * and the radius of its base. The "transform" of the corresponding
 * rigid body gives an orientation and a position to the cylinder.
 * This collision shape uses an extra margin distance around it for collision
 * detection purpose. The default margin is 4cm (if your units are meters,
 * which is recommended). In case, you want to simulate small objects
 * (smaller than the margin distance), you might want to reduce the margin by
 * specifying your own margin distance using the "margin" parameter in the
 * constructor of the cylinder shape. Otherwise, it is recommended to use the
 * default margin distance by not using the "margin" parameter in the constructor.
 */
class CylinderShape : public ConvexShape {
	protected :
		float mRadius; //!< Radius of the base
		float m_halfHeight; //!< Half height of the cylinder
		/// Private copy-constructor
		CylinderShape(const CylinderShape&) = delete;
		/// Private assignment operator
		CylinderShape& operator=(const CylinderShape&) = delete;
		vec3 getLocalSupportPointWithoutMargin(const vec3& _direction, void** _cachedCollisionData) const override;
		bool testPointInside(const vec3& _localPoint, ProxyShape* _proxyShape) const override;
		bool raycast(const Ray& _ray, RaycastInfo& _raycastInfo, ProxyShape* _proxyShape) const override;
		size_t getSizeInBytes() const override;
	public :
		/// Constructor
		CylinderShape(float _radius, float _height, float _margin = OBJECT_MARGIN);
		/// Return the radius
		float getRadius() const;
		/// Return the height
		float getHeight() const;
		void setLocalScaling(const vec3& _scaling) override;
		void getLocalBounds(vec3& _min, vec3& _max) const override;
		void computeLocalInertiaTensor(etk::Matrix3x3& _tensor, float _mass) const override;
};

}


