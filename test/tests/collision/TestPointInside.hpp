/********************************************************************************
* ReactPhysics3D physics library, http://www.ephysics.com				 *
* Copyright (c) 2010-2016 Daniel Chappuis									   *
*********************************************************************************
*																			   *
* This software is provided 'as-is', without any express or implied warranty.   *
* In no event will the authors be held liable for any damages arising from the  *
* use of this software.														 *
*																			   *
* Permission is granted to anyone to use this software for any purpose,		 *
* including commercial applications, and to alter it and redistribute it		*
* freely, subject to the following restrictions:								*
*																			   *
* 1. The origin of this software must not be misrepresented; you must not claim *
*	that you wrote the original software. If you use this software in a		*
*	product, an acknowledgment in the product documentation would be		   *
*	appreciated but is not required.										   *
*																			   *
* 2. Altered source versions must be plainly marked as such, and must not be	*
*	misrepresented as being the original software.							 *
*																			   *
* 3. This notice may not be removed or altered from any source distribution.	*
*																			   *
********************************************************************************/

#ifndef TEST_POINT_INSIDE_H
#define TEST_POINT_INSIDE_H

// Libraries
#include <test/Test.hpp>
#include <ephysics/collision/shapes/BoxShape.hpp>
#include <ephysics/collision/shapes/SphereShape.hpp>
#include <ephysics/collision/shapes/CapsuleShape.hpp>
#include <ephysics/collision/shapes/ConeShape.hpp>
#include <ephysics/collision/shapes/ConvexMeshShape.hpp>
#include <ephysics/collision/shapes/CylinderShape.hpp>

/// Reactphysics3D namespace
namespace ephysics {

// Class TestPointInside
/**
 * Unit test for the CollisionBody::testPointInside() method.
 */
class TestPointInside : public Test {

	private :

		// ---------- Atributes ---------- //

		// Physics world
		CollisionWorld* m_world;

		// Bodies
		CollisionBody* mBoxBody;
		CollisionBody* mSphereBody;
		CollisionBody* mCapsuleBody;
		CollisionBody* mConeBody;
		CollisionBody* mConvexMeshBody;
		CollisionBody* mConvexMeshBodyEdgesInfo;
		CollisionBody* mCylinderBody;
		CollisionBody* mCompoundBody;

		// Collision shapes
		BoxShape* mBoxShape;
		SphereShape* mSphereShape;
		CapsuleShape* mCapsuleShape;
		ConeShape* mConeShape;
		ConvexMeshShape* mConvexMeshShape;
		ConvexMeshShape* mConvexMeshShapeBodyEdgesInfo;
		CylinderShape* mCylinderShape;

		// etk::Transform3D
		etk::Transform3D m_bodyTransform;
		etk::Transform3D m_shapeTransform;
		etk::Transform3D mLocalShapeToWorld;
		etk::Transform3D mLocalShape2ToWorld;

		// Proxy Shapes
		ProxyShape* mBoxProxyShape;
		ProxyShape* mSphereProxyShape;
		ProxyShape* mCapsuleProxyShape;
		ProxyShape* mConeProxyShape;
		ProxyShape* mConvexMeshProxyShape;
		ProxyShape* mConvexMeshProxyShapeEdgesInfo;
		ProxyShape* mCylinderProxyShape;

	public :

		// ---------- Methods ---------- //

		/// Constructor
		TestPointInside(const etk::String& name) : Test(name) {

			// Create the world
			m_world = new CollisionWorld();

			// Body transform
			vec3 position(-3, 2, 7);
			etk::Quaternion orientation(PI / 5, PI / 6, PI / 7);
			m_bodyetk::Transform3D = Transform(position, orientation);

			// Create the bodies
			mBoxBody = m_world->createCollisionBody(m_bodyetk::Transform3D);
			mSphereBody = m_world->createCollisionBody(m_bodyetk::Transform3D);
			mCapsuleBody = m_world->createCollisionBody(m_bodyetk::Transform3D);
			mConeBody = m_world->createCollisionBody(m_bodyetk::Transform3D);
			mConvexMeshBody = m_world->createCollisionBody(m_bodyetk::Transform3D);
			mConvexMeshBodyEdgesInfo = m_world->createCollisionBody(m_bodyetk::Transform3D);
			mCylinderBody = m_world->createCollisionBody(m_bodyetk::Transform3D);
			mCompoundBody = m_world->createCollisionBody(m_bodyetk::Transform3D);

			// Collision shape transform
			vec3 shapePosition(1, -4, -3);
			etk::Quaternion shapeOrientation(3 * PI / 6 , -PI / 8, PI / 3);
			m_shapeTransform = Transform(shapePosition, shapeOrientation);

			// Compute the the transform from a local shape point to world-space
			mLocalShapeToWorld = m_bodyetk::Transform3D * m_shapeTransform;

			// Create collision shapes
			mBoxShape = new BoxShape(vec3(2, 3, 4), 0);
			mBoxProxyShape = mBoxBody->addCollisionShape(mBoxShape, m_shapeTransform);

			mSphereShape = new SphereShape(3);
			mSphereProxyShape = mSphereBody->addCollisionShape(mSphereShape, m_shapeTransform);

			mCapsuleShape = new CapsuleShape(2, 10);
			mCapsuleProxyShape = mCapsuleBody->addCollisionShape(mCapsuleShape, m_shapeTransform);

			mConeShape = new ConeShape(2, 6, 0);
			mConeProxyShape = mConeBody->addCollisionShape(mConeShape, m_shapeTransform);

			mConvexMeshShape = new ConvexMeshShape(0.0);			 // Box of dimension (2, 3, 4)
			mConvexMeshShape->addVertex(vec3(-2, -3, -4));
			mConvexMeshShape->addVertex(vec3(2, -3, -4));
			mConvexMeshShape->addVertex(vec3(2, -3, 4));
			mConvexMeshShape->addVertex(vec3(-2, -3, 4));
			mConvexMeshShape->addVertex(vec3(-2, 3, -4));
			mConvexMeshShape->addVertex(vec3(2, 3, -4));
			mConvexMeshShape->addVertex(vec3(2, 3, 4));
			mConvexMeshShape->addVertex(vec3(-2, 3, 4));
			mConvexMeshProxyShape = mConvexMeshBody->addCollisionShape(mConvexMeshShape, m_shapeTransform);

			mConvexMeshShapeBodyEdgesInfo = new ConvexMeshShape(0.0);
			mConvexMeshShapeBodyEdgesInfo->addVertex(vec3(-2, -3, -4));
			mConvexMeshShapeBodyEdgesInfo->addVertex(vec3(2, -3, -4));
			mConvexMeshShapeBodyEdgesInfo->addVertex(vec3(2, -3, 4));
			mConvexMeshShapeBodyEdgesInfo->addVertex(vec3(-2, -3, 4));
			mConvexMeshShapeBodyEdgesInfo->addVertex(vec3(-2, 3, -4));
			mConvexMeshShapeBodyEdgesInfo->addVertex(vec3(2, 3, -4));
			mConvexMeshShapeBodyEdgesInfo->addVertex(vec3(2, 3, 4));
			mConvexMeshShapeBodyEdgesInfo->addVertex(vec3(-2, 3, 4));
			mConvexMeshShapeBodyEdgesInfo->addEdge(0, 1);
			mConvexMeshShapeBodyEdgesInfo->addEdge(1, 2);
			mConvexMeshShapeBodyEdgesInfo->addEdge(2, 3);
			mConvexMeshShapeBodyEdgesInfo->addEdge(0, 3);
			mConvexMeshShapeBodyEdgesInfo->addEdge(4, 5);
			mConvexMeshShapeBodyEdgesInfo->addEdge(5, 6);
			mConvexMeshShapeBodyEdgesInfo->addEdge(6, 7);
			mConvexMeshShapeBodyEdgesInfo->addEdge(4, 7);
			mConvexMeshShapeBodyEdgesInfo->addEdge(0, 4);
			mConvexMeshShapeBodyEdgesInfo->addEdge(1, 5);
			mConvexMeshShapeBodyEdgesInfo->addEdge(2, 6);
			mConvexMeshShapeBodyEdgesInfo->addEdge(3, 7);
			mConvexMeshShapeBodyEdgesInfo->setIsEdgesInformationUsed(true);
			mConvexMeshProxyShapeEdgesInfo = mConvexMeshBodyEdgesInfo->addCollisionShape(
																	 mConvexMeshShapeBodyEdgesInfo,
																	 m_shapeTransform);

			mCylinderShape = new CylinderShape(3, 8, 0);
			mCylinderProxyShape = mCylinderBody->addCollisionShape(mCylinderShape, m_shapeTransform);

			// Compound shape is a cylinder and a sphere
			vec3 positionShape2(vec3(4, 2, -3));
			etk::Quaternion orientationShape2(-3 *PI / 8, 1.5 * PI/ 3, PI / 13);
			etk::Transform3D shapeTransform2(positionShape2, orientationShape2);
			mLocalShape2ToWorld = m_bodyetk::Transform3D * shapeTransform2;
			mCompoundBody->addCollisionShape(mCylinderShape, m_shapeTransform);
			mCompoundBody->addCollisionShape(mSphereShape, shapeTransform2);
		}

		/// Destructor
		~TestPointInside() {
			delete mBoxShape;
			delete mSphereShape;
			delete mCapsuleShape;
			delete mConeShape;
			delete mConvexMeshShape;
			delete mConvexMeshShapeBodyEdgesInfo;
			delete mCylinderShape;
		}

		/// Run the tests
		void run() {
			testBox();
			testSphere();
			testCapsule();
			testCone();
			testConvexMesh();
			testCylinder();
			testCompound();
		}

		/// Test the ProxyBoxShape::testPointInside() and
		/// CollisionBody::testPointInside() methods
		void testBox() {

			// Tests with CollisionBody
			test(mBoxBody->testPointInside(mLocalShapeToWorld * vec3(0, 0, 0)));
			test(mBoxBody->testPointInside(mLocalShapeToWorld * vec3(-1.9, 0, 0)));
			test(mBoxBody->testPointInside(mLocalShapeToWorld * vec3(1.9, 0, 0)));
			test(mBoxBody->testPointInside(mLocalShapeToWorld * vec3(0, -2.9, 0)));
			test(mBoxBody->testPointInside(mLocalShapeToWorld * vec3(0, 2.9, 0)));
			test(mBoxBody->testPointInside(mLocalShapeToWorld * vec3(0, 0, -3.9)));
			test(mBoxBody->testPointInside(mLocalShapeToWorld * vec3(0, 0, 3.9)));
			test(mBoxBody->testPointInside(mLocalShapeToWorld * vec3(-1.9, -2.9, -3.9)));
			test(mBoxBody->testPointInside(mLocalShapeToWorld * vec3(1.9, 2.9, 3.9)));
			test(mBoxBody->testPointInside(mLocalShapeToWorld * vec3(-1, -2, -1.5)));
			test(mBoxBody->testPointInside(mLocalShapeToWorld * vec3(-1, 2, -2.5)));
			test(mBoxBody->testPointInside(mLocalShapeToWorld * vec3(1, -2, 3.5)));

			test(!mBoxBody->testPointInside(mLocalShapeToWorld * vec3(-2.1, 0, 0)));
			test(!mBoxBody->testPointInside(mLocalShapeToWorld * vec3(2.1, 0, 0)));
			test(!mBoxBody->testPointInside(mLocalShapeToWorld * vec3(0, -3.1, 0)));
			test(!mBoxBody->testPointInside(mLocalShapeToWorld * vec3(0, 3.1, 0)));
			test(!mBoxBody->testPointInside(mLocalShapeToWorld * vec3(0, 0, -4.1)));
			test(!mBoxBody->testPointInside(mLocalShapeToWorld * vec3(0, 0, 4.1)));
			test(!mBoxBody->testPointInside(mLocalShapeToWorld * vec3(-2.1, -3.1, -4.1)));
			test(!mBoxBody->testPointInside(mLocalShapeToWorld * vec3(2.1, 3.1, 4.1)));
			test(!mBoxBody->testPointInside(mLocalShapeToWorld * vec3(-10, -2, -1.5)));
			test(!mBoxBody->testPointInside(mLocalShapeToWorld * vec3(-1, 4, -2.5)));
			test(!mBoxBody->testPointInside(mLocalShapeToWorld * vec3(1, -2, 4.5)));

			// Tests with ProxyBoxShape
			test(mBoxProxyShape->testPointInside(mLocalShapeToWorld * vec3(0, 0, 0)));
			test(mBoxProxyShape->testPointInside(mLocalShapeToWorld * vec3(-1.9, 0, 0)));
			test(mBoxProxyShape->testPointInside(mLocalShapeToWorld * vec3(1.9, 0, 0)));
			test(mBoxProxyShape->testPointInside(mLocalShapeToWorld * vec3(0, -2.9, 0)));
			test(mBoxProxyShape->testPointInside(mLocalShapeToWorld * vec3(0, 2.9, 0)));
			test(mBoxProxyShape->testPointInside(mLocalShapeToWorld * vec3(0, 0, -3.9)));
			test(mBoxProxyShape->testPointInside(mLocalShapeToWorld * vec3(0, 0, 3.9)));
			test(mBoxProxyShape->testPointInside(mLocalShapeToWorld * vec3(-1.9, -2.9, -3.9)));
			test(mBoxProxyShape->testPointInside(mLocalShapeToWorld * vec3(1.9, 2.9, 3.9)));
			test(mBoxProxyShape->testPointInside(mLocalShapeToWorld * vec3(-1, -2, -1.5)));
			test(mBoxProxyShape->testPointInside(mLocalShapeToWorld * vec3(-1, 2, -2.5)));
			test(mBoxProxyShape->testPointInside(mLocalShapeToWorld * vec3(1, -2, 3.5)));

			test(!mBoxProxyShape->testPointInside(mLocalShapeToWorld * vec3(-2.1, 0, 0)));
			test(!mBoxProxyShape->testPointInside(mLocalShapeToWorld * vec3(2.1, 0, 0)));
			test(!mBoxProxyShape->testPointInside(mLocalShapeToWorld * vec3(0, -3.1, 0)));
			test(!mBoxProxyShape->testPointInside(mLocalShapeToWorld * vec3(0, 3.1, 0)));
			test(!mBoxProxyShape->testPointInside(mLocalShapeToWorld * vec3(0, 0, -4.1)));
			test(!mBoxProxyShape->testPointInside(mLocalShapeToWorld * vec3(0, 0, 4.1)));
			test(!mBoxProxyShape->testPointInside(mLocalShapeToWorld * vec3(-2.1, -3.1, -4.1)));
			test(!mBoxProxyShape->testPointInside(mLocalShapeToWorld * vec3(2.1, 3.1, 4.1)));
			test(!mBoxProxyShape->testPointInside(mLocalShapeToWorld * vec3(-10, -2, -1.5)));
			test(!mBoxProxyShape->testPointInside(mLocalShapeToWorld * vec3(-1, 4, -2.5)));
			test(!mBoxProxyShape->testPointInside(mLocalShapeToWorld * vec3(1, -2, 4.5)));
		}

		/// Test the ProxySphereShape::testPointInside() and
		/// CollisionBody::testPointInside() methods
		void testSphere() {

			// Tests with CollisionBody
			test(mSphereBody->testPointInside(mLocalShapeToWorld * vec3(0, 0, 0)));
			test(mSphereBody->testPointInside(mLocalShapeToWorld * vec3(2.9, 0, 0)));
			test(mSphereBody->testPointInside(mLocalShapeToWorld * vec3(-2.9, 0, 0)));
			test(mSphereBody->testPointInside(mLocalShapeToWorld * vec3(0, 2.9, 0)));
			test(mSphereBody->testPointInside(mLocalShapeToWorld * vec3(0, -2.9, 0)));
			test(mSphereBody->testPointInside(mLocalShapeToWorld * vec3(0, 0, 2.9)));
			test(mSphereBody->testPointInside(mLocalShapeToWorld * vec3(0, 0, 2.9)));
			test(mSphereBody->testPointInside(mLocalShapeToWorld * vec3(-1, -2, -1.5)));
			test(mSphereBody->testPointInside(mLocalShapeToWorld * vec3(-1, 2, -1.5)));
			test(mSphereBody->testPointInside(mLocalShapeToWorld * vec3(1, -2, 1.5)));

			test(!mSphereBody->testPointInside(mLocalShapeToWorld * vec3(3.1, 0, 0)));
			test(!mSphereBody->testPointInside(mLocalShapeToWorld * vec3(-3.1, 0, 0)));
			test(!mSphereBody->testPointInside(mLocalShapeToWorld * vec3(0, 3.1, 0)));
			test(!mSphereBody->testPointInside(mLocalShapeToWorld * vec3(0, -3.1, 0)));
			test(!mSphereBody->testPointInside(mLocalShapeToWorld * vec3(0, 0, 3.1)));
			test(!mSphereBody->testPointInside(mLocalShapeToWorld * vec3(0, 0, -3.1)));
			test(!mSphereBody->testPointInside(mLocalShapeToWorld * vec3(-2, -2, -2)));
			test(!mSphereBody->testPointInside(mLocalShapeToWorld * vec3(-2, 2, -1.5)));
			test(!mSphereBody->testPointInside(mLocalShapeToWorld * vec3(1.5, -2, 2.5)));

			// Tests with ProxySphereShape
			test(mSphereProxyShape->testPointInside(mLocalShapeToWorld * vec3(0, 0, 0)));
			test(mSphereProxyShape->testPointInside(mLocalShapeToWorld * vec3(2.9, 0, 0)));
			test(mSphereProxyShape->testPointInside(mLocalShapeToWorld * vec3(-2.9, 0, 0)));
			test(mSphereProxyShape->testPointInside(mLocalShapeToWorld * vec3(0, 2.9, 0)));
			test(mSphereProxyShape->testPointInside(mLocalShapeToWorld * vec3(0, -2.9, 0)));
			test(mSphereProxyShape->testPointInside(mLocalShapeToWorld * vec3(0, 0, 2.9)));
			test(mSphereProxyShape->testPointInside(mLocalShapeToWorld * vec3(0, 0, 2.9)));
			test(mSphereProxyShape->testPointInside(mLocalShapeToWorld * vec3(-1, -2, -1.5)));
			test(mSphereProxyShape->testPointInside(mLocalShapeToWorld * vec3(-1, 2, -1.5)));
			test(mSphereProxyShape->testPointInside(mLocalShapeToWorld * vec3(1, -2, 1.5)));

			test(!mSphereProxyShape->testPointInside(mLocalShapeToWorld * vec3(3.1, 0, 0)));
			test(!mSphereProxyShape->testPointInside(mLocalShapeToWorld * vec3(-3.1, 0, 0)));
			test(!mSphereProxyShape->testPointInside(mLocalShapeToWorld * vec3(0, 3.1, 0)));
			test(!mSphereProxyShape->testPointInside(mLocalShapeToWorld * vec3(0, -3.1, 0)));
			test(!mSphereProxyShape->testPointInside(mLocalShapeToWorld * vec3(0, 0, 3.1)));
			test(!mSphereProxyShape->testPointInside(mLocalShapeToWorld * vec3(0, 0, -3.1)));
			test(!mSphereProxyShape->testPointInside(mLocalShapeToWorld * vec3(-2, -2, -2)));
			test(!mSphereProxyShape->testPointInside(mLocalShapeToWorld * vec3(-2, 2, -1.5)));
			test(!mSphereProxyShape->testPointInside(mLocalShapeToWorld * vec3(1.5, -2, 2.5)));
		}

		/// Test the ProxyCapsuleShape::testPointInside() and
		/// CollisionBody::testPointInside() methods
		void testCapsule() {

			// Tests with CollisionBody
			test(mCapsuleBody->testPointInside(mLocalShapeToWorld * vec3(0, 0, 0)));
			test(mCapsuleBody->testPointInside(mLocalShapeToWorld * vec3(0, 5, 0)));
			test(mCapsuleBody->testPointInside(mLocalShapeToWorld * vec3(0, -5, 0)));
			test(mCapsuleBody->testPointInside(mLocalShapeToWorld * vec3(0, -6.9, 0)));
			test(mCapsuleBody->testPointInside(mLocalShapeToWorld * vec3(0, 6.9, 0)));
			test(mCapsuleBody->testPointInside(mLocalShapeToWorld * vec3(0, 0, 1.9)));
			test(mCapsuleBody->testPointInside(mLocalShapeToWorld * vec3(0, 0, -1.9)));
			test(mCapsuleBody->testPointInside(mLocalShapeToWorld * vec3(1.9, 0, 0)));
			test(mCapsuleBody->testPointInside(mLocalShapeToWorld * vec3(-1.9, 0, 0)));
			test(mCapsuleBody->testPointInside(mLocalShapeToWorld * vec3(0.9, 0, 0.9)));
			test(mCapsuleBody->testPointInside(mLocalShapeToWorld * vec3(0.9, 0, -0.9)));
			test(mCapsuleBody->testPointInside(mLocalShapeToWorld * vec3(0, 5, 1.9)));
			test(mCapsuleBody->testPointInside(mLocalShapeToWorld * vec3(0, 5, -1.9)));
			test(mCapsuleBody->testPointInside(mLocalShapeToWorld * vec3(1.9, 5, 0)));
			test(mCapsuleBody->testPointInside(mLocalShapeToWorld * vec3(-1.9, 5, 0)));
			test(mCapsuleBody->testPointInside(mLocalShapeToWorld * vec3(0.9, 5, 0.9)));
			test(mCapsuleBody->testPointInside(mLocalShapeToWorld * vec3(0.9, 5, -0.9)));
			test(mCapsuleBody->testPointInside(mLocalShapeToWorld * vec3(0, -5, 1.9)));
			test(mCapsuleBody->testPointInside(mLocalShapeToWorld * vec3(0, -5, -1.9)));
			test(mCapsuleBody->testPointInside(mLocalShapeToWorld * vec3(1.9, -5, 0)));
			test(mCapsuleBody->testPointInside(mLocalShapeToWorld * vec3(-1.9, -5, 0)));
			test(mCapsuleBody->testPointInside(mLocalShapeToWorld * vec3(0.9, -5, 0.9)));
			test(mCapsuleBody->testPointInside(mLocalShapeToWorld * vec3(0.9, -5, -0.9)));
			test(mCapsuleBody->testPointInside(mLocalShapeToWorld * vec3(-1.7, -4, -0.9)));
			test(mCapsuleBody->testPointInside(mLocalShapeToWorld * vec3(-1, 2, 0.4)));
			test(mCapsuleBody->testPointInside(mLocalShapeToWorld * vec3(1.3, 1, 1.5)));

			test(!mCapsuleBody->testPointInside(mLocalShapeToWorld * vec3(0, -7.1, 0)));
			test(!mCapsuleBody->testPointInside(mLocalShapeToWorld * vec3(0, 7.1, 0)));
			test(!mCapsuleBody->testPointInside(mLocalShapeToWorld * vec3(0, 0, 2.1)));
			test(!mCapsuleBody->testPointInside(mLocalShapeToWorld * vec3(0, 0, -2.1)));
			test(!mCapsuleBody->testPointInside(mLocalShapeToWorld * vec3(2.1, 0, 0)));
			test(!mCapsuleBody->testPointInside(mLocalShapeToWorld * vec3(-2.1, 0, 0)));
			test(!mCapsuleBody->testPointInside(mLocalShapeToWorld * vec3(0, 5, 2.1)));
			test(!mCapsuleBody->testPointInside(mLocalShapeToWorld * vec3(0, 5, -2.1)));
			test(!mCapsuleBody->testPointInside(mLocalShapeToWorld * vec3(2.1, 5, 0)));
			test(!mCapsuleBody->testPointInside(mLocalShapeToWorld * vec3(-2.1, 5, 0)));
			test(!mCapsuleBody->testPointInside(mLocalShapeToWorld * vec3(1.5, 5, 1.6)));
			test(!mCapsuleBody->testPointInside(mLocalShapeToWorld * vec3(1.5, 5, -1.7)));
			test(!mCapsuleBody->testPointInside(mLocalShapeToWorld * vec3(0, -5, 2.1)));
			test(!mCapsuleBody->testPointInside(mLocalShapeToWorld * vec3(0, -5, -2.1)));
			test(!mCapsuleBody->testPointInside(mLocalShapeToWorld * vec3(2.1, -5, 0)));
			test(!mCapsuleBody->testPointInside(mLocalShapeToWorld * vec3(-2.1, -5, 0)));
			test(!mCapsuleBody->testPointInside(mLocalShapeToWorld * vec3(1.5, -5, 1.6)));
			test(!mCapsuleBody->testPointInside(mLocalShapeToWorld * vec3(1.5, -5, -1.7)));

			// Tests with ProxyCapsuleShape
			test(mCapsuleProxyShape->testPointInside(mLocalShapeToWorld * vec3(0, 0, 0)));
			test(mCapsuleProxyShape->testPointInside(mLocalShapeToWorld * vec3(0, 5, 0)));
			test(mCapsuleProxyShape->testPointInside(mLocalShapeToWorld * vec3(0, -5, 0)));
			test(mCapsuleProxyShape->testPointInside(mLocalShapeToWorld * vec3(0, -6.9, 0)));
			test(mCapsuleProxyShape->testPointInside(mLocalShapeToWorld * vec3(0, 6.9, 0)));
			test(mCapsuleProxyShape->testPointInside(mLocalShapeToWorld * vec3(0, 0, 1.9)));
			test(mCapsuleProxyShape->testPointInside(mLocalShapeToWorld * vec3(0, 0, -1.9)));
			test(mCapsuleProxyShape->testPointInside(mLocalShapeToWorld * vec3(1.9, 0, 0)));
			test(mCapsuleProxyShape->testPointInside(mLocalShapeToWorld * vec3(-1.9, 0, 0)));
			test(mCapsuleProxyShape->testPointInside(mLocalShapeToWorld * vec3(0.9, 0, 0.9)));
			test(mCapsuleProxyShape->testPointInside(mLocalShapeToWorld * vec3(0.9, 0, -0.9)));
			test(mCapsuleProxyShape->testPointInside(mLocalShapeToWorld * vec3(0, 5, 1.9)));
			test(mCapsuleProxyShape->testPointInside(mLocalShapeToWorld * vec3(0, 5, -1.9)));
			test(mCapsuleProxyShape->testPointInside(mLocalShapeToWorld * vec3(1.9, 5, 0)));
			test(mCapsuleProxyShape->testPointInside(mLocalShapeToWorld * vec3(-1.9, 5, 0)));
			test(mCapsuleProxyShape->testPointInside(mLocalShapeToWorld * vec3(0.9, 5, 0.9)));
			test(mCapsuleProxyShape->testPointInside(mLocalShapeToWorld * vec3(0.9, 5, -0.9)));
			test(mCapsuleProxyShape->testPointInside(mLocalShapeToWorld * vec3(0, -5, 1.9)));
			test(mCapsuleProxyShape->testPointInside(mLocalShapeToWorld * vec3(0, -5, -1.9)));
			test(mCapsuleProxyShape->testPointInside(mLocalShapeToWorld * vec3(1.9, -5, 0)));
			test(mCapsuleProxyShape->testPointInside(mLocalShapeToWorld * vec3(-1.9, -5, 0)));
			test(mCapsuleProxyShape->testPointInside(mLocalShapeToWorld * vec3(0.9, -5, 0.9)));
			test(mCapsuleProxyShape->testPointInside(mLocalShapeToWorld * vec3(0.9, -5, -0.9)));
			test(mCapsuleProxyShape->testPointInside(mLocalShapeToWorld * vec3(-1.7, -4, -0.9)));
			test(mCapsuleProxyShape->testPointInside(mLocalShapeToWorld * vec3(-1, 2, 0.4)));
			test(mCapsuleProxyShape->testPointInside(mLocalShapeToWorld * vec3(1.3, 1, 1.5)));

			test(!mCapsuleProxyShape->testPointInside(mLocalShapeToWorld * vec3(0, -7.1, 0)));
			test(!mCapsuleProxyShape->testPointInside(mLocalShapeToWorld * vec3(0, 7.1, 0)));
			test(!mCapsuleProxyShape->testPointInside(mLocalShapeToWorld * vec3(0, 0, 2.1)));
			test(!mCapsuleProxyShape->testPointInside(mLocalShapeToWorld * vec3(0, 0, -2.1)));
			test(!mCapsuleProxyShape->testPointInside(mLocalShapeToWorld * vec3(2.1, 0, 0)));
			test(!mCapsuleProxyShape->testPointInside(mLocalShapeToWorld * vec3(-2.1, 0, 0)));
			test(!mCapsuleProxyShape->testPointInside(mLocalShapeToWorld * vec3(0, 5, 2.1)));
			test(!mCapsuleProxyShape->testPointInside(mLocalShapeToWorld * vec3(0, 5, -2.1)));
			test(!mCapsuleProxyShape->testPointInside(mLocalShapeToWorld * vec3(2.1, 5, 0)));
			test(!mCapsuleProxyShape->testPointInside(mLocalShapeToWorld * vec3(-2.1, 5, 0)));
			test(!mCapsuleProxyShape->testPointInside(mLocalShapeToWorld * vec3(1.5, 5, 1.6)));
			test(!mCapsuleProxyShape->testPointInside(mLocalShapeToWorld * vec3(1.5, 5, -1.7)));
			test(!mCapsuleProxyShape->testPointInside(mLocalShapeToWorld * vec3(0, -5, 2.1)));
			test(!mCapsuleProxyShape->testPointInside(mLocalShapeToWorld * vec3(0, -5, -2.1)));
			test(!mCapsuleProxyShape->testPointInside(mLocalShapeToWorld * vec3(2.1, -5, 0)));
			test(!mCapsuleProxyShape->testPointInside(mLocalShapeToWorld * vec3(-2.1, -5, 0)));
			test(!mCapsuleProxyShape->testPointInside(mLocalShapeToWorld * vec3(1.5, -5, 1.6)));
			test(!mCapsuleProxyShape->testPointInside(mLocalShapeToWorld * vec3(1.5, -5, -1.7)));
		}

		/// Test the ProxyConeShape::testPointInside() and
		/// CollisionBody::testPointInside() methods
		void testCone() {

			// Tests with CollisionBody
			test(mConeBody->testPointInside(mLocalShapeToWorld * vec3(0, 0, 0)));
			test(mConeBody->testPointInside(mLocalShapeToWorld * vec3(0.9, 0, 0)));
			test(mConeBody->testPointInside(mLocalShapeToWorld * vec3(-0.9, 0, 0)));
			test(mConeBody->testPointInside(mLocalShapeToWorld * vec3(0, 0, 0.9)));
			test(mConeBody->testPointInside(mLocalShapeToWorld * vec3(0, 0, -0.9)));
			test(mConeBody->testPointInside(mLocalShapeToWorld * vec3(0.6, 0, -0.7)));
			test(mConeBody->testPointInside(mLocalShapeToWorld * vec3(0.6, 0, 0.7)));
			test(mConeBody->testPointInside(mLocalShapeToWorld * vec3(-0.6, 0, -0.7)));
			test(mConeBody->testPointInside(mLocalShapeToWorld * vec3(-0.6, 0, 0.7)));
			test(mConeBody->testPointInside(mLocalShapeToWorld * vec3(0, 2.9, 0)));
			test(mConeBody->testPointInside(mLocalShapeToWorld * vec3(0, -2.9, 0)));
			test(mConeBody->testPointInside(mLocalShapeToWorld * vec3(1.96, -2.9, 0)));
			test(mConeBody->testPointInside(mLocalShapeToWorld * vec3(-1.96, -2.9, 0)));
			test(mConeBody->testPointInside(mLocalShapeToWorld * vec3(0, -2.9, 1.96)));
			test(mConeBody->testPointInside(mLocalShapeToWorld * vec3(0, -2.9, -1.96)));
			test(mConeBody->testPointInside(mLocalShapeToWorld * vec3(1.3, -2.9, -1.4)));
			test(mConeBody->testPointInside(mLocalShapeToWorld * vec3(-1.3, -2.9, 1.4)));

			test(!mConeBody->testPointInside(mLocalShapeToWorld * vec3(1.1, 0, 0)));
			test(!mConeBody->testPointInside(mLocalShapeToWorld * vec3(-1.1, 0, 0)));
			test(!mConeBody->testPointInside(mLocalShapeToWorld * vec3(0, 0, 1.1)));
			test(!mConeBody->testPointInside(mLocalShapeToWorld * vec3(0, 0, -1.1)));
			test(!mConeBody->testPointInside(mLocalShapeToWorld * vec3(0.8, 0, -0.8)));
			test(!mConeBody->testPointInside(mLocalShapeToWorld * vec3(0.8, 0, 0.8)));
			test(!mConeBody->testPointInside(mLocalShapeToWorld * vec3(-0.8, 0, -0.8)));
			test(!mConeBody->testPointInside(mLocalShapeToWorld * vec3(-0.8, 0, 0.8)));
			test(!mConeBody->testPointInside(mLocalShapeToWorld * vec3(0, 3.1, 0)));
			test(!mConeBody->testPointInside(mLocalShapeToWorld * vec3(0, -3.1, 0)));
			test(!mConeBody->testPointInside(mLocalShapeToWorld * vec3(1.97, -2.9, 0)));
			test(!mConeBody->testPointInside(mLocalShapeToWorld * vec3(-1.97, -2.9, 0)));
			test(!mConeBody->testPointInside(mLocalShapeToWorld * vec3(0, -2.9, 1.97)));
			test(!mConeBody->testPointInside(mLocalShapeToWorld * vec3(0, -2.9, -1.97)));
			test(!mConeBody->testPointInside(mLocalShapeToWorld * vec3(1.5, -2.9, -1.5)));
			test(!mConeBody->testPointInside(mLocalShapeToWorld * vec3(-1.5, -2.9, 1.5)));

			// Tests with ProxyConeShape
			test(mConeProxyShape->testPointInside(mLocalShapeToWorld * vec3(0, 0, 0)));
			test(mConeProxyShape->testPointInside(mLocalShapeToWorld * vec3(0.9, 0, 0)));
			test(mConeProxyShape->testPointInside(mLocalShapeToWorld * vec3(-0.9, 0, 0)));
			test(mConeProxyShape->testPointInside(mLocalShapeToWorld * vec3(0, 0, 0.9)));
			test(mConeProxyShape->testPointInside(mLocalShapeToWorld * vec3(0, 0, -0.9)));
			test(mConeProxyShape->testPointInside(mLocalShapeToWorld * vec3(0.6, 0, -0.7)));
			test(mConeProxyShape->testPointInside(mLocalShapeToWorld * vec3(0.6, 0, 0.7)));
			test(mConeProxyShape->testPointInside(mLocalShapeToWorld * vec3(-0.6, 0, -0.7)));
			test(mConeProxyShape->testPointInside(mLocalShapeToWorld * vec3(-0.6, 0, 0.7)));
			test(mConeProxyShape->testPointInside(mLocalShapeToWorld * vec3(0, 2.9, 0)));
			test(mConeProxyShape->testPointInside(mLocalShapeToWorld * vec3(0, -2.9, 0)));
			test(mConeProxyShape->testPointInside(mLocalShapeToWorld * vec3(1.96, -2.9, 0)));
			test(mConeProxyShape->testPointInside(mLocalShapeToWorld * vec3(-1.96, -2.9, 0)));
			test(mConeProxyShape->testPointInside(mLocalShapeToWorld * vec3(0, -2.9, 1.96)));
			test(mConeProxyShape->testPointInside(mLocalShapeToWorld * vec3(0, -2.9, -1.96)));
			test(mConeProxyShape->testPointInside(mLocalShapeToWorld * vec3(1.3, -2.9, -1.4)));
			test(mConeProxyShape->testPointInside(mLocalShapeToWorld * vec3(-1.3, -2.9, 1.4)));

			test(!mConeProxyShape->testPointInside(mLocalShapeToWorld * vec3(1.1, 0, 0)));
			test(!mConeProxyShape->testPointInside(mLocalShapeToWorld * vec3(-1.1, 0, 0)));
			test(!mConeProxyShape->testPointInside(mLocalShapeToWorld * vec3(0, 0, 1.1)));
			test(!mConeProxyShape->testPointInside(mLocalShapeToWorld * vec3(0, 0, -1.1)));
			test(!mConeProxyShape->testPointInside(mLocalShapeToWorld * vec3(0.8, 0, -0.8)));
			test(!mConeProxyShape->testPointInside(mLocalShapeToWorld * vec3(0.8, 0, 0.8)));
			test(!mConeProxyShape->testPointInside(mLocalShapeToWorld * vec3(-0.8, 0, -0.8)));
			test(!mConeProxyShape->testPointInside(mLocalShapeToWorld * vec3(-0.8, 0, 0.8)));
			test(!mConeProxyShape->testPointInside(mLocalShapeToWorld * vec3(0, 3.1, 0)));
			test(!mConeProxyShape->testPointInside(mLocalShapeToWorld * vec3(0, -3.1, 0)));
			test(!mConeProxyShape->testPointInside(mLocalShapeToWorld * vec3(1.97, -2.9, 0)));
			test(!mConeProxyShape->testPointInside(mLocalShapeToWorld * vec3(-1.97, -2.9, 0)));
			test(!mConeProxyShape->testPointInside(mLocalShapeToWorld * vec3(0, -2.9, 1.97)));
			test(!mConeProxyShape->testPointInside(mLocalShapeToWorld * vec3(0, -2.9, -1.97)));
			test(!mConeProxyShape->testPointInside(mLocalShapeToWorld * vec3(1.5, -2.9, -1.5)));
			test(!mConeProxyShape->testPointInside(mLocalShapeToWorld * vec3(-1.5, -2.9, 1.5)));
		}

		/// Test the ProxyConvexMeshShape::testPointInside() and
		/// CollisionBody::testPointInside() methods
		void testConvexMesh() {

			// ----- Tests without using edges information ----- //

			// Tests with CollisionBody
			test(mConvexMeshBody->testPointInside(mLocalShapeToWorld * vec3(0, 0, 0)));
			test(mConvexMeshBody->testPointInside(mLocalShapeToWorld * vec3(-1.9, 0, 0)));
			test(mConvexMeshBody->testPointInside(mLocalShapeToWorld * vec3(1.9, 0, 0)));
			test(mConvexMeshBody->testPointInside(mLocalShapeToWorld * vec3(0, -2.9, 0)));
			test(mConvexMeshBody->testPointInside(mLocalShapeToWorld * vec3(0, 2.9, 0)));
			test(mConvexMeshBody->testPointInside(mLocalShapeToWorld * vec3(0, 0, -3.9)));
			test(mConvexMeshBody->testPointInside(mLocalShapeToWorld * vec3(0, 0, 3.9)));
			test(mConvexMeshBody->testPointInside(mLocalShapeToWorld * vec3(-1.9, -2.9, -3.9)));
			test(mConvexMeshBody->testPointInside(mLocalShapeToWorld * vec3(1.9, 2.9, 3.9)));
			test(mConvexMeshBody->testPointInside(mLocalShapeToWorld * vec3(-1, -2, -1.5)));
			test(mConvexMeshBody->testPointInside(mLocalShapeToWorld * vec3(-1, 2, -2.5)));
			test(mConvexMeshBody->testPointInside(mLocalShapeToWorld * vec3(1, -2, 3.5)));

			test(!mConvexMeshBody->testPointInside(mLocalShapeToWorld * vec3(-2.1, 0, 0)));
			test(!mConvexMeshBody->testPointInside(mLocalShapeToWorld * vec3(2.1, 0, 0)));
			test(!mConvexMeshBody->testPointInside(mLocalShapeToWorld * vec3(0, -3.1, 0)));
			test(!mConvexMeshBody->testPointInside(mLocalShapeToWorld * vec3(0, 3.1, 0)));
			test(!mConvexMeshBody->testPointInside(mLocalShapeToWorld * vec3(0, 0, -4.1)));
			test(!mConvexMeshBody->testPointInside(mLocalShapeToWorld * vec3(0, 0, 4.1)));
			test(!mConvexMeshBody->testPointInside(mLocalShapeToWorld * vec3(-2.1, -3.1, -4.1)));
			test(!mConvexMeshBody->testPointInside(mLocalShapeToWorld * vec3(2.1, 3.1, 4.1)));
			test(!mConvexMeshBody->testPointInside(mLocalShapeToWorld * vec3(-10, -2, -1.5)));
			test(!mConvexMeshBody->testPointInside(mLocalShapeToWorld * vec3(-1, 4, -2.5)));
			test(!mConvexMeshBody->testPointInside(mLocalShapeToWorld * vec3(1, -2, 4.5)));

			// Tests with ProxyConvexMeshShape
			test(mConvexMeshProxyShape->testPointInside(mLocalShapeToWorld * vec3(0, 0, 0)));
			test(mConvexMeshProxyShape->testPointInside(mLocalShapeToWorld * vec3(-1.9, 0, 0)));
			test(mConvexMeshProxyShape->testPointInside(mLocalShapeToWorld * vec3(1.9, 0, 0)));
			test(mConvexMeshProxyShape->testPointInside(mLocalShapeToWorld * vec3(0, -2.9, 0)));
			test(mConvexMeshProxyShape->testPointInside(mLocalShapeToWorld * vec3(0, 2.9, 0)));
			test(mConvexMeshProxyShape->testPointInside(mLocalShapeToWorld * vec3(0, 0, -3.9)));
			test(mConvexMeshProxyShape->testPointInside(mLocalShapeToWorld * vec3(0, 0, 3.9)));
			test(mConvexMeshProxyShape->testPointInside(mLocalShapeToWorld * vec3(-1.9, -2.9, -3.9)));
			test(mConvexMeshProxyShape->testPointInside(mLocalShapeToWorld * vec3(1.9, 2.9, 3.9)));
			test(mConvexMeshProxyShape->testPointInside(mLocalShapeToWorld * vec3(-1, -2, -1.5)));
			test(mConvexMeshProxyShape->testPointInside(mLocalShapeToWorld * vec3(-1, 2, -2.5)));
			test(mConvexMeshProxyShape->testPointInside(mLocalShapeToWorld * vec3(1, -2, 3.5)));

			test(!mConvexMeshProxyShape->testPointInside(mLocalShapeToWorld * vec3(-2.1, 0, 0)));
			test(!mConvexMeshProxyShape->testPointInside(mLocalShapeToWorld * vec3(2.1, 0, 0)));
			test(!mConvexMeshProxyShape->testPointInside(mLocalShapeToWorld * vec3(0, -3.1, 0)));
			test(!mConvexMeshProxyShape->testPointInside(mLocalShapeToWorld * vec3(0, 3.1, 0)));
			test(!mConvexMeshProxyShape->testPointInside(mLocalShapeToWorld * vec3(0, 0, -4.1)));
			test(!mConvexMeshProxyShape->testPointInside(mLocalShapeToWorld * vec3(0, 0, 4.1)));
			test(!mConvexMeshProxyShape->testPointInside(mLocalShapeToWorld * vec3(-2.1, -3.1, -4.1)));
			test(!mConvexMeshProxyShape->testPointInside(mLocalShapeToWorld * vec3(2.1, 3.1, 4.1)));
			test(!mConvexMeshProxyShape->testPointInside(mLocalShapeToWorld * vec3(-10, -2, -1.5)));
			test(!mConvexMeshProxyShape->testPointInside(mLocalShapeToWorld * vec3(-1, 4, -2.5)));
			test(!mConvexMeshProxyShape->testPointInside(mLocalShapeToWorld * vec3(1, -2, 4.5)));

			// ----- Tests using edges information ----- //

			// Tests with CollisionBody
			test(mConvexMeshBodyEdgesInfo->testPointInside(mLocalShapeToWorld * vec3(0, 0, 0)));
			test(mConvexMeshBodyEdgesInfo->testPointInside(mLocalShapeToWorld * vec3(-1.9, 0, 0)));
			test(mConvexMeshBodyEdgesInfo->testPointInside(mLocalShapeToWorld * vec3(1.9, 0, 0)));
			test(mConvexMeshBodyEdgesInfo->testPointInside(mLocalShapeToWorld * vec3(0, -2.9, 0)));
			test(mConvexMeshBodyEdgesInfo->testPointInside(mLocalShapeToWorld * vec3(0, 2.9, 0)));
			test(mConvexMeshBodyEdgesInfo->testPointInside(mLocalShapeToWorld * vec3(0, 0, -3.9)));
			test(mConvexMeshBodyEdgesInfo->testPointInside(mLocalShapeToWorld * vec3(0, 0, 3.9)));
			test(mConvexMeshBodyEdgesInfo->testPointInside(mLocalShapeToWorld * vec3(-1.9, -2.9, -3.9)));
			test(mConvexMeshBodyEdgesInfo->testPointInside(mLocalShapeToWorld * vec3(1.9, 2.9, 3.9)));
			test(mConvexMeshBodyEdgesInfo->testPointInside(mLocalShapeToWorld * vec3(-1, -2, -1.5)));
			test(mConvexMeshBodyEdgesInfo->testPointInside(mLocalShapeToWorld * vec3(-1, 2, -2.5)));
			test(mConvexMeshBodyEdgesInfo->testPointInside(mLocalShapeToWorld * vec3(1, -2, 3.5)));

			test(!mConvexMeshBodyEdgesInfo->testPointInside(mLocalShapeToWorld * vec3(-2.1, 0, 0)));
			test(!mConvexMeshBodyEdgesInfo->testPointInside(mLocalShapeToWorld * vec3(2.1, 0, 0)));
			test(!mConvexMeshBodyEdgesInfo->testPointInside(mLocalShapeToWorld * vec3(0, -3.1, 0)));
			test(!mConvexMeshBodyEdgesInfo->testPointInside(mLocalShapeToWorld * vec3(0, 3.1, 0)));
			test(!mConvexMeshBodyEdgesInfo->testPointInside(mLocalShapeToWorld * vec3(0, 0, -4.1)));
			test(!mConvexMeshBodyEdgesInfo->testPointInside(mLocalShapeToWorld * vec3(0, 0, 4.1)));
			test(!mConvexMeshBodyEdgesInfo->testPointInside(mLocalShapeToWorld * vec3(-2.1, -3.1, -4.1)));
			test(!mConvexMeshBodyEdgesInfo->testPointInside(mLocalShapeToWorld * vec3(2.1, 3.1, 4.1)));
			test(!mConvexMeshBodyEdgesInfo->testPointInside(mLocalShapeToWorld * vec3(-10, -2, -1.5)));
			test(!mConvexMeshBodyEdgesInfo->testPointInside(mLocalShapeToWorld * vec3(-1, 4, -2.5)));
			test(!mConvexMeshBodyEdgesInfo->testPointInside(mLocalShapeToWorld * vec3(1, -2, 4.5)));

			// Tests with ProxyConvexMeshShape
			test(mConvexMeshProxyShapeEdgesInfo->testPointInside(mLocalShapeToWorld * vec3(0, 0, 0)));
			test(mConvexMeshProxyShapeEdgesInfo->testPointInside(mLocalShapeToWorld * vec3(-1.9, 0, 0)));
			test(mConvexMeshProxyShapeEdgesInfo->testPointInside(mLocalShapeToWorld * vec3(1.9, 0, 0)));
			test(mConvexMeshProxyShapeEdgesInfo->testPointInside(mLocalShapeToWorld * vec3(0, -2.9, 0)));
			test(mConvexMeshProxyShapeEdgesInfo->testPointInside(mLocalShapeToWorld * vec3(0, 2.9, 0)));
			test(mConvexMeshProxyShapeEdgesInfo->testPointInside(mLocalShapeToWorld * vec3(0, 0, -3.9)));
			test(mConvexMeshProxyShapeEdgesInfo->testPointInside(mLocalShapeToWorld * vec3(0, 0, 3.9)));
			test(mConvexMeshProxyShapeEdgesInfo->testPointInside(mLocalShapeToWorld * vec3(-1.9, -2.9, -3.9)));
			test(mConvexMeshProxyShapeEdgesInfo->testPointInside(mLocalShapeToWorld * vec3(1.9, 2.9, 3.9)));
			test(mConvexMeshProxyShapeEdgesInfo->testPointInside(mLocalShapeToWorld * vec3(-1, -2, -1.5)));
			test(mConvexMeshProxyShapeEdgesInfo->testPointInside(mLocalShapeToWorld * vec3(-1, 2, -2.5)));
			test(mConvexMeshProxyShapeEdgesInfo->testPointInside(mLocalShapeToWorld * vec3(1, -2, 3.5)));

			test(!mConvexMeshProxyShapeEdgesInfo->testPointInside(mLocalShapeToWorld * vec3(-2.1, 0, 0)));
			test(!mConvexMeshProxyShapeEdgesInfo->testPointInside(mLocalShapeToWorld * vec3(2.1, 0, 0)));
			test(!mConvexMeshProxyShapeEdgesInfo->testPointInside(mLocalShapeToWorld * vec3(0, -3.1, 0)));
			test(!mConvexMeshProxyShapeEdgesInfo->testPointInside(mLocalShapeToWorld * vec3(0, 3.1, 0)));
			test(!mConvexMeshProxyShapeEdgesInfo->testPointInside(mLocalShapeToWorld * vec3(0, 0, -4.1)));
			test(!mConvexMeshProxyShapeEdgesInfo->testPointInside(mLocalShapeToWorld * vec3(0, 0, 4.1)));
			test(!mConvexMeshProxyShapeEdgesInfo->testPointInside(mLocalShapeToWorld * vec3(-2.1, -3.1, -4.1)));
			test(!mConvexMeshProxyShapeEdgesInfo->testPointInside(mLocalShapeToWorld * vec3(2.1, 3.1, 4.1)));
			test(!mConvexMeshProxyShapeEdgesInfo->testPointInside(mLocalShapeToWorld * vec3(-10, -2, -1.5)));
			test(!mConvexMeshProxyShapeEdgesInfo->testPointInside(mLocalShapeToWorld * vec3(-1, 4, -2.5)));
			test(!mConvexMeshProxyShapeEdgesInfo->testPointInside(mLocalShapeToWorld * vec3(1, -2, 4.5)));
		}

		/// Test the ProxyCylinderShape::testPointInside() and
		/// CollisionBody::testPointInside() methods
		void testCylinder() {

			// Tests with CollisionBody
			test(mCylinderBody->testPointInside(mLocalShapeToWorld * vec3(0, 0, 0)));
			test(mCylinderBody->testPointInside(mLocalShapeToWorld * vec3(0, 3.9, 0)));
			test(mCylinderBody->testPointInside(mLocalShapeToWorld * vec3(0, -3.9, 0)));
			test(mCylinderBody->testPointInside(mLocalShapeToWorld * vec3(2.9, 0, 0)));
			test(mCylinderBody->testPointInside(mLocalShapeToWorld * vec3(-2.9, 0, 0)));
			test(mCylinderBody->testPointInside(mLocalShapeToWorld * vec3(0, 0, 2.9)));
			test(mCylinderBody->testPointInside(mLocalShapeToWorld * vec3(0, 0, -2.9)));
			test(mCylinderBody->testPointInside(mLocalShapeToWorld * vec3(1.7, 0, 1.7)));
			test(mCylinderBody->testPointInside(mLocalShapeToWorld * vec3(1.7, 0, -1.7)));
			test(mCylinderBody->testPointInside(mLocalShapeToWorld * vec3(-1.7, 0, -1.7)));
			test(mCylinderBody->testPointInside(mLocalShapeToWorld * vec3(-1.7, 0, 1.7)));
			test(mCylinderBody->testPointInside(mLocalShapeToWorld * vec3(2.9, 3.9, 0)));
			test(mCylinderBody->testPointInside(mLocalShapeToWorld * vec3(-2.9, 3.9, 0)));
			test(mCylinderBody->testPointInside(mLocalShapeToWorld * vec3(0, 3.9, 2.9)));
			test(mCylinderBody->testPointInside(mLocalShapeToWorld * vec3(0, 3.9, -2.9)));
			test(mCylinderBody->testPointInside(mLocalShapeToWorld * vec3(1.7, 3.9, 1.7)));
			test(mCylinderBody->testPointInside(mLocalShapeToWorld * vec3(1.7, 3.9, -1.7)));
			test(mCylinderBody->testPointInside(mLocalShapeToWorld * vec3(-1.7, 3.9, -1.7)));
			test(mCylinderBody->testPointInside(mLocalShapeToWorld * vec3(-1.7, 3.9, 1.7)));
			test(mCylinderBody->testPointInside(mLocalShapeToWorld * vec3(2.9, -3.9, 0)));
			test(mCylinderBody->testPointInside(mLocalShapeToWorld * vec3(-2.9, -3.9, 0)));
			test(mCylinderBody->testPointInside(mLocalShapeToWorld * vec3(0, -3.9, 2.9)));
			test(mCylinderBody->testPointInside(mLocalShapeToWorld * vec3(0, -3.9, -2.9)));
			test(mCylinderBody->testPointInside(mLocalShapeToWorld * vec3(1.7, -3.9, 1.7)));
			test(mCylinderBody->testPointInside(mLocalShapeToWorld * vec3(1.7, -3.9, -1.7)));
			test(mCylinderBody->testPointInside(mLocalShapeToWorld * vec3(-1.7, -3.9, -1.7)));
			test(mCylinderBody->testPointInside(mLocalShapeToWorld * vec3(-1.7, -3.9, 1.7)));

			test(!mCylinderBody->testPointInside(mLocalShapeToWorld * vec3(0, 4.1, 0)));
			test(!mCylinderBody->testPointInside(mLocalShapeToWorld * vec3(0, -4.1, 0)));
			test(!mCylinderBody->testPointInside(mLocalShapeToWorld * vec3(3.1, 0, 0)));
			test(!mCylinderBody->testPointInside(mLocalShapeToWorld * vec3(-3.1, 0, 0)));
			test(!mCylinderBody->testPointInside(mLocalShapeToWorld * vec3(0, 0, 3.1)));
			test(!mCylinderBody->testPointInside(mLocalShapeToWorld * vec3(0, 0, -3.1)));
			test(!mCylinderBody->testPointInside(mLocalShapeToWorld * vec3(2.2, 0, 2.2)));
			test(!mCylinderBody->testPointInside(mLocalShapeToWorld * vec3(2.2, 0, -2.2)));
			test(!mCylinderBody->testPointInside(mLocalShapeToWorld * vec3(-2.2, 0, -2.2)));
			test(!mCylinderBody->testPointInside(mLocalShapeToWorld * vec3(-1.3, 0, 2.8)));
			test(!mCylinderBody->testPointInside(mLocalShapeToWorld * vec3(3.1, 3.9, 0)));
			test(!mCylinderBody->testPointInside(mLocalShapeToWorld * vec3(-3.1, 3.9, 0)));
			test(!mCylinderBody->testPointInside(mLocalShapeToWorld * vec3(0, 3.9, 3.1)));
			test(!mCylinderBody->testPointInside(mLocalShapeToWorld * vec3(0, 3.9, -3.1)));
			test(!mCylinderBody->testPointInside(mLocalShapeToWorld * vec3(2.2, 3.9, 2.2)));
			test(!mCylinderBody->testPointInside(mLocalShapeToWorld * vec3(2.2, 3.9, -2.2)));
			test(!mCylinderBody->testPointInside(mLocalShapeToWorld * vec3(-2.2, 3.9, -2.2)));
			test(!mCylinderBody->testPointInside(mLocalShapeToWorld * vec3(-2.2, 3.9, 2.2)));
			test(!mCylinderBody->testPointInside(mLocalShapeToWorld * vec3(3.1, -3.9, 0)));
			test(!mCylinderBody->testPointInside(mLocalShapeToWorld * vec3(-3.1, -3.9, 0)));
			test(!mCylinderBody->testPointInside(mLocalShapeToWorld * vec3(0, -3.9, 3.1)));
			test(!mCylinderBody->testPointInside(mLocalShapeToWorld * vec3(0, -3.9, -3.1)));
			test(!mCylinderBody->testPointInside(mLocalShapeToWorld * vec3(2.2, -3.9, 2.2)));
			test(!mCylinderBody->testPointInside(mLocalShapeToWorld * vec3(2.2, -3.9, -2.2)));
			test(!mCylinderBody->testPointInside(mLocalShapeToWorld * vec3(-2.2, -3.9, -2.2)));
			test(!mCylinderBody->testPointInside(mLocalShapeToWorld * vec3(-2.2, -3.9, 2.2)));

			// Tests with ProxyCylinderShape
			test(mCylinderProxyShape->testPointInside(mLocalShapeToWorld * vec3(0, 0, 0)));
			test(mCylinderProxyShape->testPointInside(mLocalShapeToWorld * vec3(0, 3.9, 0)));
			test(mCylinderProxyShape->testPointInside(mLocalShapeToWorld * vec3(0, -3.9, 0)));
			test(mCylinderProxyShape->testPointInside(mLocalShapeToWorld * vec3(2.9, 0, 0)));
			test(mCylinderProxyShape->testPointInside(mLocalShapeToWorld * vec3(-2.9, 0, 0)));
			test(mCylinderProxyShape->testPointInside(mLocalShapeToWorld * vec3(0, 0, 2.9)));
			test(mCylinderProxyShape->testPointInside(mLocalShapeToWorld * vec3(0, 0, -2.9)));
			test(mCylinderProxyShape->testPointInside(mLocalShapeToWorld * vec3(1.7, 0, 1.7)));
			test(mCylinderProxyShape->testPointInside(mLocalShapeToWorld * vec3(1.7, 0, -1.7)));
			test(mCylinderProxyShape->testPointInside(mLocalShapeToWorld * vec3(-1.7, 0, -1.7)));
			test(mCylinderProxyShape->testPointInside(mLocalShapeToWorld * vec3(-1.7, 0, 1.7)));
			test(mCylinderProxyShape->testPointInside(mLocalShapeToWorld * vec3(2.9, 3.9, 0)));
			test(mCylinderProxyShape->testPointInside(mLocalShapeToWorld * vec3(-2.9, 3.9, 0)));
			test(mCylinderProxyShape->testPointInside(mLocalShapeToWorld * vec3(0, 3.9, 2.9)));
			test(mCylinderProxyShape->testPointInside(mLocalShapeToWorld * vec3(0, 3.9, -2.9)));
			test(mCylinderProxyShape->testPointInside(mLocalShapeToWorld * vec3(1.7, 3.9, 1.7)));
			test(mCylinderProxyShape->testPointInside(mLocalShapeToWorld * vec3(1.7, 3.9, -1.7)));
			test(mCylinderProxyShape->testPointInside(mLocalShapeToWorld * vec3(-1.7, 3.9, -1.7)));
			test(mCylinderProxyShape->testPointInside(mLocalShapeToWorld * vec3(-1.7, 3.9, 1.7)));
			test(mCylinderProxyShape->testPointInside(mLocalShapeToWorld * vec3(2.9, -3.9, 0)));
			test(mCylinderProxyShape->testPointInside(mLocalShapeToWorld * vec3(-2.9, -3.9, 0)));
			test(mCylinderProxyShape->testPointInside(mLocalShapeToWorld * vec3(0, -3.9, 2.9)));
			test(mCylinderProxyShape->testPointInside(mLocalShapeToWorld * vec3(0, -3.9, -2.9)));
			test(mCylinderProxyShape->testPointInside(mLocalShapeToWorld * vec3(1.7, -3.9, 1.7)));
			test(mCylinderProxyShape->testPointInside(mLocalShapeToWorld * vec3(1.7, -3.9, -1.7)));
			test(mCylinderProxyShape->testPointInside(mLocalShapeToWorld * vec3(-1.7, -3.9, -1.7)));
			test(mCylinderProxyShape->testPointInside(mLocalShapeToWorld * vec3(-1.7, -3.9, 1.7)));

			test(!mCylinderProxyShape->testPointInside(mLocalShapeToWorld * vec3(0, 4.1, 0)));
			test(!mCylinderProxyShape->testPointInside(mLocalShapeToWorld * vec3(0, -4.1, 0)));
			test(!mCylinderProxyShape->testPointInside(mLocalShapeToWorld * vec3(3.1, 0, 0)));
			test(!mCylinderProxyShape->testPointInside(mLocalShapeToWorld * vec3(-3.1, 0, 0)));
			test(!mCylinderProxyShape->testPointInside(mLocalShapeToWorld * vec3(0, 0, 3.1)));
			test(!mCylinderProxyShape->testPointInside(mLocalShapeToWorld * vec3(0, 0, -3.1)));
			test(!mCylinderProxyShape->testPointInside(mLocalShapeToWorld * vec3(2.2, 0, 2.2)));
			test(!mCylinderProxyShape->testPointInside(mLocalShapeToWorld * vec3(2.2, 0, -2.2)));
			test(!mCylinderProxyShape->testPointInside(mLocalShapeToWorld * vec3(-2.2, 0, -2.2)));
			test(!mCylinderProxyShape->testPointInside(mLocalShapeToWorld * vec3(-1.3, 0, 2.8)));
			test(!mCylinderProxyShape->testPointInside(mLocalShapeToWorld * vec3(3.1, 3.9, 0)));
			test(!mCylinderProxyShape->testPointInside(mLocalShapeToWorld * vec3(-3.1, 3.9, 0)));
			test(!mCylinderProxyShape->testPointInside(mLocalShapeToWorld * vec3(0, 3.9, 3.1)));
			test(!mCylinderProxyShape->testPointInside(mLocalShapeToWorld * vec3(0, 3.9, -3.1)));
			test(!mCylinderProxyShape->testPointInside(mLocalShapeToWorld * vec3(2.2, 3.9, 2.2)));
			test(!mCylinderProxyShape->testPointInside(mLocalShapeToWorld * vec3(2.2, 3.9, -2.2)));
			test(!mCylinderProxyShape->testPointInside(mLocalShapeToWorld * vec3(-2.2, 3.9, -2.2)));
			test(!mCylinderProxyShape->testPointInside(mLocalShapeToWorld * vec3(-2.2, 3.9, 2.2)));
			test(!mCylinderProxyShape->testPointInside(mLocalShapeToWorld * vec3(3.1, -3.9, 0)));
			test(!mCylinderProxyShape->testPointInside(mLocalShapeToWorld * vec3(-3.1, -3.9, 0)));
			test(!mCylinderProxyShape->testPointInside(mLocalShapeToWorld * vec3(0, -3.9, 3.1)));
			test(!mCylinderProxyShape->testPointInside(mLocalShapeToWorld * vec3(0, -3.9, -3.1)));
			test(!mCylinderProxyShape->testPointInside(mLocalShapeToWorld * vec3(2.2, -3.9, 2.2)));
			test(!mCylinderProxyShape->testPointInside(mLocalShapeToWorld * vec3(2.2, -3.9, -2.2)));
			test(!mCylinderProxyShape->testPointInside(mLocalShapeToWorld * vec3(-2.2, -3.9, -2.2)));
			test(!mCylinderProxyShape->testPointInside(mLocalShapeToWorld * vec3(-2.2, -3.9, 2.2)));
		}

		/// Test the CollisionBody::testPointInside() method
		void testCompound() {

			// Points on the cylinder
			test(mCompoundBody->testPointInside(mLocalShapeToWorld * vec3(0, 0, 0)));
			test(mCompoundBody->testPointInside(mLocalShapeToWorld * vec3(0, 3.9, 0)));
			test(mCompoundBody->testPointInside(mLocalShapeToWorld * vec3(0, -3.9, 0)));
			test(mCompoundBody->testPointInside(mLocalShapeToWorld * vec3(2.9, 0, 0)));
			test(mCompoundBody->testPointInside(mLocalShapeToWorld * vec3(-2.9, 0, 0)));
			test(mCompoundBody->testPointInside(mLocalShapeToWorld * vec3(0, 0, 2.9)));
			test(mCompoundBody->testPointInside(mLocalShapeToWorld * vec3(0, 0, -2.9)));
			test(mCompoundBody->testPointInside(mLocalShapeToWorld * vec3(1.7, 0, 1.7)));
			test(mCompoundBody->testPointInside(mLocalShapeToWorld * vec3(1.7, 0, -1.7)));
			test(mCompoundBody->testPointInside(mLocalShapeToWorld * vec3(-1.7, 0, -1.7)));
			test(mCompoundBody->testPointInside(mLocalShapeToWorld * vec3(-1.7, 0, 1.7)));
			test(mCompoundBody->testPointInside(mLocalShapeToWorld * vec3(2.9, 3.9, 0)));
			test(mCompoundBody->testPointInside(mLocalShapeToWorld * vec3(-2.9, 3.9, 0)));
			test(mCompoundBody->testPointInside(mLocalShapeToWorld * vec3(0, 3.9, 2.9)));
			test(mCompoundBody->testPointInside(mLocalShapeToWorld * vec3(0, 3.9, -2.9)));
			test(mCompoundBody->testPointInside(mLocalShapeToWorld * vec3(1.7, 3.9, 1.7)));
			test(mCompoundBody->testPointInside(mLocalShapeToWorld * vec3(1.7, 3.9, -1.7)));
			test(mCompoundBody->testPointInside(mLocalShapeToWorld * vec3(-1.7, 3.9, -1.7)));
			test(mCompoundBody->testPointInside(mLocalShapeToWorld * vec3(-1.7, 3.9, 1.7)));
			test(mCompoundBody->testPointInside(mLocalShapeToWorld * vec3(2.9, -3.9, 0)));
			test(mCompoundBody->testPointInside(mLocalShapeToWorld * vec3(-2.9, -3.9, 0)));
			test(mCompoundBody->testPointInside(mLocalShapeToWorld * vec3(0, -3.9, 2.9)));
			test(mCompoundBody->testPointInside(mLocalShapeToWorld * vec3(0, -3.9, -2.9)));
			test(mCompoundBody->testPointInside(mLocalShapeToWorld * vec3(1.7, -3.9, 1.7)));
			test(mCompoundBody->testPointInside(mLocalShapeToWorld * vec3(1.7, -3.9, -1.7)));
			test(mCompoundBody->testPointInside(mLocalShapeToWorld * vec3(-1.7, -3.9, -1.7)));
			test(mCompoundBody->testPointInside(mLocalShapeToWorld * vec3(-1.7, -3.9, 1.7)));

			// Points on the sphere
			test(mCompoundBody->testPointInside(mLocalShape2ToWorld * vec3(0, 0, 0)));
			test(mCompoundBody->testPointInside(mLocalShape2ToWorld * vec3(2.9, 0, 0)));
			test(mCompoundBody->testPointInside(mLocalShape2ToWorld * vec3(-2.9, 0, 0)));
			test(mCompoundBody->testPointInside(mLocalShape2ToWorld * vec3(0, 2.9, 0)));
			test(mCompoundBody->testPointInside(mLocalShape2ToWorld * vec3(0, -2.9, 0)));
			test(mCompoundBody->testPointInside(mLocalShape2ToWorld * vec3(0, 0, 2.9)));
			test(mCompoundBody->testPointInside(mLocalShape2ToWorld * vec3(0, 0, 2.9)));
			test(mCompoundBody->testPointInside(mLocalShape2ToWorld * vec3(-1, -2, -1.5)));
			test(mCompoundBody->testPointInside(mLocalShape2ToWorld * vec3(-1, 2, -1.5)));
			test(mCompoundBody->testPointInside(mLocalShape2ToWorld * vec3(1, -2, 1.5)));
		}
 };

}

#endif
