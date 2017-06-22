/** @file
 * @author Daniel Chappuis
 * @copyright 2010-2016 Daniel Chappuis
 * @license BSD 3 clauses (see license file)
 */
#pragma once

#include <ephysics/configuration.hpp>
#include <ephysics/collision/shapes/AABB.hpp>
#include <ephysics/body/CollisionBody.hpp>

namespace ephysics {
	class BroadPhaseAlgorithm;
	class BroadPhaseRaycastTestCallback;
	class DynamicAABBTreeOverlapCallback;
	struct RaycastTest;
	/**
	 * @brief It represents a node of the dynamic AABB tree.
	 */
	struct TreeNode {
		const static int32_t NULL_TREE_NODE; //!< Null tree node constant
		/**
		 * @brief A node is either in the tree (has a parent) or in the free nodes list (has a next node)
		 */
		union {
			int32_t parentID; //!< Parent node ID
			int32_t nextNodeID; //!< Next allocated node ID
		};
		/**
		 * @brief A node is either a leaf (has data) or is an int32_ternal node (has children)
		 */
		union {
			int32_t children[2]; //!< Left and right child of the node (children[0] = left child)
			//! Two pieces of data stored at that node (in case the node is a leaf)
			union {
				int32_t dataInt[2];
				void* dataPointer;
			};
		};
		int16_t height; //!< Height of the node in the tree
		AABB aabb; //!< Fat axis aligned bounding box (AABB) corresponding to the node
		/// Return true if the node is a leaf of the tree
		bool isLeaf() const;
	};
	
	/**
	 * @brief Overlapping callback method that has to be used as parameter of the
	 * reportAllShapesOverlappingWithNode() method.
	 */
	class DynamicAABBTreeOverlapCallback {
		public :
			virtual ~DynamicAABBTreeOverlapCallback() = default;
			// Called when a overlapping node has been found during the call to
			// DynamicAABBTree:reportAllShapesOverlappingWithAABB()
			virtual void notifyOverlappingNode(int32_t nodeId)=0;
	};
	
	/**
	 * @brief Raycast callback in the Dynamic AABB Tree called when the AABB of a leaf
	 * node is hit by the ray.
	 */
	class DynamicAABBTreeRaycastCallback {
		public:
			virtual ~DynamicAABBTreeRaycastCallback() = default;
			// Called when the AABB of a leaf node is hit by a ray
			virtual float raycastBroadPhaseShape(int32_t nodeId, const Ray& ray)=0;
	};
	
	/**
	 * @brief It implements a dynamic AABB tree that is used for broad-phase
	 * collision detection. This data structure is inspired by Nathanael Presson's
	 * dynamic tree implementation in BulletPhysics. The following implementation is
	 * based on the one from Erin Catto in Box2D as described in the book
	 * "Introduction to Game Physics with Box2D" by Ian Parberry.
	 */
	class DynamicAABBTree {
		private:
			TreeNode* m_nodes; //!< Pointer to the memory location of the nodes of the tree
			int32_t m_rootNodeID; //!< ID of the root node of the tree
			int32_t m_freeNodeID; //!< ID of the first node of the list of free (allocated) nodes in the tree that we can use
			int32_t m_numberAllocatedNodes; //!< Number of allocated nodes in the tree
			int32_t m_numberNodes; //!< Number of nodes in the tree
			float m_extraAABBGap; //!< Extra AABB Gap used to allow the collision shape to move a little bit without triggering a large modification of the tree which can be costly
			/// Allocate and return a node to use in the tree
			int32_t allocateNode();
			/// Release a node
			void releaseNode(int32_t _nodeID);
			/// Insert a leaf node in the tree
			void insertLeafNode(int32_t _nodeID);
			/// Remove a leaf node from the tree
			void removeLeafNode(int32_t _nodeID);
			/// Balance the sub-tree of a given node using left or right rotations.
			int32_t balanceSubTreeAtNode(int32_t _nodeID);
			/// Compute the height of a given node in the tree
			int32_t computeHeight(int32_t _nodeID);
			/// Internally add an object int32_to the tree
			int32_t addObjectInternal(const AABB& _aabb);
			/// Initialize the tree
			void init();
			#ifndef NDEBUG
				/// Check if the tree structure is valid (for debugging purpose)
				void check() const;
				/// Check if the node structure is valid (for debugging purpose)
				void checkNode(int32_t _nodeID) const;
			#endif
		public:
			/// Constructor
			DynamicAABBTree(float _extraAABBGap = 0.0f);
			/// Destructor
			virtual ~DynamicAABBTree();
			/// Add an object int32_to the tree (where node data are two int32_tegers)
			int32_t addObject(const AABB& _aabb, int32_t _data1, int32_t _data2);
			/// Add an object int32_to the tree (where node data is a pointer)
			int32_t addObject(const AABB& _aabb, void* _data);
			/// Remove an object from the tree
			void removeObject(int32_t _nodeID);
			/// Update the dynamic tree after an object has moved.
			bool updateObject(int32_t _nodeID, const AABB& _newAABB, const vec3& _displacement, bool _forceReinsert = false);
			/// Return the fat AABB corresponding to a given node ID
			const AABB& getFatAABB(int32_t _nodeID) const;
			/// Return the pointer to the data array of a given leaf node of the tree
			int32_t* getNodeDataInt(int32_t _nodeID) const;
			/// Return the data pointer of a given leaf node of the tree
			void* getNodeDataPointer(int32_t _nodeID) const;
			/// Report all shapes overlapping with the AABB given in parameter.
			void reportAllShapesOverlappingWithAABB(const AABB& _aabb,
													DynamicAABBTreeOverlapCallback& _callback) const;
			/// Ray casting method
			void raycast(const Ray& _ray, DynamicAABBTreeRaycastCallback& _callback) const;
			/// Compute the height of the tree
			int32_t computeHeight();
			/// Return the root AABB of the tree
			AABB getRootAABB() const;
			/// Clear all the nodes and reset the tree
			void reset();
	};


}
