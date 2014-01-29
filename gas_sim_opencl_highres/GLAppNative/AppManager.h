//
//  AppManager.h
//  GLAppNative
//
//  Created by Jens Kristoffer Reitan Markussen on 28.12.13.
//  Copyright (c) 2013 Jens Kristoffer Reitan Markussen. All rights reserved.
//

#ifndef GLAppNative_AppManager_h
#define GLAppNative_AppManager_h

#include "GLUtils/GLUtils.hpp"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"

#include "CLUtils/CLUtils.hpp"

#include "Timer.hpp"
#include "TextureFBO.h"

#include <vector>

using namespace GLUtils;

class AppManager{
public:
    /**
	 * Constructor
	 */
	AppManager();
    
	/**
	 * Destructor
	 */
	~AppManager();
    
	/**
	 * Initializes the game, including the OpenGL context
	 * and data required
	 */
	void init();
    
	/**
	 * The main loop of the app. Runs the main loop
	 */
	void begin();
    
private:
    /**
	 * Quit function
	 */
	void quit();
    
    /**
	 * Function that applies initial simulation state
	 */
    void applyInitial();
    
    /**
	 * Simulation step
	 */
    void reconstruct();
    
    /**
	 * Simulation step
	 */
    void evaluateFluxes();
    
    /**
	 * Simulation step
	 */
    void computeRK(glm::vec2 c);
    
    /**
	 * Function that runs sim kernel
	 */
    void runKernel(double dt);
    
	/**
	 * Function that handles rendering into the OpenGL context
	 */
	void render();
    
	/**
	 * Creates the OpenGL context using GLFW
	 */
	void createOpenGLContext();
    
	/**
	 * Sets states for OpenGL that we want to keep persistent
	 * throughout the game
	 */
	void setOpenGLStates();
    
	/**
	 * Compiles, attaches, links, and sets uniforms for
	 * a simple OpenGL program
	 */
	void createProgram();
    
	/**
	 * Creates vertex array objects
	 */
	void createVAO();
    
	/**
     * Sets up the FBO for us
     */
	void createFBO();
    
    /**
     * Callback for GLFW key press
     */
    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
    
    /**
     * Callback for GLFW mouse button press
     */
    static void mouse_callback(GLFWwindow* window, int button, int action, int mods);
    
    /**
     * Callback for GLFW cursor motion
     */
    static void cursor_callback(GLFWwindow* window, double x, double y);
    
    /**
     * Callback for GLFW error
     */
    static void error_callback(int error, const char* description) {
        THROW_EXCEPTION(description);
    }
    
private:
    
    static const unsigned int Nx            = 264;
    static const unsigned int Ny            = 264;
    
    static const unsigned int N_RK          = 2;
    
    static const unsigned int window_width  = 800;
	static const unsigned int window_height = 600;
    
    float gamma;
    
    // GLFW window handle
    GLFWwindow* window;
    
    // OpenCL context handle
    CLUtils::CLcontext context;
    
    // Timer
    Timer timer;
    
    Program* visualize;
    
    BO<GL_ARRAY_BUFFER>* vert;
    BO<GL_ELEMENT_ARRAY_BUFFER>* ind;
    
    GLuint vao;
    
    CLUtils::Program*   compute_program;
    cl_kernel           compute_boundary;
    cl_kernel           compute_reconstruct;
    cl_kernel           evaluate_flux;
    cl_kernel           compute_RK;
    
    
    CLUtils::MO<CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR>*  Q_set[N_RK+1];
    CLUtils::MO<CL_MEM_READ_WRITE>*                         Sx_set;
    CLUtils::MO<CL_MEM_READ_WRITE>*                         Sy_set;
    CLUtils::MO<CL_MEM_READ_WRITE>*                         F_set;
    CLUtils::MO<CL_MEM_READ_WRITE>*                         G_set;
    
    CLUtils::ImageBuffer<CL_MEM_READ_WRITE>*                R_tex;
    
};

#endif
