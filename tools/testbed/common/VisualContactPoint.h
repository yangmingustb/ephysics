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

#ifndef VISUAL_CONTACT_POINT_H
#define VISUAL_CONTACT_POINT_H

// Libraries
#include <ephysics/openglframework.hpp>

const float VISUAL_CONTACT_POINT_RADIUS = 0.2f;

// Class VisualContactPoint
class VisualContactPoint : public openglframework::Object3D {

	private :

		// -------------------- Attributes -------------------- //

		/// Total number of existing contact points (static attribute)
		static int32_t mNbTotalPoints;

		/// Sphere mesh for the visual contact point
		static openglframework::Mesh mMesh;

		/// Vertex Buffer Object for the vertices data
		static openglframework::VertexBufferObject mVBOVertices;

		/// Vertex Buffer Object for the normals data
		static openglframework::VertexBufferObject mVBONormals;

		/// Vertex Buffer Object for the indices
		static openglframework::VertexBufferObject mVBOIndices;

		/// Vertex Array Object for the vertex data
		static openglframework::VertexArrayObject mVAO;

		/// True if static data (VBO, VAO) has been created already
		static bool mStaticDataCreated;

		/// Color
		openglframework::Color mColor;

		// Create the Vertex Buffer Objects used to render with OpenGL.
		static void createVBOAndVAO();

		// -------------------- Methods -------------------- //

	public :

		// -------------------- Methods -------------------- //

		/// Constructor
		VisualContactPoint(const openglframework::vec3& position,
						   const etk::String &meshFolderPath);

		/// Destructor
		~VisualContactPoint();

		/// Load and initialize the mesh for all the contact points
		static void createStaticData(const etk::String& meshFolderPath);

		/// Destroy the mesh for the contact points
		static void destroyStaticData();

		/// Render the sphere at the correct position and with the correct orientation
		void render(openglframework::Shader& shader,
					const openglframework::Matrix4& worldToCameraMatrix);
};

#endif
