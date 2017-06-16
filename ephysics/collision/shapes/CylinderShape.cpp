/** @file
 * @author Daniel Chappuis
 * @copyright 2010-2016 Daniel Chappuis
 * @license BSD 3 clauses (see license file)
 */

// Libraries
#include <ephysics/collision/shapes/CylinderShape.hpp>
#include <ephysics/collision/ProxyShape.hpp>
#include <ephysics/configuration.hpp>

using namespace ephysics;

// Constructor
/**
 * @param radius Radius of the cylinder (in meters)
 * @param height Height of the cylinder (in meters)
 * @param margin Collision margin (in meters) around the collision shape
 */
CylinderShape::CylinderShape(float radius, float height, float margin)
			  : ConvexShape(CYLINDER, margin), mRadius(radius),
				m_halfHeight(height/float(2.0)) {
	assert(radius > 0.0f);
	assert(height > 0.0f);
}

// Destructor
CylinderShape::~CylinderShape() {

}

// Return a local support point in a given direction without the object margin
vec3 CylinderShape::getLocalSupportPointWithoutMargin(const vec3& _direction, void** _cachedCollisionData) const {
	vec3 supportPoint(0.0, 0.0, 0.0);
	float uDotv = _direction.y();
	vec3 w(_direction.x(), 0.0, _direction.z());
	float lengthW = sqrt(_direction.x() * _direction.x() + _direction.z() * _direction.z());
	if (lengthW > MACHINE_EPSILON) {
		if (uDotv < 0.0) {
			supportPoint.setY(-m_halfHeight);
		} else {
			supportPoint.setY(m_halfHeight);
		}
		supportPoint += (mRadius / lengthW) * w;
	} else {
		if (uDotv < 0.0) {
			supportPoint.setY(-m_halfHeight);
		} else {
			supportPoint.setY(m_halfHeight);
		}
	}
	return supportPoint;
}

// Raycast method with feedback information
/// Algorithm based on the one described at page 194 in Real-ime Collision Detection by
/// Morgan Kaufmann.
bool CylinderShape::raycast(const Ray& ray, RaycastInfo& raycastInfo, ProxyShape* proxyShape) const {

	const vec3 n = ray.point2 - ray.point1;

	const float epsilon = float(0.01);
	vec3 p(float(0), -m_halfHeight, float(0));
	vec3 q(float(0), m_halfHeight, float(0));
	vec3 d = q - p;
	vec3 m = ray.point1 - p;
	float t;

	float mDotD = m.dot(d);
	float nDotD = n.dot(d);
	float dDotD = d.dot(d);

	// Test if the segment is outside the cylinder
	if (mDotD < 0.0f && mDotD + nDotD < float(0.0)) return false;
	if (mDotD > dDotD && mDotD + nDotD > dDotD) return false;

	float nDotN = n.dot(n);
	float mDotN = m.dot(n);

	float a = dDotD * nDotN - nDotD * nDotD;
	float k = m.dot(m) - mRadius * mRadius;
	float c = dDotD * k - mDotD * mDotD;

	// If the ray is parallel to the cylinder axis
	if (std::abs(a) < epsilon) {

		// If the origin is outside the surface of the cylinder, we return no hit
		if (c > 0.0f) return false;

		// Here we know that the segment int32_tersect an endcap of the cylinder

		// If the ray int32_tersects with the "p" endcap of the cylinder
		if (mDotD < 0.0f) {

			t = -mDotN / nDotN;

			// If the int32_tersection is behind the origin of the ray or beyond the maximum
			// raycasting distance, we return no hit
			if (t < 0.0f || t > ray.maxFraction) return false;

			// Compute the hit information
			vec3 localHitPoint = ray.point1 + t * n;
			raycastInfo.body = proxyShape->getBody();
			raycastInfo.proxyShape = proxyShape;
			raycastInfo.hitFraction = t;
			raycastInfo.worldPoint = localHitPoint;
			vec3 normalDirection(0, float(-1), 0);
			raycastInfo.worldNormal = normalDirection;

			return true;
		}
		else if (mDotD > dDotD) {   // If the ray int32_tersects with the "q" endcap of the cylinder

			t = (nDotD - mDotN) / nDotN;

			// If the int32_tersection is behind the origin of the ray or beyond the maximum
			// raycasting distance, we return no hit
			if (t < 0.0f || t > ray.maxFraction) return false;

			// Compute the hit information
			vec3 localHitPoint = ray.point1 + t * n;
			raycastInfo.body = proxyShape->getBody();
			raycastInfo.proxyShape = proxyShape;
			raycastInfo.hitFraction = t;
			raycastInfo.worldPoint = localHitPoint;
			vec3 normalDirection(0, 1.0f, 0);
			raycastInfo.worldNormal = normalDirection;

			return true;
		}
		else {  // If the origin is inside the cylinder, we return no hit
			return false;
		}
	}
	float b = dDotD * mDotN - nDotD * mDotD;
	float discriminant = b * b - a * c;

	// If the discriminant is negative, no real roots and therfore, no hit
	if (discriminant < 0.0f) return false;

	// Compute the smallest root (first int32_tersection along the ray)
	float t0 = t = (-b - std::sqrt(discriminant)) / a;

	// If the int32_tersection is outside the cylinder on "p" endcap side
	float value = mDotD + t * nDotD;
	if (value < 0.0f) {

		// If the ray is pointing away from the "p" endcap, we return no hit
		if (nDotD <= 0.0f) return false;

		// Compute the int32_tersection against the "p" endcap (int32_tersection agains whole plane)
		t = -mDotD / nDotD;

		// Keep the int32_tersection if the it is inside the cylinder radius
		if (k + t * (float(2.0) * mDotN + t) > 0.0f) return false;

		// If the int32_tersection is behind the origin of the ray or beyond the maximum
		// raycasting distance, we return no hit
		if (t < 0.0f || t > ray.maxFraction) return false;

		// Compute the hit information
		vec3 localHitPoint = ray.point1 + t * n;
		raycastInfo.body = proxyShape->getBody();
		raycastInfo.proxyShape = proxyShape;
		raycastInfo.hitFraction = t;
		raycastInfo.worldPoint = localHitPoint;
		vec3 normalDirection(0, float(-1.0), 0);
		raycastInfo.worldNormal = normalDirection;

		return true;
	}
	else if (value > dDotD) {   // If the int32_tersection is outside the cylinder on the "q" side

		// If the ray is pointing away from the "q" endcap, we return no hit
		if (nDotD >= 0.0f) return false;

		// Compute the int32_tersection against the "q" endcap (int32_tersection against whole plane)
		t = (dDotD - mDotD) / nDotD;

		// Keep the int32_tersection if it is inside the cylinder radius
		if (k + dDotD - float(2.0) * mDotD + t * (float(2.0) * (mDotN - nDotD) + t) >
			0.0f) return false;

		// If the int32_tersection is behind the origin of the ray or beyond the maximum
		// raycasting distance, we return no hit
		if (t < 0.0f || t > ray.maxFraction) return false;

		// Compute the hit information
		vec3 localHitPoint = ray.point1 + t * n;
		raycastInfo.body = proxyShape->getBody();
		raycastInfo.proxyShape = proxyShape;
		raycastInfo.hitFraction = t;
		raycastInfo.worldPoint = localHitPoint;
		vec3 normalDirection(0, 1.0f, 0);
		raycastInfo.worldNormal = normalDirection;

		return true;
	}

	t = t0;

	// If the int32_tersection is behind the origin of the ray or beyond the maximum
	// raycasting distance, we return no hit
	if (t < 0.0f || t > ray.maxFraction) return false;

	// Compute the hit information
	vec3 localHitPoint = ray.point1 + t * n;
	raycastInfo.body = proxyShape->getBody();
	raycastInfo.proxyShape = proxyShape;
	raycastInfo.hitFraction = t;
	raycastInfo.worldPoint = localHitPoint;
	vec3 v = localHitPoint - p;
	vec3 w = (v.dot(d) / d.length2()) * d;
	vec3 normalDirection = (localHitPoint - (p + w));
	raycastInfo.worldNormal = normalDirection;

	return true;
}
