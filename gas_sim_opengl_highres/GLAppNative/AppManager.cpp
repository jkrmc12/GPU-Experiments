//
//  AppManager.cpp
//  GLAppNative
//
//  Created by Jens Kristoffer Reitan Markussen on 28.12.13.
//  Copyright (c) 2013 Jens Kristoffer Reitan Markussen. All rights reserved.
//

#include "AppManager.h"
#include <vector>
#include <glm/glm.hpp>

AppManager::AppManager():
    //lax_f(NULL),
    visualize(NULL),
    copy(NULL),
    vert(NULL),
    ind(NULL),
    kernel0(NULL),
    kernel1(NULL),
    gamma(0.0f),
    pressure(0.0f){
}

AppManager::~AppManager(){
}

void AppManager::init(){
    /* Initialize the library */
    if (glfwInit() != GL_TRUE) {
        THROW_EXCEPTION("Failed to initialize GLFW");
    }
    glfwSetErrorCallback(error_callback);
    
    createOpenGLContext();
    setOpenGLStates();
    createFBO();
    createProgram();
    createVAO();
    
    applyInitial();
}

void AppManager::begin(){
    while (!glfwWindowShouldClose(window)) {
        /* Poll for and process events */
        glfwPollEvents();
        
        /* Render loop */
        render();
        
        /* Swap front and back buffers */
        glfwSwapBuffers(window);
    }
    
    /* Clean up everything */
    quit();
}

void AppManager::quit(){
    delete visualize;
    delete copy;
    delete vert;
    delete ind;
    delete kernel0;
    delete kernel1;
    
    glfwDestroyWindow(window);
    glfwTerminate();
}


float E(float rho, float u, float v, float gamma, float p){
    return 0.5*rho*(u*u+v*v)+p/(gamma-1.0f);
}

void AppManager::applyInitial(){
    gamma       = 1.4f;
    
    GLuint tex;
    glGenTextures(1, &tex);
    
    glBindTexture(GL_TEXTURE_2D, tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    // Initialize grid with initial data
    std::vector<GLfloat> data(Nx*Ny*4);
    
    for (size_t i = 0; i < Ny; i++) {
        float x     = 0.1f;
        size_t k    = (Nx * i + (size_t)(x*(float)Nx))*4;
        data[k]     = 0.3f;
        //data[k+1]   = 0.3*0.3f;
        //data[k+3]   = E(data[k], data[k+1], data[k+2], gamma, 0.3f);
    }
    
    for (size_t i = 0; i < 360; i++) {
        float rad   = glm::radians((float)i);
        float X = 0.5 + (0.2 * glm::sin(rad));
        float Y = 0.5 + (0.2 * glm::cos(rad));
        
        size_t y = (size_t)(Y*(float)Nx);
        size_t x = (size_t)(X*(float)Ny);
        
        data[(Nx * y + x)*4] = 0.1f;
    }
    
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, Nx, Ny,
                 0, GL_RGBA, GL_FLOAT, data.data());
    
    CHECK_GL_ERRORS();
    
    // Render initial grid to kernel0
    kernel0->bind();
    glViewport(0, 0, Nx, Ny);
    
    copy->use();
    
    //set uniforms
    glUniform1i(copy->getUniform("QTex"), 0);
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex);
    
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, NULL);
    glBindVertexArray(0);
    
    copy->disuse();
    kernel0->unbind();
    
    glBindTexture(GL_TEXTURE_2D, 0);
    glDeleteTextures(1, &tex);
    
    CHECK_GL_ERRORS();
}

void reconstruct(){
    
}

void evaluateFluxes(){
    
}

void computeRK(glm::vec2 c){
    
}

void AppManager::runKernel(double dt){
    
    static const glm::vec2 c[2] =
                        {glm::vec2(0.0,1.0),
                        glm::vec2(0.5,0.5)};
    
    for (size_t i = 0; i < 2; i++) {
        // apply boundary condition
        // TODO: implement boundary shader
        
        // reconstruct point values
        reconstruct();
        
        // evaluate fluxes
        evaluateFluxes();
        
        // compute RK
        computeRK(c[i]);
    }
    
    CHECK_GL_ERRORS();
}

void AppManager::render(){
    double dt = 0.005;//timer.elapsedAndRestart();
    runKernel(dt);
    
    glViewport(0, 0, window_width*2, window_height*2);
    visualize->use();
    
    glUniform1i(visualize->getUniform("QTex"), 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, kernel0->getTexture(0));
    
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, NULL);
    glBindVertexArray(0);
    
    visualize->disuse();
    
    CHECK_GL_ERRORS();
}

void AppManager::createOpenGLContext(){
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    glfwWindowHint(GLFW_DEPTH_BITS, 32);
    glfwWindowHint(GLFW_RED_BITS, 8);
    glfwWindowHint(GLFW_GREEN_BITS, 8);
    glfwWindowHint(GLFW_BLUE_BITS, 8);
    glfwWindowHint(GLFW_ALPHA_BITS, 8);
    
    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(window_width, window_height, "GL App", NULL, NULL);
    if (window == NULL) {
        THROW_EXCEPTION("Failed to create window");
    }
    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_callback);
    glfwSetCursorPosCallback(window, cursor_callback);

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    
    glewExperimental = GL_TRUE;
	GLenum glewErr = glewInit();
	if (glewErr != GLEW_OK) {
		std::stringstream err;
		err << "Error initializing GLEW: " << glewGetErrorString(glewErr);
		THROW_EXCEPTION(err.str());
	}
    
	// Unfortunately glewInit generates an OpenGL error, but does what it's
	// supposed to (setting function pointers for core functionality).
	// Lets do the ugly thing of swallowing the error....
    glGetError();
}

void AppManager::setOpenGLStates(){
    //glEnable(GL_DEPTH_TEST);
	//glDepthFunc(GL_LEQUAL);
	glEnable(GL_CULL_FACE);
    
    glViewport(0, 0, window_width, window_height);
	glClearColor(0.0, 0.0, 0.0, 1.0);
    
    CHECK_GL_ERRORS();
}

void AppManager::createProgram(){
    visualize   = new Program("kernel.vert","visualize.frag");
    copy        = new Program("kernel.vert","copy.frag");
    
    //Set uniforms

    CHECK_GL_ERRORS();
}

void AppManager::createVAO(){
    GLfloat quad_vertices[] =  {
		-1.0f, -1.0f,
        1.0f, -1.0f,
        1.0f,  1.0f,
		-1.0f,  1.0f,
	};
    vert = new BO<GL_ARRAY_BUFFER>(quad_vertices, sizeof(quad_vertices));
    
	GLubyte quad_indices[] = {
		0, 1, 2, //triangle 1
		2, 3, 0, //triangle 2
	};
    ind = new BO<GL_ELEMENT_ARRAY_BUFFER>(quad_indices, sizeof(quad_indices));
    
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    visualize->use();
    vert->bind();
    visualize->setAttributePointer("position", 2);
    ind->bind();
    glBindVertexArray(0);
    
    CHECK_GL_ERRORS();
}

void AppManager::createFBO(){
    kernel0 = new TextureFBO(Nx, Ny, 1, GL_RGBA32F);
    kernel1 = new TextureFBO(Nx, Ny, 1, GL_RGBA32F);
    
    CHECK_GL_ERRORS();
}

void AppManager::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}

void AppManager::mouse_callback(GLFWwindow* window, int button, int action, int mods){
}

void AppManager::cursor_callback(GLFWwindow* window, double x, double y){
}