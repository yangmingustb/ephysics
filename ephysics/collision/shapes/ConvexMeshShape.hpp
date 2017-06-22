/** @file
 * @author Daniel Chappuis
 * @copyright 2010-2016 Daniel Chappuis
 * @license BSD 3 clauses (see license file)
 */
#pragma once

#include <ephysics/collision/shapes/ConvexShape.hpp>
#include <ephysics/engine/CollisionWorld.hpp>
#include <ephysics/mathematics/mathematics.hpp>
#include <ephysics/collision/TriangleMesh.hpp>
#include <ephysics/collision/narrowphase/GJK/GJKAlgorithm.hpp>
#include <vector>
#include <set>
#include <map>

namespace ephysics {
	class CollisionWorld;
	/**
	 * @brief It represents a convex mesh shape. In order to create a convex mesh shape, you
	 * need to indicate the local-space position of the mesh vertices. You do it either by
	 * passing a vertices array to the constructor or using the addVertex() method. Make sure
	 * that the set of vertices that you use to create the shape are indeed part of a convex
	 * mesh. The center of mass of the shape will be at the origin of the local-space geometry
	 * that you use to create the mesh. The method used for collision detection with a convex
	 * mesh shape has an O(n) running time with "n" beeing the number of vertices in the mesh.
	 * Therefore, you should try not to use too many vertices. However, it is possible to speed
	 * up the collision detection by using the edges information of your mesh. The running time
	 * of the collision detection that uses the edges is almost O(1) constant time at the cost
	 * of additional memory used to store the vertices. You can indicate edges information
	 * with the addEdge() method. Then, you must use the setIsEdgesInformationUsed(true) method
	 * in order to use the edges information for collision detection.
	 */
	class ConvexMeshShape : public ConvexShape {
		protected :
			std::vector<vec3> m_vertices; //!< Array with the vertices of the mesh
			uint32_t m_numberVertices; //!< Number of vertices in the mesh
			vec3 m_minBounds; //!< Mesh minimum bounds in the three local x, y and z directions
			vec3 m_maxBounds; //!< Mesh maximum bounds in the three local x, y and z directions
			bool m_isEdgesInformationUsed; //!< True if the shape contains the edges of the convex mesh in order to make the collision detection faster
			std::map<uint32_t, std::set<uint32_t> > m_edgesAdjacencyList; //!< Adjacency list representing the edges of the mesh
			/// Private copy-constructor
			ConvexMeshShape(const ConvexMeshShape& shape);
			/// Private assignment operator
			ConvexMeshShape& operator=(const ConvexMeshShape& shape);
			/// Recompute the bounds of the mesh
			void recalculateBounds();
			/// Set the scaling vector of the collision shape
			virtual void setLocalScaling(const vec3& scaling);
			/// Return a local support point in a given direction without the object margin.
			virtual vec3 getLocalSupportPointWithoutMargin(const vec3& direction,
															  void** cachedCollisionData) const;
			/// Return true if a point is inside the collision shape
			virtual bool testPointInside(const vec3& localPoint, ProxyShape* proxyShape) const;
			/// Raycast method with feedback information
			virtual bool raycast(const Ray& ray, RaycastInfo& raycastInfo, ProxyShape* proxyShape) const;
			/// Return the number of bytes used by the collision shape
			virtual size_t getSizeInBytes() const;
		public :
			/// Constructor to initialize with an array of 3D vertices.
			ConvexMeshShape(const float* arrayVertices, uint32_t nbVertices, int32_t stride,
							float margin = OBJECT_MARGIN);
			/// Constructor to initialize with a triangle vertex array
			ConvexMeshShape(TriangleVertexArray* triangleVertexArray, bool isEdgesInformationUsed = true,
							float margin = OBJECT_MARGIN);
			/// Constructor.
			ConvexMeshShape(float margin = OBJECT_MARGIN);
			/// Destructor
			virtual ~ConvexMeshShape();
			/// Return the local bounds of the shape in x, y and z directions
			virtual void getLocalBounds(vec3& min, vec3& max) const;
			/// Return the local inertia tensor of the collision shape.
			virtual void computeLocalInertiaTensor(etk::Matrix3x3& tensor, float mass) const;
			/// Add a vertex int32_to the convex mesh
			void addVertex(const vec3& vertex);
			/// Add an edge int32_to the convex mesh by specifying the two vertex indices of the edge.
			void addEdge(uint32_t v1, uint32_t v2);
			/// Return true if the edges information is used to speed up the collision detection
			bool isEdgesInformationUsed() const;
			/// Set the variable to know if the edges information is used to speed up the
			/// collision detection
			void setIsEdgesInformationUsed(bool isEdgesUsed);
	};
}


