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

#ifndef TESTBED_APPLICATION_H
#define	TESTBED_APPLICATION_H

// Libraries
#include <ephysics/openglframework.hpp>
#include <ephysics/Gui.hpp>
#include <ephysics/Scene.hpp>
#include <ephysics/Timer.hpp>
#include <GLFW/glfw3.hpp>

using namespace nanogui;

// Macro for OpenGL errors
#define checkOpenGLErrors() checkOpenGLErrorsInternal(__FILE__,__LINE__)

// Constants
const float DEFAULT_TIMESTEP = 1.0f / 60.0f;

/// Class TestbedApplication
class TestbedApplication : public Screen {

	private :

		// -------------------- Constants -------------------- //

		static const float SCROLL_SENSITIVITY;

		// -------------------- Attributes -------------------- //

		bool mIsInitialized;

		Gui mGui;

		/// Timer
		Timer mTimer;

		/// List of 3D scenes
		etk::Vector<Scene*> mScenes;

		/// Current 3D scene
		Scene* mCurrentScene;

		/// Physics engine settings
		EngineSettings mEngineSettings;

		/// Current number of frames per seconds
		double mFPS;

		/// Number of frames during the last second
		int32_t mNbFrames;

		/// Current time for fps computation (in seconds)
		double mCurrentTime;

		/// Previous time for fps computation (in seconds)
		double mPreviousTime;

		/// Last time the FPS have been computed
		double mLastTimeComputedFPS;

		/// Update time (in seconds)
		double mFrameTime;

		/// Physics update time (in seconds)
		double mPhysicsTime;

		/// True if multisampling is active
		bool mIsMultisamplingActive;

		/// Width and height of the window
		int32_t m_width, mHeight;

		/// True if the next simulation update is a single physics step
		bool mSinglePhysicsStepEnabled;

		/// True if the single physics step has been taken already
		bool mSinglePhysicsStepDone;

		openglframework::vec2 mWindowToFramebufferRatio;

		/// True if shadow mapping is enabled
		bool mIsShadowMappingEnabled;

		/// True if contact points are displayed
		bool mIsContactPointsDisplayed;

		/// True if vsync is enabled
		bool mIsVSyncEnabled;

		// -------------------- Methods -------------------- //

		/// Private copy-constructor (for the singleton class)
		TestbedApplication(TestbedApplication const&);

		/// Private assignment operator (for the singleton class)
		void operator=(TestbedApplication const&);

		/// Update the physics of the current scene
		void updatePhysics();

		/// Update
		void update();

		/// Update the simulation by taking a single physics step
		void updateSinglePhysicsStep();

		/// Check the OpenGL errors
		static void checkOpenGLErrorsInternal(const char* file, int32_t line);

		/// Compute the FPS
		void computeFPS();

		/// Initialize all the scenes
		void createScenes();

		/// Remove all the scenes
		void destroyScenes();

		/// Return the list of the scenes
		etk::Vector<Scene*> getScenes();

		/// Start/stop the simulation
		void togglePlayPauseSimulation();

		/// Play the simulation
		void playSimulation();

		/// Pause the simulation
		void pauseSimulation();

		/// Restart the simulation
		void restartSimulation();

		/// Set the variable to know if we need to take a single physics step
		void toggleTakeSinglePhysicsStep();

		/// Enable/Disable shadow mapping
		void enableShadows(bool enable);

		/// Display/Hide contact points
		void displayContactPoints(bool display);

	public :

		// -------------------- Methods -------------------- //

		/// Private constructor (for the singleton class)
		TestbedApplication(bool isFullscreen);

		/// Destructor
		virtual ~TestbedApplication();

		/// Render the content of the application
		virtual void drawContents();

		/// Window resize event handler
		virtual bool resizeEvent(const vec2i& size);

		/// Default keyboard event handler
		virtual bool keyboardEvent(int32_t key, int32_t scancode, int32_t action, int32_t modifiers);

		/// Handle a mouse button event (default implementation: propagate to children)
		virtual bool mouseButtonEvent(const vec2i &p, int32_t button, bool down, int32_t modifiers);

		/// Handle a mouse motion event (default implementation: propagate to children)
		virtual bool mouseMotionEvent(const vec2i &p, const vec2i &rel, int32_t button, int32_t modifiers);

		/// Handle a mouse scroll event (default implementation: propagate to children)
		virtual bool scrollEvent(const vec2i &p, const vec2f &rel);

		/// Initialize the application
		void init();

		/// Change the current scene
		void switchScene(Scene* newScene);

		/// Enable/Disable Vertical synchronization
		void enableVSync(bool enable);

		// -------------------- Friendship -------------------- //

		friend class Gui;
};

// Return the list of the scenes
inline etk::Vector<Scene*> TestbedApplication::getScenes() {
	return mScenes;
}

// Toggle play/pause for the simulation
inline void TestbedApplication::togglePlayPauseSimulation() {

	if (mTimer.isRunning()) {
		mTimer.stop();
	}
	else {
		mTimer.start();
	}
}

// Play the simulation
inline void TestbedApplication::playSimulation() {
	if (!mTimer.isRunning()) mTimer.start();
}

// Pause the simulation
inline void TestbedApplication::pauseSimulation() {
	if (mTimer.isRunning()) mTimer.stop();
}

// Restart the simulation
inline void TestbedApplication::restartSimulation() {
	mCurrentScene->reset();
	mTimer.start();
}

// Take a single step of simulation
inline void TestbedApplication::toggleTakeSinglePhysicsStep() {
	mSinglePhysicsStepEnabled = true;
	mSinglePhysicsStepDone = false;

	if (mTimer.isRunning()) {
		mSinglePhysicsStepEnabled = false;
	}
}

// Enable/Disable shadow mapping
inline void TestbedApplication::enableShadows(bool enable) {
	mIsShadowMappingEnabled = enable;
}

/// Display/Hide contact points
inline void TestbedApplication::displayContactPoints(bool display) {
	mIsContactPointsDisplayed = display;
}

// Enable/Disable Vertical synchronization
inline void TestbedApplication::enableVSync(bool enable) {
	mIsVSyncEnabled = enable;
	if (mIsVSyncEnabled) {
		glfwSwapInterval(1);
	}
	else {
		glfwSwapInterval(0);
	}
}

#endif
