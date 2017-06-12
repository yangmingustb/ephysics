/** @file
 * @author Daniel Chappuis
 * @copyright 2010-2016 Daniel Chappuis
 * @license BSD 3 clauses (see license file)
 */
#pragma once

// Libraries
#include <ephysics/memory/MemoryAllocator.h>
#include <ephysics/body/RigidBody.h>
#include <ephysics/constraint/Joint.h>
#include <ephysics/collision/ContactManifold.h>

namespace reactphysics3d {

// Class Island
/**
 * An island represent an isolated group of awake bodies that are connected with each other by
 * some contraints (contacts or joints).
 */
class Island {

	private:

		// -------------------- Attributes -------------------- //

		/// Array with all the bodies of the island
		RigidBody** m_bodies;

		/// Array with all the contact manifolds between bodies of the island
		ContactManifold** m_contactManifolds;

		/// Array with all the joints between bodies of the island
		Joint** m_joints;

		/// Current number of bodies in the island
		uint32_t m_numberBodies;

		/// Current number of contact manifold in the island
		uint32_t m_numberContactManifolds;

		/// Current number of joints in the island
		uint32_t m_numberJoints;

		/// Reference to the memory allocator
		MemoryAllocator& m_memoryAllocator;

		/// Number of bytes allocated for the bodies array
		size_t m_numberAllocatedBytesBodies;

		/// Number of bytes allocated for the contact manifolds array
		size_t m_numberAllocatedBytesContactManifolds;

		/// Number of bytes allocated for the joints array
		size_t m_numberAllocatedBytesJoints;

		// -------------------- Methods -------------------- //

		/// Private assignment operator
		Island& operator=(const Island& island);

		/// Private copy-constructor
		Island(const Island& island);

	public:

		// -------------------- Methods -------------------- //

		/// Constructor
		Island(uint32_t nbMaxBodies, uint32_t nbMaxContactManifolds, uint32_t nbMaxJoints,
			   MemoryAllocator& memoryAllocator);

		/// Destructor
		~Island();

		/// Add a body int32_to the island
		void addBody(RigidBody* body);

		/// Add a contact manifold int32_to the island
		void addContactManifold(ContactManifold* contactManifold);

		/// Add a joint int32_to the island
		void addJoint(Joint* joint);

		/// Return the number of bodies in the island
		uint32_t getNbBodies() const;

		/// Return the number of contact manifolds in the island
		uint32_t getNbContactManifolds() const;

		/// Return the number of joints in the island
		uint32_t getNbJoints() const;

		/// Return a pointer to the array of bodies
		RigidBody** getBodies();

		/// Return a pointer to the array of contact manifolds
		ContactManifold** getContactManifold();

		/// Return a pointer to the array of joints
		Joint** getJoints();

		// -------------------- Friendship -------------------- //

		friend class DynamicsWorld;
};

// Add a body int32_to the island
inline void Island::addBody(RigidBody* body) {
	assert(!body->isSleeping());
	m_bodies[m_numberBodies] = body;
	m_numberBodies++;
}

// Add a contact manifold int32_to the island
inline void Island::addContactManifold(ContactManifold* contactManifold) {
	m_contactManifolds[m_numberContactManifolds] = contactManifold;
	m_numberContactManifolds++;
}

// Add a joint int32_to the island
inline void Island::addJoint(Joint* joint) {
	m_joints[m_numberJoints] = joint;
	m_numberJoints++;
}

// Return the number of bodies in the island
inline uint32_t Island::getNbBodies() const {
	return m_numberBodies;
}

// Return the number of contact manifolds in the island
inline uint32_t Island::getNbContactManifolds() const {
	return m_numberContactManifolds;
}

// Return the number of joints in the island
inline uint32_t Island::getNbJoints() const {
	return m_numberJoints;
}

// Return a pointer to the array of bodies
inline RigidBody** Island::getBodies() {
	return m_bodies;
}

// Return a pointer to the array of contact manifolds
inline ContactManifold** Island::getContactManifold() {
	return m_contactManifolds;
}

// Return a pointer to the array of joints
inline Joint** Island::getJoints() {
	return m_joints;
}

}
