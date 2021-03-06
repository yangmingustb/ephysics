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

// Libraries
#include <ephysics/Dumbbell.hpp>

openglframework::VertexBufferObject Dumbbell::mVBOVertices(GL_ARRAY_BUFFER);
openglframework::VertexBufferObject Dumbbell::mVBONormals(GL_ARRAY_BUFFER);
openglframework::VertexBufferObject Dumbbell::mVBOTextureCoords(GL_ARRAY_BUFFER);
openglframework::VertexBufferObject Dumbbell::mVBOIndices(GL_ELEMENT_ARRAY_BUFFER);
openglframework::VertexArrayObject Dumbbell::mVAO;
int32_t Dumbbell::totalNbDumbbells = 0;

// Constructor
Dumbbell::Dumbbell(const openglframework::vec3 &position,
				   ephysics::DynamicsWorld* dynamicsWorld, const etk::String& meshFolderPath)
		 : openglframework::Mesh() {

	// Load the mesh from a file
	openglframework::MeshReaderWriter::loadMeshFromFile(meshFolderPath + "dumbbell.obj", *this);

	// Calculate the normals of the mesh
	calculateNormals();

	// Identity scaling matrix
	m_scalingMatrix.setToIdentity();

	mDistanceBetweenSphere = 8.0f;

	// Initialize the position where the sphere will be rendered
	translateWorld(position);

	// Create a sphere collision shape for the two ends of the dumbbell
	// ReactPhysics3D will clone this object to create an int32_ternal one. Therefore,
	// it is OK if this object is destroyed right after calling RigidBody::addCollisionShape()
	const ephysics::float radiusSphere = ephysics::float(1.5);
	const ephysics::float massSphere = ephysics::float(2.0);
	mSphereShape = new ephysics::SphereShape(radiusSphere);

	// Create a cylinder collision shape for the middle of the dumbbell
	// ReactPhysics3D will clone this object to create an int32_ternal one. Therefore,
	// it is OK if this object is destroyed right after calling RigidBody::addCollisionShape()
	const ephysics::float radiusCylinder = ephysics::0.5f;
	const ephysics::float heightCylinder = ephysics::float(8.0);
	const ephysics::float massCylinder = ephysics::1.0f;
	mCylinderShape = new ephysics::CylinderShape(radiusCylinder, heightCylinder);

	// Initial position and orientation of the rigid body
	ephysics::vec3 initPosition(position.x(), position.y(), position.z());
	ephysics::float angleAroundX = 0;//ephysics::PI / 2;
	ephysics::etk::Quaternion initOrientation(angleAroundX, 0, 0);
	ephysics::etk::Transform3D transform_body(initPosition, initOrientation);

	mPreviousetk::Transform3D = transform_body;

	// Initial transform of the first sphere collision shape of the dumbbell (in local-space)
	ephysics::etk::Transform3D transformSphereShape1(ephysics::vec3(0, mDistanceBetweenSphere / 2.0f, 0), rp3d::etk::Quaternion::identity());

	// Initial transform of the second sphere collision shape of the dumbell (in local-space)
	ephysics::etk::Transform3D transformSphereShape2(ephysics::vec3(0, -mDistanceBetweenSphere / 2.0f, 0), rp3d::etk::Quaternion::identity());

	// Initial transform of the cylinder collision shape of the dumbell (in local-space)
	ephysics::etk::Transform3D transformCylinderShape(ephysics::vec3(0, 0, 0), rp3d::etk::Quaternion::identity());

	// Create a rigid body corresponding to the dumbbell in the dynamics world
	ephysics::RigidBody* body = dynamicsWorld->createRigidBody(transform_body);

	// Add the three collision shapes to the body and specify the mass and transform of the shapes
	m_proxyShapeSphere1 = body->addCollisionShape(mSphereShape, transformSphereShape1, massSphere);
	m_proxyShapeSphere2 = body->addCollisionShape(mSphereShape, transformSphereShape2, massSphere);
	m_proxyShapeCylinder = body->addCollisionShape(mCylinderShape, transformCylinderShape, massCylinder);

	m_body = body;

	m_transformMatrix = m_transformMatrix * m_scalingMatrix;

	// Create the VBOs and VAO
	if (totalNbDumbbells == 0) {
		createVBOAndVAO();
	}

	totalNbDumbbells++;
}

// Constructor
Dumbbell::Dumbbell(const openglframework::vec3 &position,
				   ephysics::CollisionWorld* world, const etk::String& meshFolderPath)
		 : openglframework::Mesh() {

	// Load the mesh from a file
	openglframework::MeshReaderWriter::loadMeshFromFile(meshFolderPath + "dumbbell.obj", *this);

	// Calculate the normals of the mesh
	calculateNormals();

	// Identity scaling matrix
	m_scalingMatrix.setToIdentity();

	mDistanceBetweenSphere = 8.0f;

	// Initialize the position where the sphere will be rendered
	translateWorld(position);

	// Create a sphere collision shape for the two ends of the dumbbell
	// ReactPhysics3D will clone this object to create an int32_ternal one. Therefore,
	// it is OK if this object is destroyed right after calling RigidBody::addCollisionShape()
	const ephysics::float radiusSphere = ephysics::float(1.5);
	mSphereShape = new ephysics::SphereShape(radiusSphere);

	// Create a cylinder collision shape for the middle of the dumbbell
	// ReactPhysics3D will clone this object to create an int32_ternal one. Therefore,
	// it is OK if this object is destroyed right after calling RigidBody::addCollisionShape()
	const ephysics::float radiusCylinder = ephysics::0.5f;
	const ephysics::float heightCylinder = ephysics::float(8.0);
	mCylinderShape = new ephysics::CylinderShape(radiusCylinder, heightCylinder);

	// Initial position and orientation of the rigid body
	ephysics::vec3 initPosition(position.x(), position.y(), position.z());
	ephysics::float angleAroundX = 0;//ephysics::PI / 2;
	ephysics::etk::Quaternion initOrientation(angleAroundX, 0, 0);
	ephysics::etk::Transform3D transform_body(initPosition, initOrientation);

	// Initial transform of the first sphere collision shape of the dumbbell (in local-space)
	ephysics::etk::Transform3D transformSphereShape1(ephysics::vec3(0, mDistanceBetweenSphere / 2.0f, 0), rp3d::etk::Quaternion::identity());

	// Initial transform of the second sphere collision shape of the dumbell (in local-space)
	ephysics::etk::Transform3D transformSphereShape2(ephysics::vec3(0, -mDistanceBetweenSphere / 2.0f, 0), rp3d::etk::Quaternion::identity());

	// Initial transform of the cylinder collision shape of the dumbell (in local-space)
	ephysics::etk::Transform3D transformCylinderShape(ephysics::vec3(0, 0, 0), rp3d::etk::Quaternion::identity());

	// Create a rigid body corresponding to the dumbbell in the dynamics world
	m_body = world->createCollisionBody(transform_body);

	// Add the three collision shapes to the body and specify the mass and transform of the shapes
	m_proxyShapeSphere1 = m_body->addCollisionShape(mSphereShape, transformSphereShape1);
	m_proxyShapeSphere2 = m_body->addCollisionShape(mSphereShape, transformSphereShape2);
	m_proxyShapeCylinder = m_body->addCollisionShape(mCylinderShape, transformCylinderShape);

	m_transformMatrix = m_transformMatrix * m_scalingMatrix;

	// Create the VBOs and VAO
	if (totalNbDumbbells == 0) {
		createVBOAndVAO();
	}

	totalNbDumbbells++;
}

// Destructor
Dumbbell::~Dumbbell() {

	if (totalNbDumbbells == 1) {

		// Destroy the mesh
		destroy();

		// Destroy the VBOs and VAO
		mVBOIndices.destroy();
		mVBOVertices.destroy();
		mVBONormals.destroy();
		mVBOTextureCoords.destroy();
		mVAO.destroy();
	}
	delete mSphereShape;
	delete mCylinderShape;
	totalNbDumbbells--;
}

// Render the sphere at the correct position and with the correct orientation
void Dumbbell::render(openglframework::Shader& shader,
					const openglframework::Matrix4& worldToCameraMatrix) {

	// Bind the shader
	shader.bind();

	// Set the model to camera matrix
	shader.setMatrix4x4Uniform("localToWorldMatrix", m_transformMatrix);
	shader.setMatrix4x4Uniform("worldToCameraMatrix", worldToCameraMatrix);

	// Set the normal matrix (inverse transpose of the 3x3 upper-left sub matrix of the
	// model-view matrix)
	const openglframework::Matrix4 localToCameraMatrix = worldToCameraMatrix * m_transformMatrix;
	const openglframework::Matrix3 normalMatrix =
					   localToCameraMatrix.getUpperLeft3x3Matrix().getInverse().getTranspose();
	shader.setetk::Matrix3x3Uniform("normalMatrix", normalMatrix, false);

	// Set the vertex color
	openglframework::Color currentColor = m_body->isSleeping() ? mSleepingColor : mColor;
	openglframework::Vector4 color(currentColor.r, currentColor.g, currentColor.b, currentColor.a);
	shader.setVector4Uniform("vertexColor", color, false);

	// Bind the VAO
	mVAO.bind();

	mVBOVertices.bind();

	// Get the location of shader attribute variables
	GLint32_t vertexPositionLoc = shader.getAttribLocation("vertexPosition");
	GLint32_t vertexNormalLoc = shader.getAttribLocation("vertexNormal", false);

	glEnableVertexAttribArray(vertexPositionLoc);
	glVertexAttribPointer(vertexPositionLoc, 3, GL_FLOAT, GL_FALSE, 0, (char*)NULL);

	mVBONormals.bind();

	if (vertexNormalLoc != -1) glEnableVertexAttribArray(vertexNormalLoc);
	if (vertexNormalLoc != -1) glVertexAttribPointer(vertexNormalLoc, 3, GL_FLOAT, GL_FALSE, 0, (char*)NULL);

	// For each part of the mesh
	for (uint32_t i=0; i<getNbParts(); i++) {
		glDrawElements(GL_TRIANGLES, getNbFaces(i) * 3, GL_UNSIGNED_INT, (char*)NULL);
	}

	glDisableVertexAttribArray(vertexPositionLoc);
	if (vertexNormalLoc != -1) glDisableVertexAttribArray(vertexNormalLoc);

	mVBONormals.unbind();
	mVBOVertices.unbind();

	// Unbind the VAO
	mVAO.unbind();

	// Unbind the shader
	shader.unbind();
}

// Create the Vertex Buffer Objects used to render with OpenGL.
/// We create two VBOs (one for vertices and one for indices)
void Dumbbell::createVBOAndVAO() {

	// Create the VBO for the vertices data
	mVBOVertices.create();
	mVBOVertices.bind();
	size_t sizeVertices = m_vertices.size() * sizeof(openglframework::vec3);
	mVBOVertices.copyDataIntoVBO(sizeVertices, getVerticesPointer(), GL_STATIC_DRAW);
	mVBOVertices.unbind();

	// Create the VBO for the normals data
	mVBONormals.create();
	mVBONormals.bind();
	size_t sizeNormals = mNormals.size() * sizeof(openglframework::vec3);
	mVBONormals.copyDataIntoVBO(sizeNormals, getNormalsPointer(), GL_STATIC_DRAW);
	mVBONormals.unbind();

	if (hasTexture()) {
		// Create the VBO for the texture co data
		mVBOTextureCoords.create();
		mVBOTextureCoords.bind();
		size_t sizeTextureCoords = mUVs.size() * sizeof(openglframework::vec2);
		mVBOTextureCoords.copyDataIntoVBO(sizeTextureCoords, getUVTextureCoordinatesPointer(), GL_STATIC_DRAW);
		mVBOTextureCoords.unbind();
	}

	// Create th VBO for the indices data
	mVBOIndices.create();
	mVBOIndices.bind();
	size_t sizeIndices = mIndices[0].size() * sizeof(uint32_t);
	mVBOIndices.copyDataIntoVBO(sizeIndices, getIndicesPointer(), GL_STATIC_DRAW);
	mVBOIndices.unbind();

	// Create the VAO for both VBOs
	mVAO.create();
	mVAO.bind();

	// Bind the VBO of vertices
	mVBOVertices.bind();

	// Bind the VBO of normals
	mVBONormals.bind();

	if (hasTexture()) {
		// Bind the VBO of texture coords
		mVBOTextureCoords.bind();
	}

	// Bind the VBO of indices
	mVBOIndices.bind();

	// Unbind the VAO
	mVAO.unbind();
}

// Reset the transform
void Dumbbell::resetTransform(const ephysics::Transform& transform) {

	// Reset the transform
	m_body->setTransform(transform);

	m_body->setIsSleeping(false);

	// Reset the velocity of the rigid body
	ephysics::RigidBody* rigidBody = dynamic_cast<ephysics::RigidBody*>(m_body);
	if (rigidBody != NULL) {
		rigidBody->setLinearVelocity(ephysics::vec3(0, 0, 0));
		rigidBody->setAngularVelocity(ephysics::vec3(0, 0, 0));
	}

	updateetk::Transform3D(1.0f);
}

// Set the scaling of the object
void Dumbbell::setScaling(const openglframework::vec3& scaling) {

	// Scale the collision shape
	ephysics::vec3 newScaling(scaling.x(), scaling.y(), scaling.z());
	m_proxyShapeCylinder->setLocalScaling(newScaling);
	m_proxyShapeSphere1->setLocalScaling(newScaling);
	m_proxyShapeSphere2->setLocalScaling(newScaling);

	mDistanceBetweenSphere = (mDistanceBetweenSphere / m_scalingMatrix.getValue(1, 1)) * scaling.y();

	// Initial transform of the first sphere collision shape of the dumbbell (in local-space)
	ephysics::etk::Transform3D transformSphereShape1(ephysics::vec3(0, mDistanceBetweenSphere / 2.0f, 0), rp3d::etk::Quaternion::identity());

	// Initial transform of the second sphere collision shape of the dumbell (in local-space)
	ephysics::etk::Transform3D transformSphereShape2(ephysics::vec3(0, -mDistanceBetweenSphere / 2.0f, 0), rp3d::etk::Quaternion::identity());

	m_proxyShapeSphere1->setLocalToBodyTransform(transformSphereShape1);
	m_proxyShapeSphere2->setLocalToBodyTransform(transformSphereShape2);

	// Scale the graphics object
	m_scalingMatrix = openglframework::Matrix4(scaling.x(), 0, 0, 0,
											  0, scaling.y(), 0, 0,
											  0, 0, scaling.z(), 0,
											  0, 0, 0, 1);
}
