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

#include "VirtualTrackball.h"

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
     * Function that enforces boundary condition
     */
    void setBoundary(CLUtils::MO<CL_MEM_READ_WRITE>* Qn);
    
    /**
     * Computes timestep based on CFL
     */
    float computeDt(CLUtils::MO<CL_MEM_READ_WRITE>* Qn);
    
    /**
	 * Simulation step
	 */
    void reconstruct(CLUtils::MO<CL_MEM_READ_WRITE>* Qn);
    
    /**
	 * Simulation step
	 */
    void evaluateFluxes(CLUtils::MO<CL_MEM_READ_WRITE>* Qn);
    
    /**
	 * Simulation step
	 */
    void computeRK(size_t n, float dt);
    
    /**
	 * Copy texture to framebuffer texture
	 */
    void copy(CLUtils::MO<CL_MEM_READ_WRITE>* src,
              CLUtils::MO<CL_MEM_READ_WRITE>* dest);
    
    /**
	 * Function that runs sim kernel
	 */
    void runKernel();
    
	/**
	 * Function that handles rendering into the OpenGL context
	 */
	void render();
    
    /**
     * Dumps simulation results to a new JSON files
     */
    void dumpJSON(std::string filename);
    
    /**
     * Download and dump debug information about a dataset
     */
    void debugDownload(CLUtils::MO<CL_MEM_READ_WRITE>* Qn, bool dump = false);
    
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
     * Sets up the buffers for us
     */
	void createBuffers();
    
    /**
     * Creates a NxNy size triangle strip grid
     */
    void createTriangleStrip(unsigned int Nx, unsigned int Ny, BO<GL_ARRAY_BUFFER>*& vert, BO<GL_ELEMENT_ARRAY_BUFFER>*& ind);
    
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
    
    static const unsigned int Nx            = 5;
    static const unsigned int Ny            = 4;
    
    static const unsigned int N_RK          = 3;
    
    static const unsigned int window_width  = 800;
	static const unsigned int window_height = 600;
    
    float gravity;
    
    // GLFW window handle
    GLFWwindow* window;
    
    // OpenCL context handle
    CLUtils::CLcontext context;
    
    // Timer
    float   time;
    size_t  step;
    
    GLuint tex;
    Program* visualize;
    
    BO<GL_ARRAY_BUFFER>* surface_vert;
    BO<GL_ELEMENT_ARRAY_BUFFER>* surface_ind;
    
    unsigned int restart_token;
    unsigned int indices_count;
    
    float h_min;
    float h_max;
    
    GLuint vao;
    
    glm::mat4 model;
    struct {
		glm::mat4 projection;
		glm::mat4 view;
	} camera;
    
    static VirtualTrackball trackball;
    
    CLUtils::Program*   compute_program;
    cl_kernel           compute_reconstruct;
    cl_kernel           evaluate_flux;
    cl_kernel           compute_RK;
    cl_kernel           compute_eigenvalues;
    cl_kernel           copy_domain;
    cl_kernel           prepare_render;
    cl_kernel           set_initial;
    cl_kernel           set_boundary_x;
    cl_kernel           set_boundary_y;
    
    
    CLUtils::MO<CL_MEM_READ_WRITE>*             Q_set[N_RK+1];
    CLUtils::MO<CL_MEM_READ_WRITE>*             Sx_set;
    CLUtils::MO<CL_MEM_READ_WRITE>*             Sy_set;
    CLUtils::MO<CL_MEM_READ_WRITE>*             F_set;
    CLUtils::MO<CL_MEM_READ_WRITE>*             G_set;
    CLUtils::MO<CL_MEM_WRITE_ONLY>*             E_set;
    
    CLUtils::ImageBuffer<CL_MEM_READ_WRITE>*    R_tex;
    
};

#endif
