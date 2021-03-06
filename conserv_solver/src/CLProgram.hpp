#ifndef _CLPROGRAM_HPP__
#define _CLPROGRAM_HPP__

#include <string>
#include <sstream>
#include <vector>
#include <iomanip>

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <cl.h>
#endif

namespace CLUtils {
    
	
    inline std::string readFile(std::string file) {
        long length;
        std::string buffer;
        std::string contents;
        
        std::ifstream is;
        is.open(file.c_str());
        
        if (!is.good()) {
            std::string err = "Could not open ";
            err.append(file);
            THROW_EXCEPTION(err);
        }
        
        // get length of file:
        is.seekg (0, std::ios::end);
        length = is.tellg();
        is.seekg (0, std::ios::beg);
        
        // reserve memory:
        contents.reserve(length);
        
        // read data
        while(getline(is,buffer)) {
            contents.append(buffer);
            contents.append("\n");
        }
        is.close();
        
        return contents;
    }
    
    
    
    class Program {
    public:
        Program(CLcontext& context, std::string file, std::string* options = NULL) {
            cl_int err;
            
            const char* temp = readFile(file).c_str();
            const char* source[] = {temp};
            prog = clCreateProgramWithSource(context.context, 1, source, NULL, &err);
            if(err != CL_SUCCESS){
                THROW_EXCEPTION("Failed to create program");
            }
            
            compile(context.device, options);
        }
        
        cl_kernel createKernel(std::string func) {
            cl_int err;
            
            cl_kernel kernel = clCreateKernel(prog, func.c_str(), &err);
            if(err != CL_SUCCESS) {
                THROW_EXCEPTION("Failed to create kernel");
            }
            
            return kernel;
        }

        ~Program(){
            clReleaseProgram(prog);
        }

    private:
        void compile(cl_device_id device, std::string* options) {
            cl_int err;
            
            const char* opts = NULL;
            if(options != NULL){
                opts = options->c_str();
            }
            
            err = clBuildProgram(prog, 1, &device, opts, NULL, NULL);
            if(err != CL_SUCCESS){
                std::stringstream ss;
                ss << "Kernel failed to compile.\n";
                
                if(err == CL_BUILD_PROGRAM_FAILURE){
                    size_t size;
                    clGetProgramBuildInfo(prog, device, CL_PROGRAM_BUILD_LOG, 0, NULL, &size);
                    
                    // Get the log
                    std::string log;
                    log.resize(size);
                    clGetProgramBuildInfo(prog, device, CL_PROGRAM_BUILD_LOG, size, const_cast<char*>(log.data()),0);
                    
                    // Store the log
                    ss << log.c_str() << "." << std::endl;
                }
                
                THROW_EXCEPTION(ss.str());
            }
        }
        cl_program prog;
    };
    
}; //Namespace CLUtils

#endif