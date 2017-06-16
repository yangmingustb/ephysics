/** @file
 * @author Daniel Chappuis
 * @copyright 2010-2016 Daniel Chappuis
 * @license BSD 3 clauses (see license file)
 */
#pragma once

#include <cassert>
#include <ephysics/body/CollisionBody.hpp>
#include <ephysics/engine/Material.hpp>
#include <ephysics/mathematics/mathematics.hpp>
#include <ephysics/memory/MemoryAllocator.hpp>

namespace ephysics {

// Class declarations
struct JointListElement;
class Joint;
class DynamicsWorld;

/**
 * @brief This class represents a rigid body of the physics
 * engine. A rigid body is a non-deformable body that
 * has a constant mass. This class inherits from the
 * CollisionBody class.
 */
class RigidBody : public CollisionBody {
	protected :
		float m_initMass; //!< Intial mass of the body
		vec3 m_centerOfMassLocal; //!< Center of mass of the body in local-space coordinates. The center of mass can therefore be different from the body origin
		vec3 m_centerOfMassWorld; //!< Center of mass of the body in world-space coordinates
		vec3 m_linearVelocity; //!< Linear velocity of the body
		vec3 m_angularVelocity; //!< Angular velocity of the body
		vec3 m_externalForce; //!< Current external force on the body
		vec3 m_externalTorque; //!< Current external torque on the body
		etk::Matrix3x3 m_inertiaTensorLocal; //!< Local inertia tensor of the body (in local-space) with respect to the center of mass of the body
		etk::Matrix3x3 m_inertiaTensorLocalInverse; //!< Inverse of the inertia tensor of the body
		float m_massInverse; //!< Inverse of the mass of the body
		bool m_isGravityEnabled; //!< True if the gravity needs to be applied to this rigid body
		Material m_material; //!< Material properties of the rigid body
		float m_linearDamping; //!< Linear velocity damping factor
		float m_angularDamping; //!< Angular velocity damping factor
		JointListElement* m_jointsList; //!< First element of the linked list of joints involving this body
		/// Private copy-constructor
		RigidBody(const RigidBody& body);

		/// Private assignment operator
		RigidBody& operator=(const RigidBody& body);

		/**
		 * @brief Remove a joint from the joints list
		 */
		void removeJointFrom_jointsList(MemoryAllocator& _memoryAllocator, const Joint* _joint);
		/**
		 * @brief Update the transform of the body after a change of the center of mass
		 */
		void updateTransformWithCenterOfMass() {
			// Translate the body according to the translation of the center of mass position
			m_transform.setPosition(m_centerOfMassWorld - m_transform.getOrientation() * m_centerOfMassLocal);
		}
		/**
		 * @brief Update the broad-phase state for this body (because it has moved for instance)
		 */
		virtual void updateBroadPhaseState() const;
	public :
		/**
		 * @brief Constructor
		 * @param transform The transformation of the body
		 * @param world The world where the body has been added
		 * @param id The ID of the body
		 */
		RigidBody(const etk::Transform3D& transform, CollisionWorld& world, bodyindex id);

		/// Destructor
		virtual ~RigidBody();
		void setType(BodyType _type); // TODO: override
		/**
		 * @brief Set the current position and orientation
		 * @param[in] _transform The transformation of the body that transforms the local-space of the body int32_to world-space
		 */
		virtual void setTransform(const etk::Transform3D& _transform);
		/**
		 * @brief Get the mass of the body
		 * @return The mass (in kilograms) of the body
		 */
		float getMass() const {
			return m_initMass;
		}
		/**
		 * @brief Get the linear velocity
		 * @return The linear velocity vector of the body
		 */
		vec3 getLinearVelocity() const {
			return m_linearVelocity;
		}
		/**
		 * @brief Set the linear velocity of the rigid body.
		 * @param[in] _linearVelocity Linear velocity vector of the body
		 */
		void setLinearVelocity(const vec3& _linearVelocity);
		/**
		 * @brief Get the angular velocity of the body
		 * @return The angular velocity vector of the body
		 */
		vec3 getAngularVelocity() const {
			return m_angularVelocity;
		}
		/**
		* @brief Set the angular velocity.
		* @param[in] _angularVelocity The angular velocity vector of the body
		*/
		void setAngularVelocity(const vec3& _angularVelocity);
		/**
		 * @brief Set the variable to know whether or not the body is sleeping
		 * @param[in] _isSleeping New sleeping state of the body
		 */
		virtual void setIsSleeping(bool _isSleeping);
		/**
		 * @brief Get the local inertia tensor of the body (in local-space coordinates)
		 * @return The 3x3 inertia tensor matrix of the body
		 */
		const etk::Matrix3x3& getInertiaTensorLocal() const {
			return m_inertiaTensorLocal;
		}

		/// Set the local inertia tensor of the body (in body coordinates)
		void setInertiaTensorLocal(const etk::Matrix3x3& inertiaTensorLocal);

		/// Set the local center of mass of the body (in local-space coordinates)
		void setCenterOfMassLocal(const vec3& centerOfMassLocal);
		/**
		 * @brief Set the mass of the rigid body
		 * @param[in] _mass The mass (in kilograms) of the body
		 */
		void setMass(float _mass);

		/**
		 * @brief Get the inertia tensor in world coordinates.
		 * The inertia tensor I_w in world coordinates is computed
		 * with the local inertia tensor I_b in body coordinates
		 * by I_w = R * I_b * R^T
		 * where R is the rotation matrix (and R^T its transpose) of
		 * the current orientation quaternion of the body
		 * @return The 3x3 inertia tensor matrix of the body in world-space coordinates
		 */
		etk::Matrix3x3 getInertiaTensorWorld() const {
			// Compute and return the inertia tensor in world coordinates
			return m_transform.getOrientation().getMatrix() * m_inertiaTensorLocal *
			       m_transform.getOrientation().getMatrix().getTranspose();
		}
		
		/**
		 * @brief Get the inverse of the inertia tensor in world coordinates.
		 * The inertia tensor I_w in world coordinates is computed with the
		 * local inverse inertia tensor I_b^-1 in body coordinates
		 * by I_w = R * I_b^-1 * R^T
		 * where R is the rotation matrix (and R^T its transpose) of the
		 * current orientation quaternion of the body
		 * @return The 3x3 inverse inertia tensor matrix of the body in world-space coordinates
		 */
		etk::Matrix3x3 getInertiaTensorInverseWorld() const {
			// TODO : DO NOT RECOMPUTE THE MATRIX MULTIPLICATION EVERY TIME. WE NEED TO STORE THE
			//		INVERSE WORLD TENSOR IN THE CLASS AND UPLDATE IT WHEN THE ORIENTATION OF THE BODY CHANGES
			// Compute and return the inertia tensor in world coordinates
			return m_transform.getOrientation().getMatrix() * m_inertiaTensorLocalInverse *
			       m_transform.getOrientation().getMatrix().getTranspose();
		}
		/**
		 * @brief get the need of gravity appling to this rigid body
		 * @return True if the gravity is applied to the body
		 */
		bool isGravityEnabled() const {
			return m_isGravityEnabled;
		}
		/**
		 * @brief Set the variable to know if the gravity is applied to this rigid body
		 * @param[in] _isEnabled True if you want the gravity to be applied to this body
		 */
		void enableGravity(bool _isEnabled) {
			m_isGravityEnabled = _isEnabled;
		}
		/**
		 * @brief get a reference to the material properties of the rigid body
		 * @return A reference to the material of the body
		 */
		Material& getMaterial() {
			return m_material;
		}
		/**
		 * @brief Set a new material for this rigid body
		 * @param[in] _material The material you want to set to the body
		 */
		void setMaterial(const Material& _material) {
			m_material = _material;
		}
		/**
		 * @brief Get the linear velocity damping factor
		 * @return The linear damping factor of this body
		 */
		float getLinearDamping() const {
			return m_linearDamping;
		}
		/**
		 * @brief Set the linear damping factor. This is the ratio of the linear velocity that the body will lose every at seconds of simulation.
		 * @param[in] _linearDamping The linear damping factor of this body
		 */
		void setLinearDamping(float _linearDamping) {
			assert(_linearDamping >= 0.0f);
			m_linearDamping = _linearDamping;
		}
		/**
		 * @brief Get the angular velocity damping factor
		 * @return The angular damping factor of this body
		 */
		float getAngularDamping() const {
			return m_angularDamping;
		}
		/**
		 * @brief Set the angular damping factor. This is the ratio of the angular velocity that the body will lose at every seconds of simulation.
		 * @param[in] _angularDamping The angular damping factor of this body
		 */
		void setAngularDamping(float _angularDamping) {
			assert(_angularDamping >= 0.0f);
			m_angularDamping = _angularDamping;
		}
		/**
		 * @brief Get the first element of the linked list of joints involving this body
		 * @return The first element of the linked-list of all the joints involving this body
		 */
		const JointListElement* getJointsList() const {
			return m_jointsList;
		}
		/**
		 * @brief Get the first element of the linked list of joints involving this body
		 * @return The first element of the linked-list of all the joints involving this body
		 */
		JointListElement* getJointsList() {
			return m_jointsList;
		}
		/**
		 * @brief Apply an external force to the body at its center of mass.
		 * If the body is sleeping, calling this method will wake it up. Note that the
		 * force will we added to the sum of the applied forces and that this sum will be
		 * reset to zero at the end of each call of the DynamicsWorld::update() method.
		 * You can only apply a force to a dynamic body otherwise, this method will do nothing.
		 * @param[in] _force The external force to apply on the center of mass of the body
		 */
		void applyForceToCenterOfMass(const vec3& _force);
		/**
		 * @brief Apply an external force to the body at a given point (in world-space coordinates).
		 * If the point is not at the center of mass of the body, it will also
		 * generate some torque and therefore, change the angular velocity of the body.
		 * If the body is sleeping, calling this method will wake it up. Note that the
		 * force will we added to the sum of the applied forces and that this sum will be
		 * reset to zero at the end of each call of the DynamicsWorld::update() method.
		 * You can only apply a force to a dynamic body otherwise, this method will do nothing.
		 * @param[in] _force The force to apply on the body
		 * @param[in] _point The point where the force is applied (in world-space coordinates)
		 */
		void applyForce(const vec3& _force, const vec3& _point);
		/**
		 * @brief Apply an external torque to the body.
		 * If the body is sleeping, calling this method will wake it up. Note that the
		 * force will we added to the sum of the applied torques and that this sum will be
		 * reset to zero at the end of each call of the DynamicsWorld::update() method.
		 * You can only apply a force to a dynamic body otherwise, this method will do nothing.
		 * @param[in] _torque The external torque to apply on the body
		 */
		void applyTorque(const vec3& _torque);

		/// Add a collision shape to the body.
		virtual ProxyShape* addCollisionShape(CollisionShape* collisionShape,
											  const etk::Transform3D& transform,
											  float mass);

		/// Remove a collision shape from the body
		virtual void removeCollisionShape(const ProxyShape* proxyShape);

		/// Recompute the center of mass, total mass and inertia tensor of the body using all
		/// the collision shapes attached to the body.
		void recomputeMassInformation();

		friend class DynamicsWorld;
		friend class ContactSolver;
		friend class BallAndSocketJoint;
		friend class SliderJoint;
		friend class HingeJoint;
		friend class FixedJoint;
};














}
