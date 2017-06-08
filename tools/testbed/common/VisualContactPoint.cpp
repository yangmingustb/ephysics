/********************************************************************************
* ReactPhysics3D physics library, http://www.reactphysics3d.com				 *
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
#include <ephysics/VisualContactPoint.h>

// Initialization of static variables
openglframework::VertexBufferObject VisualContactPoint::mVBOVertices(GL_ARRAY_BUFFER);
openglframework::VertexBufferObject VisualContactPoint::mVBONormals(GL_ARRAY_BUFFER);
openglframework::VertexBufferObject VisualContactPoint::mVBOIndices(GL_ELEMENT_ARRAY_BUFFER);
openglframework::VertexArrayObject VisualContactPoint::mVAO;
int32_t VisualContactPoint::mNbTotalPoints = 0;
openglframework::Mesh VisualContactPoint::mMesh;
bool VisualContactPoint::mStaticDataCreated = false;

// Constructor
VisualContactPoint::VisualContactPoint(const openglframework::Vector3& position,
									   const std::string& meshFolderPath)
	: mColor(1.0f, 0.0f, 0.0f, 1.0f) {

	// Initialize the position where the mesh will be rendered
	translateWorld(position);
}

// Destructor
VisualContactPoint::~VisualContactPoint() {

}

// Load and initialize the mesh for all the contact points
void VisualContactPoint::createStaticData(const std::string& meshFolderPath) {

	if (mStaticDataCreated) return;

	// Load the mesh from a file
	openglframework::MeshReaderWriter::loadMeshFromFile(meshFolderPath + "sphere.obj", mMesh);

	// Calculate the normals of the mesh
	mMesh.calculateNormals();

	mMesh.scaleVertices(VISUAL_CONTACT_POINT_RADIUS);

	createVBOAndVAO();

	mStaticDataCreated = true;
}

// Destroy the mesh for the contact points
void VisualContactPoint::destroyStaticData() {

	if (!mStaticDataCreated) return;

	// Destroy the VBOs and VAO
	mVBOIndices.destroy();
	mVBOVertices.destroy();
	mVBONormals.destroy();
	mVAO.destroy();

	mMesh.destroy();

	mStaticDataCreated = false;
}

// Render the sphere at the correct position and with the correct orientation
void VisualContactPoint::render(openglframework::Shader& shader,
								const openglframework::Matrix4& worldToCameraMatrix) {

	// Bind the VAO
	mVAO.bind();

	// Bind the shader
	shader.bind();

	mVBOVertices.bind();

	// Set the model to camera matrix
	shader.setMatrix4x4Uniform("localToWorldMatrix", mTransformMatrix);
	shader.setMatrix4x4Uniform("worldToCameraMatrix", worldToCameraMatrix);

	// Set the normal matrix (inverse transpose of the 3x3 upper-left sub matrix of the
	// model-view matrix)
	const openglframework::Matrix4 localToCameraMatrix = worldToCameraMatrix * mTransformMatrix;
	const openglframework::Matrix3 normalMatrix =
					   localToCameraMatrix.getUpperLeft3x3Matrix().getInverse().getTranspose();
	shader.setMatrix3x3Uniform("normalMatrix", normalMatrix, false);

	// Set the vertex color
	openglframework::Vector4 color(mColor.r, mColor.g, mColor.b, mColor.a);
	shader.setVector4Uniform("vertexColor", color, false);

	// Get the location of shader attribute variables
	GLint32_t vertexPositionLoc = shader.getAttribLocation("vertexPosition");
	GLint32_t vertexNormalLoc = shader.getAttribLocation("vertexNormal", false);

	glEnableVertexAttribArray(vertexPositionLoc);
	glVertexAttribPointer(vertexPositionLoc, 3, GL_FLOAT, GL_FALSE, 0, (char*)NULL);

	mVBONormals.bind();

	if (vertexNormalLoc != -1) glVertexAttribPointer(vertexNormalLoc, 3, GL_FLOAT, GL_FALSE, 0, (char*)NULL);
	if (vertexNormalLoc != -1) glEnableVertexAttribArray(vertexNormalLoc);

	// For each part of the mesh
	for (uint32_t i=0; i<mMesh.getNbParts(); i++) {
		glDrawElements(GL_TRIANGLES, mMesh.getNbFaces(i) * 3, GL_UNSIGNED_INT, (char*)NULL);
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
void VisualContactPoint::createVBOAndVAO() {

	// Create the VBO for the vertices data
	mVBOVertices.create();
	mVBOVertices.bind();
	size_t sizeVertices = mMesh.getVertices().size() * sizeof(openglframework::Vector3);
	mVBOVertices.copyDataIntoVBO(sizeVertices, mMesh.getVerticesPointer(), GL_STATIC_DRAW);
	mVBOVertices.unbind();

	// Create the VBO for the normals data
	mVBONormals.create();
	mVBONormals.bind();
	size_t sizeNormals = mMesh.getNormals().size() * sizeof(openglframework::Vector3);
	mVBONormals.copyDataIntoVBO(sizeNormals, mMesh.getNormalsPointer(), GL_STATIC_DRAW);
	mVBONormals.unbind();

	// Create th VBO for the indices data
	mVBOIndices.create();
	mVBOIndices.bind();
	size_t sizeIndices = mMesh.getIndices(0).size() * sizeof(uint32_t);
	mVBOIndices.copyDataIntoVBO(sizeIndices, mMesh.getIndicesPointer(), GL_STATIC_DRAW);
	mVBOIndices.unbind();

	// Create the VAO for both VBOs
	mVAO.create();
	mVAO.bind();

	// Bind the VBO of vertices
	mVBOVertices.bind();

	// Bind the VBO of normals
	mVBONormals.bind();

	// Bind the VBO of indices
	mVBOIndices.bind();

	// Unbind the VAO
	mVAO.unbind();
}
