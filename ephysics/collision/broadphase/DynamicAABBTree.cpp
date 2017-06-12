/** @file
 * @author Daniel Chappuis
 * @copyright 2010-2016 Daniel Chappuis
 * @license BSD 3 clauses (see license file)
 */

// Libraries
#include <ephysics/collision/broadphase/DynamicAABBTree.h>
#include <ephysics/collision/broadphase/BroadPhaseAlgorithm.h>
#include <ephysics/memory/Stack.h>
#include <ephysics/engine/Profiler.h>

using namespace reactphysics3d;

// Initialization of static variables
const int32_t TreeNode::NULL_TREE_NODE = -1;

// Constructor
DynamicAABBTree::DynamicAABBTree(float extraAABBGap) : mExtraAABBGap(extraAABBGap) {

	init();
}

// Destructor
DynamicAABBTree::~DynamicAABBTree() {

	// Free the allocated memory for the nodes
	free(mNodes);
}

// Initialize the tree
void DynamicAABBTree::init() {

	m_rootNodeID = TreeNode::NULL_TREE_NODE;
	mNbNodes = 0;
	mNbAllocatedNodes = 8;

	// Allocate memory for the nodes of the tree
	mNodes = (TreeNode*) malloc(mNbAllocatedNodes * sizeof(TreeNode));
	assert(mNodes);
	memset(mNodes, 0, mNbAllocatedNodes * sizeof(TreeNode));

	// Initialize the allocated nodes
	for (int32_t i=0; i<mNbAllocatedNodes - 1; i++) {
		mNodes[i].nextNodeID = i + 1;
		mNodes[i].height = -1;
	}
	mNodes[mNbAllocatedNodes - 1].nextNodeID = TreeNode::NULL_TREE_NODE;
	mNodes[mNbAllocatedNodes - 1].height = -1;
	mFreeNodeID = 0;
}

// Clear all the nodes and reset the tree
void DynamicAABBTree::reset() {

	// Free the allocated memory for the nodes
	free(mNodes);

	// Initialize the tree
	init();
}

// Allocate and return a new node in the tree
int32_t DynamicAABBTree::allocateNode() {

	// If there is no more allocated node to use
	if (mFreeNodeID == TreeNode::NULL_TREE_NODE) {

		assert(mNbNodes == mNbAllocatedNodes);

		// Allocate more nodes in the tree
		mNbAllocatedNodes *= 2;
		TreeNode* oldNodes = mNodes;
		mNodes = (TreeNode*) malloc(mNbAllocatedNodes * sizeof(TreeNode));
		assert(mNodes);
		memcpy(mNodes, oldNodes, mNbNodes * sizeof(TreeNode));
		free(oldNodes);

		// Initialize the allocated nodes
		for (int32_t i=mNbNodes; i<mNbAllocatedNodes - 1; i++) {
			mNodes[i].nextNodeID = i + 1;
			mNodes[i].height = -1;
		}
		mNodes[mNbAllocatedNodes - 1].nextNodeID = TreeNode::NULL_TREE_NODE;
		mNodes[mNbAllocatedNodes - 1].height = -1;
		mFreeNodeID = mNbNodes;
	}

	// Get the next free node
	int32_t freeNodeID = mFreeNodeID;
	mFreeNodeID = mNodes[freeNodeID].nextNodeID;
	mNodes[freeNodeID].parentID = TreeNode::NULL_TREE_NODE;
	mNodes[freeNodeID].height = 0;
	mNbNodes++;

	return freeNodeID;
}

// Release a node
void DynamicAABBTree::releaseNode(int32_t nodeID) {

	assert(mNbNodes > 0);
	assert(nodeID >= 0 && nodeID < mNbAllocatedNodes);
	assert(mNodes[nodeID].height >= 0);
	mNodes[nodeID].nextNodeID = mFreeNodeID;
	mNodes[nodeID].height = -1;
	mFreeNodeID = nodeID;
	mNbNodes--;
}

// Internally add an object int32_to the tree
int32_t DynamicAABBTree::addObjectInternal(const AABB& aabb) {

	// Get the next available node (or allocate new ones if necessary)
	int32_t nodeID = allocateNode();

	// Create the fat aabb to use in the tree
	const Vector3 gap(mExtraAABBGap, mExtraAABBGap, mExtraAABBGap);
	mNodes[nodeID].aabb.setMin(aabb.getMin() - gap);
	mNodes[nodeID].aabb.setMax(aabb.getMax() + gap);

	// Set the height of the node in the tree
	mNodes[nodeID].height = 0;

	// Insert the new leaf node in the tree
	insertLeafNode(nodeID);
	assert(mNodes[nodeID].isLeaf());

	assert(nodeID >= 0);

	// Return the Id of the node
	return nodeID;
}

// Remove an object from the tree
void DynamicAABBTree::removeObject(int32_t nodeID) {

	assert(nodeID >= 0 && nodeID < mNbAllocatedNodes);
	assert(mNodes[nodeID].isLeaf());

	// Remove the node from the tree
	removeLeafNode(nodeID);
	releaseNode(nodeID);
}

// Update the dynamic tree after an object has moved.
/// If the new AABB of the object that has moved is still inside its fat AABB, then
/// nothing is done. Otherwise, the corresponding node is removed and reinserted int32_to the tree.
/// The method returns true if the object has been reinserted int32_to the tree. The "displacement"
/// argument is the linear velocity of the AABB multiplied by the elapsed time between two
/// frames. If the "forceReinsert" parameter is true, we force a removal and reinsertion of the node
/// (this can be useful if the shape AABB has become much smaller than the previous one for instance).
bool DynamicAABBTree::updateObject(int32_t nodeID, const AABB& newAABB, const Vector3& displacement, bool forceReinsert) {

	PROFILE("DynamicAABBTree::updateObject()");

	assert(nodeID >= 0 && nodeID < mNbAllocatedNodes);
	assert(mNodes[nodeID].isLeaf());
	assert(mNodes[nodeID].height >= 0);

	// If the new AABB is still inside the fat AABB of the node
	if (!forceReinsert && mNodes[nodeID].aabb.contains(newAABB)) {
		return false;
	}

	// If the new AABB is outside the fat AABB, we remove the corresponding node
	removeLeafNode(nodeID);

	// Compute the fat AABB by inflating the AABB with a constant gap
	mNodes[nodeID].aabb = newAABB;
	const Vector3 gap(mExtraAABBGap, mExtraAABBGap, mExtraAABBGap);
	mNodes[nodeID].aabb.m_minCoordinates -= gap;
	mNodes[nodeID].aabb.m_maxCoordinates += gap;

	// Inflate the fat AABB in direction of the linear motion of the AABB
	if (displacement.x < float(0.0)) {
	  mNodes[nodeID].aabb.m_minCoordinates.x += DYNAMIC_TREE_AABB_LIN_GAP_MULTIPLIER *displacement.x;
	}
	else {
	  mNodes[nodeID].aabb.m_maxCoordinates.x += DYNAMIC_TREE_AABB_LIN_GAP_MULTIPLIER *displacement.x;
	}
	if (displacement.y < float(0.0)) {
	  mNodes[nodeID].aabb.m_minCoordinates.y += DYNAMIC_TREE_AABB_LIN_GAP_MULTIPLIER *displacement.y;
	}
	else {
	  mNodes[nodeID].aabb.m_maxCoordinates.y += DYNAMIC_TREE_AABB_LIN_GAP_MULTIPLIER *displacement.y;
	}
	if (displacement.z < float(0.0)) {
	  mNodes[nodeID].aabb.m_minCoordinates.z += DYNAMIC_TREE_AABB_LIN_GAP_MULTIPLIER *displacement.z;
	}
	else {
	  mNodes[nodeID].aabb.m_maxCoordinates.z += DYNAMIC_TREE_AABB_LIN_GAP_MULTIPLIER *displacement.z;
	}

	assert(mNodes[nodeID].aabb.contains(newAABB));

	// Reinsert the node int32_to the tree
	insertLeafNode(nodeID);

	return true;
}

// Insert a leaf node in the tree. The process of inserting a new leaf node
// in the dynamic tree is described in the book "Introduction to Game Physics
// with Box2D" by Ian Parberry.
void DynamicAABBTree::insertLeafNode(int32_t nodeID) {

	// If the tree is empty
	if (m_rootNodeID == TreeNode::NULL_TREE_NODE) {
		m_rootNodeID = nodeID;
		mNodes[m_rootNodeID].parentID = TreeNode::NULL_TREE_NODE;
		return;
	}

	assert(m_rootNodeID != TreeNode::NULL_TREE_NODE);

	// Find the best sibling node for the new node
	AABB newNodeAABB = mNodes[nodeID].aabb;
	int32_t currentNodeID = m_rootNodeID;
	while (!mNodes[currentNodeID].isLeaf()) {

		int32_t leftChild = mNodes[currentNodeID].children[0];
		int32_t rightChild = mNodes[currentNodeID].children[1];

		// Compute the merged AABB
		float volumeAABB = mNodes[currentNodeID].aabb.getVolume();
		AABB mergedAABBs;
		mergedAABBs.mergeTwoAABBs(mNodes[currentNodeID].aabb, newNodeAABB);
		float mergedVolume = mergedAABBs.getVolume();

		// Compute the cost of making the current node the sibbling of the new node
		float costS = float(2.0) * mergedVolume;

		// Compute the minimum cost of pushing the new node further down the tree (inheritance cost)
		float costI = float(2.0) * (mergedVolume - volumeAABB);

		// Compute the cost of descending int32_to the left child
		float costLeft;
		AABB currentAndLeftAABB;
		currentAndLeftAABB.mergeTwoAABBs(newNodeAABB, mNodes[leftChild].aabb);
		if (mNodes[leftChild].isLeaf()) {   // If the left child is a leaf
			costLeft = currentAndLeftAABB.getVolume() + costI;
		}
		else {
			float leftChildVolume = mNodes[leftChild].aabb.getVolume();
			costLeft = costI + currentAndLeftAABB.getVolume() - leftChildVolume;
		}

		// Compute the cost of descending int32_to the right child
		float costRight;
		AABB currentAndRightAABB;
		currentAndRightAABB.mergeTwoAABBs(newNodeAABB, mNodes[rightChild].aabb);
		if (mNodes[rightChild].isLeaf()) {   // If the right child is a leaf
			costRight = currentAndRightAABB.getVolume() + costI;
		}
		else {
			float rightChildVolume = mNodes[rightChild].aabb.getVolume();
			costRight = costI + currentAndRightAABB.getVolume() - rightChildVolume;
		}

		// If the cost of making the current node a sibbling of the new node is smaller than
		// the cost of going down int32_to the left or right child
		if (costS < costLeft && costS < costRight) break;

		// It is cheaper to go down int32_to a child of the current node, choose the best child
		if (costLeft < costRight) {
			currentNodeID = leftChild;
		}
		else {
			currentNodeID = rightChild;
		}
	}

	int32_t siblingNode = currentNodeID;

	// Create a new parent for the new node and the sibling node
	int32_t oldParentNode = mNodes[siblingNode].parentID;
	int32_t newParentNode = allocateNode();
	mNodes[newParentNode].parentID = oldParentNode;
	mNodes[newParentNode].aabb.mergeTwoAABBs(mNodes[siblingNode].aabb, newNodeAABB);
	mNodes[newParentNode].height = mNodes[siblingNode].height + 1;
	assert(mNodes[newParentNode].height > 0);

	// If the sibling node was not the root node
	if (oldParentNode != TreeNode::NULL_TREE_NODE) {
		assert(!mNodes[oldParentNode].isLeaf());
		if (mNodes[oldParentNode].children[0] == siblingNode) {
			mNodes[oldParentNode].children[0] = newParentNode;
		}
		else {
			mNodes[oldParentNode].children[1] = newParentNode;
		}
		mNodes[newParentNode].children[0] = siblingNode;
		mNodes[newParentNode].children[1] = nodeID;
		mNodes[siblingNode].parentID = newParentNode;
		mNodes[nodeID].parentID = newParentNode;
	}
	else {  // If the sibling node was the root node
		mNodes[newParentNode].children[0] = siblingNode;
		mNodes[newParentNode].children[1] = nodeID;
		mNodes[siblingNode].parentID = newParentNode;
		mNodes[nodeID].parentID = newParentNode;
		m_rootNodeID = newParentNode;
	}

	// Move up in the tree to change the AABBs that have changed
	currentNodeID = mNodes[nodeID].parentID;
	assert(!mNodes[currentNodeID].isLeaf());
	while (currentNodeID != TreeNode::NULL_TREE_NODE) {

		// Balance the sub-tree of the current node if it is not balanced
		currentNodeID = balanceSubTreeAtNode(currentNodeID);
		assert(mNodes[nodeID].isLeaf());

		assert(!mNodes[currentNodeID].isLeaf());
		int32_t leftChild = mNodes[currentNodeID].children[0];
		int32_t rightChild = mNodes[currentNodeID].children[1];
		assert(leftChild != TreeNode::NULL_TREE_NODE);
		assert(rightChild != TreeNode::NULL_TREE_NODE);

		// Recompute the height of the node in the tree
		mNodes[currentNodeID].height = std::max(mNodes[leftChild].height,
												mNodes[rightChild].height) + 1;
		assert(mNodes[currentNodeID].height > 0);

		// Recompute the AABB of the node
		mNodes[currentNodeID].aabb.mergeTwoAABBs(mNodes[leftChild].aabb, mNodes[rightChild].aabb);

		currentNodeID = mNodes[currentNodeID].parentID;
	}

	assert(mNodes[nodeID].isLeaf());
}

// Remove a leaf node from the tree
void DynamicAABBTree::removeLeafNode(int32_t nodeID) {

	assert(nodeID >= 0 && nodeID < mNbAllocatedNodes);
	assert(mNodes[nodeID].isLeaf());

	// If we are removing the root node (root node is a leaf in this case)
	if (m_rootNodeID == nodeID) {
		m_rootNodeID = TreeNode::NULL_TREE_NODE;
		return;
	}

	int32_t parentNodeID = mNodes[nodeID].parentID;
	int32_t grandParentNodeID = mNodes[parentNodeID].parentID;
	int32_t siblingNodeID;
	if (mNodes[parentNodeID].children[0] == nodeID) {
		siblingNodeID = mNodes[parentNodeID].children[1];
	}
	else {
		siblingNodeID = mNodes[parentNodeID].children[0];
	}

	// If the parent of the node to remove is not the root node
	if (grandParentNodeID != TreeNode::NULL_TREE_NODE) {

		// Destroy the parent node
		if (mNodes[grandParentNodeID].children[0] == parentNodeID) {
			mNodes[grandParentNodeID].children[0] = siblingNodeID;
		}
		else {
			assert(mNodes[grandParentNodeID].children[1] == parentNodeID);
			mNodes[grandParentNodeID].children[1] = siblingNodeID;
		}
		mNodes[siblingNodeID].parentID = grandParentNodeID;
		releaseNode(parentNodeID);

		// Now, we need to recompute the AABBs of the node on the path back to the root
		// and make sure that the tree is still balanced
		int32_t currentNodeID = grandParentNodeID;
		while(currentNodeID != TreeNode::NULL_TREE_NODE) {

			// Balance the current sub-tree if necessary
			currentNodeID = balanceSubTreeAtNode(currentNodeID);

			assert(!mNodes[currentNodeID].isLeaf());

			// Get the two children of the current node
			int32_t leftChildID = mNodes[currentNodeID].children[0];
			int32_t rightChildID = mNodes[currentNodeID].children[1];

			// Recompute the AABB and the height of the current node
			mNodes[currentNodeID].aabb.mergeTwoAABBs(mNodes[leftChildID].aabb,
													 mNodes[rightChildID].aabb);
			mNodes[currentNodeID].height = std::max(mNodes[leftChildID].height,
													mNodes[rightChildID].height) + 1;
			assert(mNodes[currentNodeID].height > 0);

			currentNodeID = mNodes[currentNodeID].parentID;
		}
	}
	else { // If the parent of the node to remove is the root node

		// The sibling node becomes the new root node
		m_rootNodeID = siblingNodeID;
		mNodes[siblingNodeID].parentID = TreeNode::NULL_TREE_NODE;
		releaseNode(parentNodeID);
	}
}

// Balance the sub-tree of a given node using left or right rotations.
/// The rotation schemes are described in the book "Introduction to Game Physics
/// with Box2D" by Ian Parberry. This method returns the new root node ID.
int32_t DynamicAABBTree::balanceSubTreeAtNode(int32_t nodeID) {

	assert(nodeID != TreeNode::NULL_TREE_NODE);

	TreeNode* nodeA = mNodes + nodeID;

	// If the node is a leaf or the height of A's sub-tree is less than 2
	if (nodeA->isLeaf() || nodeA->height < 2) {

		// Do not perform any rotation
		return nodeID;
	}

	// Get the two children nodes
	int32_t nodeBID = nodeA->children[0];
	int32_t nodeCID = nodeA->children[1];
	assert(nodeBID >= 0 && nodeBID < mNbAllocatedNodes);
	assert(nodeCID >= 0 && nodeCID < mNbAllocatedNodes);
	TreeNode* nodeB = mNodes + nodeBID;
	TreeNode* nodeC = mNodes + nodeCID;

	// Compute the factor of the left and right sub-trees
	int32_t balanceFactor = nodeC->height - nodeB->height;

	// If the right node C is 2 higher than left node B
	if (balanceFactor > 1) {

		assert(!nodeC->isLeaf());

		int32_t nodeFID = nodeC->children[0];
		int32_t nodeGID = nodeC->children[1];
		assert(nodeFID >= 0 && nodeFID < mNbAllocatedNodes);
		assert(nodeGID >= 0 && nodeGID < mNbAllocatedNodes);
		TreeNode* nodeF = mNodes + nodeFID;
		TreeNode* nodeG = mNodes + nodeGID;

		nodeC->children[0] = nodeID;
		nodeC->parentID = nodeA->parentID;
		nodeA->parentID = nodeCID;

		if (nodeC->parentID != TreeNode::NULL_TREE_NODE) {

			if (mNodes[nodeC->parentID].children[0] == nodeID) {
				mNodes[nodeC->parentID].children[0] = nodeCID;
			}
			else {
				assert(mNodes[nodeC->parentID].children[1] == nodeID);
				mNodes[nodeC->parentID].children[1] = nodeCID;
			}
		}
		else {
			m_rootNodeID = nodeCID;
		}

		assert(!nodeC->isLeaf());
		assert(!nodeA->isLeaf());

		// If the right node C was higher than left node B because of the F node
		if (nodeF->height > nodeG->height) {

			nodeC->children[1] = nodeFID;
			nodeA->children[1] = nodeGID;
			nodeG->parentID = nodeID;

			// Recompute the AABB of node A and C
			nodeA->aabb.mergeTwoAABBs(nodeB->aabb, nodeG->aabb);
			nodeC->aabb.mergeTwoAABBs(nodeA->aabb, nodeF->aabb);

			// Recompute the height of node A and C
			nodeA->height = std::max(nodeB->height, nodeG->height) + 1;
			nodeC->height = std::max(nodeA->height, nodeF->height) + 1;
			assert(nodeA->height > 0);
			assert(nodeC->height > 0);
		}
		else {  // If the right node C was higher than left node B because of node G
			nodeC->children[1] = nodeGID;
			nodeA->children[1] = nodeFID;
			nodeF->parentID = nodeID;

			// Recompute the AABB of node A and C
			nodeA->aabb.mergeTwoAABBs(nodeB->aabb, nodeF->aabb);
			nodeC->aabb.mergeTwoAABBs(nodeA->aabb, nodeG->aabb);

			// Recompute the height of node A and C
			nodeA->height = std::max(nodeB->height, nodeF->height) + 1;
			nodeC->height = std::max(nodeA->height, nodeG->height) + 1;
			assert(nodeA->height > 0);
			assert(nodeC->height > 0);
		}

		// Return the new root of the sub-tree
		return nodeCID;
	}

	// If the left node B is 2 higher than right node C
	if (balanceFactor < -1) {

		assert(!nodeB->isLeaf());

		int32_t nodeFID = nodeB->children[0];
		int32_t nodeGID = nodeB->children[1];
		assert(nodeFID >= 0 && nodeFID < mNbAllocatedNodes);
		assert(nodeGID >= 0 && nodeGID < mNbAllocatedNodes);
		TreeNode* nodeF = mNodes + nodeFID;
		TreeNode* nodeG = mNodes + nodeGID;

		nodeB->children[0] = nodeID;
		nodeB->parentID = nodeA->parentID;
		nodeA->parentID = nodeBID;

		if (nodeB->parentID != TreeNode::NULL_TREE_NODE) {

			if (mNodes[nodeB->parentID].children[0] == nodeID) {
				mNodes[nodeB->parentID].children[0] = nodeBID;
			}
			else {
				assert(mNodes[nodeB->parentID].children[1] == nodeID);
				mNodes[nodeB->parentID].children[1] = nodeBID;
			}
		}
		else {
			m_rootNodeID = nodeBID;
		}

		assert(!nodeB->isLeaf());
		assert(!nodeA->isLeaf());

		// If the left node B was higher than right node C because of the F node
		if (nodeF->height > nodeG->height) {

			nodeB->children[1] = nodeFID;
			nodeA->children[0] = nodeGID;
			nodeG->parentID = nodeID;

			// Recompute the AABB of node A and B
			nodeA->aabb.mergeTwoAABBs(nodeC->aabb, nodeG->aabb);
			nodeB->aabb.mergeTwoAABBs(nodeA->aabb, nodeF->aabb);

			// Recompute the height of node A and B
			nodeA->height = std::max(nodeC->height, nodeG->height) + 1;
			nodeB->height = std::max(nodeA->height, nodeF->height) + 1;
			assert(nodeA->height > 0);
			assert(nodeB->height > 0);
		}
		else {  // If the left node B was higher than right node C because of node G
			nodeB->children[1] = nodeGID;
			nodeA->children[0] = nodeFID;
			nodeF->parentID = nodeID;

			// Recompute the AABB of node A and B
			nodeA->aabb.mergeTwoAABBs(nodeC->aabb, nodeF->aabb);
			nodeB->aabb.mergeTwoAABBs(nodeA->aabb, nodeG->aabb);

			// Recompute the height of node A and B
			nodeA->height = std::max(nodeC->height, nodeF->height) + 1;
			nodeB->height = std::max(nodeA->height, nodeG->height) + 1;
			assert(nodeA->height > 0);
			assert(nodeB->height > 0);
		}

		// Return the new root of the sub-tree
		return nodeBID;
	}

	// If the sub-tree is balanced, return the current root node
	return nodeID;
}

/// Report all shapes overlapping with the AABB given in parameter.
void DynamicAABBTree::reportAllShapesOverlappingWithAABB(const AABB& aabb,
														 DynamicAABBTreeOverlapCallback& callback) const {

	// Create a stack with the nodes to visit
	Stack<int32_t, 64> stack;
	stack.push(m_rootNodeID);

	// While there are still nodes to visit
	while(stack.getNbElements() > 0) {

		// Get the next node ID to visit
		int32_t nodeIDToVisit = stack.pop();

		// Skip it if it is a null node
		if (nodeIDToVisit == TreeNode::NULL_TREE_NODE) continue;

		// Get the corresponding node
		const TreeNode* nodeToVisit = mNodes + nodeIDToVisit;

		// If the AABB in parameter overlaps with the AABB of the node to visit
		if (aabb.testCollision(nodeToVisit->aabb)) {

			// If the node is a leaf
			if (nodeToVisit->isLeaf()) {

				// Notify the broad-phase about a new potential overlapping pair
				callback.notifyOverlappingNode(nodeIDToVisit);
			}
			else {  // If the node is not a leaf

				// We need to visit its children
				stack.push(nodeToVisit->children[0]);
				stack.push(nodeToVisit->children[1]);
			}
		}
	}
}

// Ray casting method
void DynamicAABBTree::raycast(const Ray& ray, DynamicAABBTreeRaycastCallback &callback) const {

	PROFILE("DynamicAABBTree::raycast()");

	float maxFraction = ray.maxFraction;

	Stack<int32_t, 128> stack;
	stack.push(m_rootNodeID);

	// Walk through the tree from the root looking for proxy shapes
	// that overlap with the ray AABB
	while (stack.getNbElements() > 0) {

		// Get the next node in the stack
		int32_t nodeID = stack.pop();

		// If it is a null node, skip it
		if (nodeID == TreeNode::NULL_TREE_NODE) continue;

		// Get the corresponding node
		const TreeNode* node = mNodes + nodeID;

		Ray rayTemp(ray.point1, ray.point2, maxFraction);

		// Test if the ray int32_tersects with the current node AABB
		if (!node->aabb.testRayIntersect(rayTemp)) continue;

		// If the node is a leaf of the tree
		if (node->isLeaf()) {

			// Call the callback that will raycast again the broad-phase shape
			float hitFraction = callback.raycastBroadPhaseShape(nodeID, rayTemp);

			// If the user returned a hitFraction of zero, it means that
			// the raycasting should stop here
			if (hitFraction == float(0.0)) {
				return;
			}

			// If the user returned a positive fraction
			if (hitFraction > float(0.0)) {

				// We update the maxFraction value and the ray
				// AABB using the new maximum fraction
				if (hitFraction < maxFraction) {
					maxFraction = hitFraction;
				}
			}

			// If the user returned a negative fraction, we continue
			// the raycasting as if the proxy shape did not exist
		}
		else {  // If the node has children

			// Push its children in the stack of nodes to explore
			stack.push(node->children[0]);
			stack.push(node->children[1]);
		}
	}
}

#ifndef NDEBUG

// Check if the tree structure is valid (for debugging purpose)
void DynamicAABBTree::check() const {

	// Recursively check each node
	checkNode(m_rootNodeID);

	int32_t nbFreeNodes = 0;
	int32_t freeNodeID = mFreeNodeID;

	// Check the free nodes
	while(freeNodeID != TreeNode::NULL_TREE_NODE) {
		assert(0 <= freeNodeID && freeNodeID < mNbAllocatedNodes);
		freeNodeID = mNodes[freeNodeID].nextNodeID;
		nbFreeNodes++;
	}

	assert(mNbNodes + nbFreeNodes == mNbAllocatedNodes);
}

// Check if the node structure is valid (for debugging purpose)
void DynamicAABBTree::checkNode(int32_t nodeID) const {

	if (nodeID == TreeNode::NULL_TREE_NODE) return;

	// If it is the root
	if (nodeID == m_rootNodeID) {
		assert(mNodes[nodeID].parentID == TreeNode::NULL_TREE_NODE);
	}

	// Get the children nodes
	TreeNode* pNode = mNodes + nodeID;
	assert(!pNode->isLeaf());
	int32_t leftChild = pNode->children[0];
	int32_t rightChild = pNode->children[1];

	assert(pNode->height >= 0);
	assert(pNode->aabb.getVolume() > 0);

	// If the current node is a leaf
	if (pNode->isLeaf()) {

		// Check that there are no children
		assert(leftChild == TreeNode::NULL_TREE_NODE);
		assert(rightChild == TreeNode::NULL_TREE_NODE);
		assert(pNode->height == 0);
	}
	else {

		// Check that the children node IDs are valid
		assert(0 <= leftChild && leftChild < mNbAllocatedNodes);
		assert(0 <= rightChild && rightChild < mNbAllocatedNodes);

		// Check that the children nodes have the correct parent node
		assert(mNodes[leftChild].parentID == nodeID);
		assert(mNodes[rightChild].parentID == nodeID);

		// Check the height of node
		int32_t height = 1 + std::max(mNodes[leftChild].height, mNodes[rightChild].height);
		assert(mNodes[nodeID].height == height);

		// Check the AABB of the node
		AABB aabb;
		aabb.mergeTwoAABBs(mNodes[leftChild].aabb, mNodes[rightChild].aabb);
		assert(aabb.getMin() == mNodes[nodeID].aabb.getMin());
		assert(aabb.getMax() == mNodes[nodeID].aabb.getMax());

		// Recursively check the children nodes
		checkNode(leftChild);
		checkNode(rightChild);
	}
}

// Compute the height of the tree
int32_t DynamicAABBTree::computeHeight() {
   return computeHeight(m_rootNodeID);
}

// Compute the height of a given node in the tree
int32_t DynamicAABBTree::computeHeight(int32_t nodeID) {
	assert(nodeID >= 0 && nodeID < mNbAllocatedNodes);
	TreeNode* node = mNodes + nodeID;

	// If the node is a leaf, its height is zero
	if (node->isLeaf()) {
		return 0;
	}

	// Compute the height of the left and right sub-tree
	int32_t leftHeight = computeHeight(node->children[0]);
	int32_t rightHeight = computeHeight(node->children[1]);

	// Return the height of the node
	return 1 + std::max(leftHeight, rightHeight);
}

#endif
