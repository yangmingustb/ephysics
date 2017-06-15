/** @file
 * @author Daniel Chappuis
 * @copyright 2010-2016 Daniel Chappuis
 * @license BSD 3 clauses (see license file)
 */
#pragma once

// Libraries
#include <ephysics/configuration.h>
#include <ephysics/mathematics/mathematics.h>
#include <ephysics/constraint/Joint.h>
#include <ephysics/engine/Island.h>
#include <map>
#include <set>

namespace reactphysics3d {

// Structure ConstraintSolverData
/**
 * This structure contains data from the constraint solver that are used to solve
 * each joint constraint.
 */
struct ConstraintSolverData {

	public :

		/// Current time step of the simulation
		float timeStep;

		/// Array with the bodies linear velocities
		vec3* linearVelocities;

		/// Array with the bodies angular velocities
		vec3* angularVelocities;

		/// Reference to the bodies positions
		vec3* positions;

		/// Reference to the bodies orientations
		etk::Quaternion* orientations;

		/// Reference to the map that associates rigid body to their index
		/// in the constrained velocities array
		const std::map<RigidBody*, uint32_t>& mapBodyToConstrainedVelocityIndex;

		/// True if warm starting of the solver is active
		bool isWarmStartingActive;

		/// Constructor
		ConstraintSolverData(const std::map<RigidBody*, uint32_t>& refMapBodyToConstrainedVelocityIndex)
						   :linearVelocities(NULL), angularVelocities(NULL),
							positions(NULL), orientations(NULL),
							mapBodyToConstrainedVelocityIndex(refMapBodyToConstrainedVelocityIndex){

		}

};

// Class ConstraintSolver
/**
 * This class represents the constraint solver that is used to solve constraints between
 * the rigid bodies. The constraint solver is based on the "Sequential Impulse" technique
 * described by Erin Catto in his GDC slides (http://code.google.com/p/box2d/downloads/list).
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
class ConstraintSolver {

	private :

		// -------------------- Attributes -------------------- //

		/// Reference to the map that associates rigid body to their index in
		/// the constrained velocities array
		const std::map<RigidBody*, uint32_t>& m_mapBodyToConstrainedVelocityIndex;

		/// Current time step
		float m_timeStep;

		/// True if the warm starting of the solver is active
		bool mIsWarmStartingActive;

		/// Constraint solver data used to initialize and solve the constraints
		ConstraintSolverData m_constraintSolverData;

	public :

		// -------------------- Methods -------------------- //

		/// Constructor
		ConstraintSolver(const std::map<RigidBody*, uint32_t>& mapBodyToVelocityIndex);

		/// Destructor
		~ConstraintSolver();

		/// Initialize the constraint solver for a given island
		void initializeForIsland(float dt, Island* island);

		/// Solve the constraints
		void solveVelocityConstraints(Island* island);

		/// Solve the position constraints
		void solvePositionConstraints(Island* island);

		/// Return true if the Non-Linear-Gauss-Seidel position correction technique is active
		bool getIsNonLinearGaussSeidelPositionCorrectionActive() const;

		/// Enable/Disable the Non-Linear-Gauss-Seidel position correction technique.
		void setIsNonLinearGaussSeidelPositionCorrectionActive(bool isActive);

		/// Set the constrained velocities arrays
		void setConstrainedVelocitiesArrays(vec3* constrainedLinearVelocities,
											vec3* constrainedAngularVelocities);

		/// Set the constrained positions/orientations arrays
		void setConstrainedPositionsArrays(vec3* constrainedPositions,
										   etk::Quaternion* constrainedOrientations);
};

// Set the constrained velocities arrays
inline void ConstraintSolver::setConstrainedVelocitiesArrays(vec3* constrainedLinearVelocities,
															vec3* constrainedAngularVelocities) {
	assert(constrainedLinearVelocities != NULL);
	assert(constrainedAngularVelocities != NULL);
	m_constraintSolverData.linearVelocities = constrainedLinearVelocities;
	m_constraintSolverData.angularVelocities = constrainedAngularVelocities;
}

// Set the constrained positions/orientations arrays
inline void ConstraintSolver::setConstrainedPositionsArrays(vec3* constrainedPositions,
														   etk::Quaternion* constrainedOrientations) {
	assert(constrainedPositions != NULL);
	assert(constrainedOrientations != NULL);
	m_constraintSolverData.positions = constrainedPositions;
	m_constraintSolverData.orientations = constrainedOrientations;
}

}
