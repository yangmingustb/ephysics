/** @file
 * Original ReactPhysics3D C++ library by Daniel Chappuis <http://www.reactphysics3d.com/> This code is re-licensed with permission from ReactPhysics3D author.
 * @author Daniel CHAPPUIS
 * @author Edouard DUPIN
 * @copyright 2010-2016, Daniel Chappuis
 * @copyright 2017, Edouard DUPIN
 * @license MPL v2.0 (see license file)
 */
#pragma once

#include <ephysics/constraint/ContactPoint.hpp>
#include <ephysics/configuration.hpp>
#include <ephysics/constraint/Joint.hpp>
#include <ephysics/collision/ContactManifold.hpp>
#include <ephysics/engine/Island.hpp>
#include <ephysics/engine/Impulse.hpp>
#include <etk/Map.hpp>

namespace ephysics {
	/**
	 * @brief This class represents the contact solver that is used to solve rigid bodies contacts.
	 * The constraint solver is based on the "Sequential Impulse" technique described by
	 * Erin Catto in his GDC slides (http://code.google.com/p/box2d/downloads/list).
	 *
	 * A constraint between two bodies is represented by a function C(x) which is equal to zero
	 * when the constraint is satisfied. The condition C(x)=0 describes a valid position and the
	 * condition dC(x)/dt=0 describes a valid velocity. We have dC(x)/dt = Jv + b = 0 where J is
	 * the Jacobian matrix of the constraint, v is a vector that contains the velocity of both
	 * bodies and b is the constraint bias. We are looking for a force F_c that will act on the
	 * bodies to keep the constraint satisfied. Note that from the virtual work principle, we have
	 * F_c = J^t * lambda where J^t is the transpose of the Jacobian matrix and lambda is a
	 * Lagrange multiplier. Therefore, finding the force F_c is equivalent to finding the Lagrange
	 * multiplier lambda.
	 *
	 * An impulse P = F * dt where F is a force and dt is the timestep. We can apply impulses a
	 * body to change its velocity. The idea of the Sequential Impulse technique is to apply
	 * impulses to bodies of each constraints in order to keep the constraint satisfied.
	 *
	 * --- Step 1 ---
	 *
	 * First, we int32_tegrate the applied force F_a acting of each rigid body (like gravity, ...) and
	 * we obtain some new velocities v2' that tends to violate the constraints.
	 *
	 * v2' = v1 + dt * M^-1 * F_a
	 *
	 * where M is a matrix that contains mass and inertia tensor information.
	 *
	 * --- Step 2 ---
	 *
	 * During the second step, we iterate over all the constraints for a certain number of
	 * iterations and for each constraint we compute the impulse to apply to the bodies needed
	 * so that the new velocity of the bodies satisfy Jv + b = 0. From the Newton law, we know that
	 * M * deltaV = P_c where M is the mass of the body, deltaV is the difference of velocity and
	 * P_c is the constraint impulse to apply to the body. Therefore, we have
	 * v2 = v2' + M^-1 * P_c. For each constraint, we can compute the Lagrange multiplier lambda
	 * using : lambda = -m_c (Jv2' + b) where m_c = 1 / (J * M^-1 * J^t). Now that we have the
	 * Lagrange multiplier lambda, we can compute the impulse P_c = J^t * lambda * dt to apply to
	 * the bodies to satisfy the constraint.
	 *
	 * --- Step 3 ---
	 *
	 * In the third step, we int32_tegrate the new position x2 of the bodies using the new velocities
	 * v2 computed in the second step with : x2 = x1 + dt * v2.
	 *
	 * Note that in the following code (as it is also explained in the slides from Erin Catto),
	 * the value lambda is not only the lagrange multiplier but is the multiplication of the
	 * Lagrange multiplier with the timestep dt. Therefore, in the following code, when we use
	 * lambda, we mean (lambda * dt).
	 *
	 * We are using the accumulated impulse technique that is also described in the slides from
	 * Erin Catto.
	 *
	 * We are also using warm starting. The idea is to warm start the solver at the beginning of
	 * each step by applying the last impulstes for the constraints that we already existing at the
	 * previous step. This allows the iterative solver to converge faster towards the solution.
	 *
	 * For contact constraints, we are also using split impulses so that the position correction
	 * that uses Baumgarte stabilization does not change the momentum of the bodies.
	 *
	 * There are two ways to apply the friction constraints. Either the friction constraints are
	 * applied at each contact point or they are applied only at the center of the contact manifold
	 * between two bodies. If we solve the friction constraints at each contact point, we need
	 * two constraints (two tangential friction directions) and if we solve the friction
	 * constraints at the center of the contact manifold, we need two constraints for tangential
	 * friction but also another twist friction constraint to prevent spin of the body around the
	 * contact manifold center.
	 */
	class ContactSolver {
		private:
			/**
			 * Contact solver int32_ternal data structure that to store all the
			 * information relative to a contact point
			 */
			struct ContactPointSolver {
				float penetrationImpulse; //!< Accumulated normal impulse
				float friction1Impulse; //!< Accumulated impulse in the 1st friction direction
				float friction2Impulse; //!< Accumulated impulse in the 2nd friction direction
				float penetrationSplitImpulse; //!< Accumulated split impulse for penetration correction
				vec3 rollingResistanceImpulse; //!< Accumulated rolling resistance impulse
				vec3 normal; //!< Normal vector of the contact
				vec3 frictionVector1; //!< First friction vector in the tangent plane
				vec3 frictionvec2; //!< Second friction vector in the tangent plane
				vec3 oldFrictionVector1; //!< Old first friction vector in the tangent plane
				vec3 oldFrictionvec2; //!< Old second friction vector in the tangent plane
				vec3 r1; //!< Vector from the body 1 center to the contact point
				vec3 r2; //!< Vector from the body 2 center to the contact point
				vec3 r1CrossT1; //!< Cross product of r1 with 1st friction vector
				vec3 r1CrossT2; //!< Cross product of r1 with 2nd friction vector
				vec3 r2CrossT1; //!< Cross product of r2 with 1st friction vector
				vec3 r2CrossT2; //!< Cross product of r2 with 2nd friction vector
				vec3 r1CrossN; //!< Cross product of r1 with the contact normal
				vec3 r2CrossN; //!< Cross product of r2 with the contact normal
				float penetrationDepth; //!< Penetration depth
				float restitutionBias; //!< Velocity restitution bias
				float inversePenetrationMass; //!< Inverse of the matrix K for the penenetration
				float inverseFriction1Mass; //!< Inverse of the matrix K for the 1st friction
				float inverseFriction2Mass; //!< Inverse of the matrix K for the 2nd friction
				bool isRestingContact; //!< True if the contact was existing last time step
				ContactPoint* externalContact; //!< Pointer to the external contact
			};
			/**
			 * @brief Contact solver int32_ternal data structure to store all the information relative to a contact manifold.
			 */
			struct ContactManifoldSolver {
				uint32_t indexBody1; //!< Index of body 1 in the constraint solver
				uint32_t indexBody2; //!< Index of body 2 in the constraint solver
				float massInverseBody1; //!< Inverse of the mass of body 1
				float massInverseBody2; //!< Inverse of the mass of body 2
				etk::Matrix3x3 inverseInertiaTensorBody1; //!< Inverse inertia tensor of body 1
				etk::Matrix3x3 inverseInertiaTensorBody2; //!< Inverse inertia tensor of body 2
				ContactPointSolver contacts[MAX_CONTACT_POINTS_IN_MANIFOLD]; //!< Contact point constraints
				uint32_t nbContacts; //!< Number of contact points
				bool isBody1DynamicType; //!< True if the body 1 is of type dynamic
				bool isBody2DynamicType; //!< True if the body 2 is of type dynamic
				float restitutionFactor; //!< Mix of the restitution factor for two bodies
				float frictionCoefficient; //!< Mix friction coefficient for the two bodies
				float rollingResistanceFactor; //!< Rolling resistance factor between the two bodies
				ContactManifold* externalContactManifold; //!< Pointer to the external contact manifold
				// - Variables used when friction constraints are apply at the center of the manifold-//
				vec3 normal; //!< Average normal vector of the contact manifold
				vec3 frictionPointBody1; //!< Point on body 1 where to apply the friction constraints
				vec3 frictionPointBody2; //!< Point on body 2 where to apply the friction constraints
				vec3 r1Friction; //!< R1 vector for the friction constraints
				vec3 r2Friction; //!< R2 vector for the friction constraints
				vec3 r1CrossT1; //!< Cross product of r1 with 1st friction vector
				vec3 r1CrossT2; //!< Cross product of r1 with 2nd friction vector
				vec3 r2CrossT1; //!< Cross product of r2 with 1st friction vector
				vec3 r2CrossT2; //!< Cross product of r2 with 2nd friction vector
				float inverseFriction1Mass; //!< Matrix K for the first friction constraint
				float inverseFriction2Mass; //!< Matrix K for the second friction constraint
				float inverseTwistFrictionMass; //!< Matrix K for the twist friction constraint
				etk::Matrix3x3 inverseRollingResistance; //!< Matrix K for the rolling resistance constraint
				vec3 frictionVector1; //!< First friction direction at contact manifold center
				vec3 frictionvec2; //!< Second friction direction at contact manifold center
				vec3 oldFrictionVector1; //!< Old 1st friction direction at contact manifold center
				vec3 oldFrictionvec2; //!< Old 2nd friction direction at contact manifold center
				float friction1Impulse; //!< First friction direction impulse at manifold center
				float friction2Impulse; //!< Second friction direction impulse at manifold center
				float frictionTwistImpulse; //!< Twist friction impulse at contact manifold center
				vec3 rollingResistanceImpulse; //!< Rolling resistance impulse
			};
			static const float BETA; //!< Beta value for the penetration depth position correction without split impulses
			static const float BETA_SPLIT_IMPULSE; //!< Beta value for the penetration depth position correction with split impulses
			static const float SLOP; //!< Slop distance (allowed penetration distance between bodies)
			vec3* m_splitLinearVelocities; //!< Split linear velocities for the position contact solver (split impulse)
			vec3* m_splitAngularVelocities; //!< Split angular velocities for the position contact solver (split impulse)
			float m_timeStep; //!< Current time step
			etk::Vector<ContactManifoldSolver> m_contactConstraints; //!< Contact constraints
			vec3* m_linearVelocities; //!< Array of linear velocities
			vec3* m_angularVelocities; //!< Array of angular velocities
			const etk::Map<RigidBody*, uint32_t>& m_mapBodyToConstrainedVelocityIndex; //!< Reference to the map of rigid body to their index in the constrained velocities array
			bool m_isWarmStartingActive; //!< True if the warm starting of the solver is active
			bool m_isSplitImpulseActive; //!< True if the split impulse position correction is active
			bool m_isSolveFrictionAtContactManifoldCenterActive; //!< True if we solve 3 friction constraints at the contact manifold center only instead of 2 friction constraints at each contact point
			/**
			 * @brief Initialize the contact constraints before solving the system
			 */
			void initializeContactConstraints();
			/**
			 * @brief Apply an impulse to the two bodies of a constraint
			 * @param[in] _impulse Impulse to apply
			 * @param[in] _manifold Constraint to apply the impulse
			 */
			void applyImpulse(const Impulse& _impulse, const ContactManifoldSolver& _manifold);
			/**
			 * @brief Apply an impulse to the two bodies of a constraint
			 * @param[in] _impulse Impulse to apply
			 * @param[in] _manifold Constraint to apply the impulse
			 */
			void applySplitImpulse(const Impulse& _impulse, const ContactManifoldSolver& _manifold);
			/**
			 * @brief Compute the collision restitution factor from the restitution factor of each body
			 * @param[in] _body1 First body to compute
			 * @param[in] _body2 Second body to compute
			 * @return Collision restitution factor
			 */
			float computeMixedRestitutionFactor(RigidBody* _body1, RigidBody* _body2) const;
			/**
			 * @brief Compute the mixed friction coefficient from the friction coefficient of each body
			 * @param[in] _body1 First body to compute
			 * @param[in] _body2 Second body to compute
			 * @return Mixed friction coefficient
			 */
			float computeMixedFrictionCoefficient(RigidBody* _body1, RigidBody* _body2) const;
			/**
			 * @brief Compute the mixed rolling resistance factor between two bodies
			 * @param[in] _body1 First body to compute
			 * @param[in] _body2 Second body to compute
			 * @return Mixed rolling resistance
			 */
			float computeMixedRollingResistance(RigidBody* _body1, RigidBody* _body2) const;
			/**
			 * @brief Compute the two unit orthogonal vectors "t1" and "t2" that span the tangential friction
			 *        plane for a contact point. The two vectors have to be such that : t1 x t2 = contactNormal.
			 * @param[in] _deltaVelocity Velocity ratio (with the delta time step)
			 * @param[in,out] _contactPoint Contact point property
			 */
			void computeFrictionVectors(const vec3& _deltaVelocity, ContactPointSolver& _contactPoint) const;
			/**
			 * @brief Compute the two unit orthogonal vectors "t1" and "t2" that span the tangential friction
			 *        plane for a contact manifold. The two vectors have to be such that : t1 x t2 = contactNormal.
			 * @param[in] _deltaVelocity Velocity ratio (with the delta time step)
			 * @param[in,out] _contactPoint Contact point property
			 */
			void computeFrictionVectors(const vec3& _deltaVelocity, ContactManifoldSolver& _contactPoint) const;
			/**
			 * @brief Compute a penetration constraint impulse
			 * @param[in] _deltaLambda Ratio to apply at the calculation.
			 * @param[in,out] _contactPoint Contact point property
			 * @return Impulse of the penetration result
			 */
			const Impulse computePenetrationImpulse(float _deltaLambda, const ContactPointSolver& _contactPoint) const;
			/**
			 * @brief Compute the first friction constraint impulse
			 * @param[in] _deltaLambda Ratio to apply at the calculation.
			 * @param[in] _contactPoint Contact point property
			 * @return Impulse of the friction result
			 */
			const Impulse computeFriction1Impulse(float _deltaLambda, const ContactPointSolver& _contactPoint) const;
			/**
			 * @brief Compute the second friction constraint impulse
			 * @param[in] _deltaLambda Ratio to apply at the calculation.
			 * @param[in] _contactPoint Contact point property
			 * @return Impulse of the friction result
			 */
			const Impulse computeFriction2Impulse(float _deltaLambda, const ContactPointSolver& _contactPoint) const;
		public:
			/**
			 * @brief Constructor
			 * @param[in] _mapBodyToVelocityIndex
			 */
			ContactSolver(const etk::Map<RigidBody*, uint32_t>& _mapBodyToVelocityIndex);
			/**
			 * @brief Virtualize the destructor
			 */
			virtual ~ContactSolver() = default;
			/**
			 * @brief Initialize the constraint solver for a given island
			 * @param[in] _dt Delta step time
			 * @param[in] _island Island list property
			 */
			void initializeForIsland(float _dt, Island* _island);
			/**
			 * @brief Set the split velocities arrays
			 * @param[in] _splitLinearVelocities Split linear velocities Table pointer (not free)
			 * @param[in] _splitAngularVelocities Split angular velocities Table pointer (not free)
			 */
			void setSplitVelocitiesArrays(vec3* _splitLinearVelocities, vec3* _splitAngularVelocities);
			/**
			 * @brief Set the constrained velocities arrays
			 * @param[in] _constrainedLinearVelocities Constrained Linear velocities Table pointer (not free)
			 * @param[in] _constrainedAngularVelocities Constrained angular velocities Table pointer (not free)
			 */
			void setConstrainedVelocitiesArrays(vec3* _constrainedLinearVelocities, vec3* _constrainedAngularVelocities);
			/**
			 * @brief Warm start the solver.
			 * For each constraint, we apply the previous impulse (from the previous step)
			 * at the beginning. With this technique, we will converge faster towards the solution of the linear system
			 */
			void warmStart();
			/**
			 * @brief Store the computed impulses to use them to warm start the solver at the next iteration
			 */
			void storeImpulses();
			/**
			 * @brief Solve the contacts
			 */
			void solve();
			/**
			 * @brief Get the split impulses position correction technique is used for contacts
			 * @return true the split status is Enable
			 * @return true the split status is Disable
			 */
			bool isSplitImpulseActive() const;
			/**
			 * @brief Activate or Deactivate the split impulses for contacts
			 * @param[in] _isActive status to set.
			 */
			void setIsSplitImpulseActive(bool _isActive);
			/**
			 * @brief Activate or deactivate the solving of friction constraints at the center of
			/// the contact manifold instead of solving them at each contact point
			 * @param[in] _isActive Enable or not the center inertie
			 */
			void setIsSolveFrictionAtContactManifoldCenterActive(bool _isActive);
			/**
			 * @brief Clean up the constraint solver
			 */
			void cleanup();
	};

}
