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

#ifndef COLLISION_SHAPES_SCENE_H
#define COLLISION_SHAPES_SCENE_H

// Libraries
#include <ephysics/openglframework.hpp>
#include <ephysics/ephysics.hpp>
#include <ephysics/SceneDemo.hpp>
#include <ephysics/Sphere.hpp>
#include <ephysics/Box.hpp>
#include <ephysics/Cone.hpp>
#include <ephysics/Cylinder.hpp>
#include <ephysics/Capsule.hpp>
#include <ephysics/ConvexMesh.hpp>
#include <ephysics/ConcaveMesh.hpp>
#include <ephysics/Dumbbell.hpp>
#include <ephysics/VisualContactPoint.hpp>

namespace collisionshapesscene {

// Constants
const float SCENE_RADIUS = 30.0f;
const int32_t NB_BOXES = 5;
const int32_t NB_SPHERES = 5;
const int32_t NB_CONES = 5;
const int32_t NB_CYLINDERS = 5;
const int32_t NB_CAPSULES = 5;
const int32_t NB_MESHES = 3;
const int32_t NB_COMPOUND_SHAPES = 3;
const openglframework::vec3 BOX_SIZE(2, 2, 2);
const float SPHERE_RADIUS = 1.5f;
const float CONE_RADIUS = 2.0f;
const float CONE_HEIGHT = 3.0f;
const float CYLINDER_RADIUS = 1.0f;
const float CYLINDER_HEIGHT = 5.0f;
const float CAPSULE_RADIUS = 1.0f;
const float CAPSULE_HEIGHT = 1.0f;
const float DUMBBELL_HEIGHT = 1.0f;
const openglframework::vec3 FLOOR_SIZE(50, 0.5f, 50);		// Floor dimensions in meters
const float BOX_MASS = 1.0f;
const float CONE_MASS = 1.0f;
const float CYLINDER_MASS = 1.0f;
const float CAPSULE_MASS = 1.0f;
const float MESH_MASS = 1.0f;
const float FLOOR_MASS = 100.0f;							// Floor mass in kilograms

// Class CollisionShapesScene
class CollisionShapesScene : public SceneDemo {

	private :

		// -------------------- Attributes -------------------- //

		/// All the spheres of the scene
		etk::Vector<Box*> mBoxes;

		etk::Vector<Sphere*> mSpheres;

		etk::Vector<Cone*> mCones;

		etk::Vector<Cylinder*> mCylinders;

		etk::Vector<Capsule*> mCapsules;

		/// All the convex meshes of the scene
		etk::Vector<ConvexMesh*> mConvexMeshes;

		/// All the dumbbell of the scene
		etk::Vector<Dumbbell*> mDumbbells;

		/// Box for the floor
		Box* mFloor;

		/// Dynamics world used for the physics simulation
		ephysics::DynamicsWorld* mDynamicsWorld;

	public:

		// -------------------- Methods -------------------- //

		/// Constructor
		CollisionShapesScene(const etk::String& name);

		/// Destructor
		virtual ~CollisionShapesScene();

		/// Update the physics world (take a simulation step)
		/// Can be called several times per frame
		virtual void updatePhysics();

		/// Take a step for the simulation
		virtual void update();

		/// Render the scene in a single pass
		virtual void renderSinglePass(openglframework::Shader& shader,
									  const openglframework::Matrix4& worldToCameraMatrix);

		/// Reset the scene
		virtual void reset();

		/// Return all the contact points of the scene
		virtual etk::Vector<ContactPoint> getContactPoints() const;
};

// Return all the contact points of the scene
inline etk::Vector<ContactPoint> CollisionShapesScene::getContactPoints() const {
	return computeContactPointsOfWorld(mDynamicsWorld);
}

}

#endif
