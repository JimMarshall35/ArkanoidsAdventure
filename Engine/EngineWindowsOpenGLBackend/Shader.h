#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
/*
struct UniformBufferVariable {
    std::string Name;
    void* Data;
    size_t DataSize;
};
struct UniformBufferDescription {
    std::string BlockName;
    std::vector<UniformBufferVariable> BufferVariables;
};
*/

class Shader
{
public:
    unsigned int ID = 0;
    Shader() {};
    Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath = nullptr)
    {
        if (geometryPath)
            LoadFromString(vertexPath, fragmentPath, geometryPath);
        else
            LoadFromString(vertexPath, fragmentPath);
    }

    void LoadFromFile(const char* vertexPath, const char* fragmentPath, const char* geometryPath = nullptr) {
        // 1. retrieve the vertex/fragment source code from filePath
        std::string vertexCode;
        std::string fragmentCode;
        std::string geometryCode;
        std::ifstream vShaderFile;
        std::ifstream fShaderFile;
        std::ifstream gShaderFile;
        // ensure ifstream objects can throw exceptions:
        vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        try
        {
            // open files
            vShaderFile.open(vertexPath);
            fShaderFile.open(fragmentPath);
            std::stringstream vShaderStream, fShaderStream;
            // read file's buffer contents into streams
            vShaderStream << vShaderFile.rdbuf();
            fShaderStream << fShaderFile.rdbuf();
            // close file handlers
            vShaderFile.close();
            fShaderFile.close();
            // convert stream into string
            vertexCode = vShaderStream.str();
            fragmentCode = fShaderStream.str();
            // if geometry shader path is present, also load a geometry shader
            if (geometryPath != nullptr)
            {
                gShaderFile.open(geometryPath);
                std::stringstream gShaderStream;
                gShaderStream << gShaderFile.rdbuf();
                gShaderFile.close();
                geometryCode = gShaderStream.str();
            }
        }
        catch (std::ifstream::failure& e)
        {
            std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
        }
        const char* vShaderCode = vertexCode.c_str();
        const char* fShaderCode = fragmentCode.c_str();
        // 2. compile shaders
        unsigned int vertex, fragment;
        // vertex shader
        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vShaderCode, NULL);
        glCompileShader(vertex);
        checkCompileErrors(vertex, "VERTEX");
        // fragment Shader
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fShaderCode, NULL);
        glCompileShader(fragment);
        checkCompileErrors(fragment, "FRAGMENT");
        // if geometry shader is given, compile geometry shader
        unsigned int geometry;
        if (geometryPath != nullptr)
        {
            const char* gShaderCode = geometryCode.c_str();
            geometry = glCreateShader(GL_GEOMETRY_SHADER);
            glShaderSource(geometry, 1, &gShaderCode, NULL);
            glCompileShader(geometry);
            checkCompileErrors(geometry, "GEOMETRY");
        }
        // shader Program
        ID = glCreateProgram();
        glAttachShader(ID, vertex);
        glAttachShader(ID, fragment);
        if (geometryPath != nullptr)
            glAttachShader(ID, geometry);
        glLinkProgram(ID);
        checkCompileErrors(ID, "PROGRAM");
        // delete the shaders as they're linked into our program now and no longer necessery
        glDeleteShader(vertex);
        glDeleteShader(fragment);
        if (geometryPath != nullptr)
            glDeleteShader(geometry);
    }


    void LoadFromString(const std::string vertexCode, const std::string fragmentCode, const std::string geometryCode = "")
    {

        const char* vShaderCode = vertexCode.c_str();
        const char* fShaderCode = fragmentCode.c_str();
        // 2. compile shaders
        unsigned int vertex, fragment;
        // vertex shader
        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vShaderCode, NULL);
        glCompileShader(vertex);
        checkCompileErrors(vertex, "VERTEX");
        // fragment Shader
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fShaderCode, NULL);
        glCompileShader(fragment);
        checkCompileErrors(fragment, "FRAGMENT");
        // if geometry shader is given, compile geometry shader
        unsigned int geometry;
        if (geometryCode != "")
        {
            const char* gShaderCode = geometryCode.c_str();
            geometry = glCreateShader(GL_GEOMETRY_SHADER);
            glShaderSource(geometry, 1, &gShaderCode, NULL);
            glCompileShader(geometry);
            checkCompileErrors(geometry, "GEOMETRY");
        }
        // shader Program
        ID = glCreateProgram();
        glAttachShader(ID, vertex);
        glAttachShader(ID, fragment);
        if (geometryCode != "")
            glAttachShader(ID, geometry);
        glLinkProgram(ID);
        checkCompileErrors(ID, "PROGRAM");
        // delete the shaders as they're linked into our program now and no longer necessery
        glDeleteShader(vertex);
        glDeleteShader(fragment);
        if (geometryCode != "")
            glDeleteShader(geometry);

    }
    // activate the shader
    // ------------------------------------------------------------------------
    void use() const
    {
        glUseProgram(ID);
    }
    // utility uniform functions
    // ------------------------------------------------------------------------

    GLint GetUniformLocation(const char* name) const
    {
        return glGetUniformLocation(ID, name);
    }
    // ------------------------------------------------------------------------
    void SetBoolDirect(GLint id, bool value) const
    {
        glUniform1i(id, (int)value);
    }
    // ------------------------------------------------------------------------

    void SetIntDirect(GLint id, int value) const
    {
        glUniform1i(id, (int)value);
    }
    // ------------------------------------------------------------------------
    void setFloatDirect(GLint id, float value) const
    {
        glUniform1f(id, value);
    }
    // ------------------------------------------------------------------------
    // ------------------------------------------------------------------------
    void setVec2Direct(GLint id, const glm::vec2& value) const
    {
        glUniform2fv(id, 1, &value[0]);
    }
    void SetIVec2Direct(GLint id, const glm::ivec2& value) const {
        glUniform2iv(id, 1, &value[0]);
    }
    void setVec2Direct(GLint id, float x, float y) const
    {
        glUniform2f(id, x, y);
    }
    // ------------------------------------------------------------------------
    void setVec3Direct(GLint id, const glm::vec3& value) const
    {
        glUniform3fv(id, 1, &value[0]);
    }
    void setVec3Direct(GLint id, float x, float y, float z) const
    {
        glUniform3f(id, x, y, z);
    }
    // ------------------------------------------------------------------------
    void setVec4Direct(GLint id, const glm::vec4& value) const
    {
        glUniform4fv(id, 1, &value[0]);
    }
    void setVec4(GLint id, float x, float y, float z, float w)
    {
        glUniform4f(id, x, y, z, w);
    }
    // ------------------------------------------------------------------------
    void setMat2Direct(GLint id, const glm::mat2& mat) const
    {
        glUniformMatrix2fv(id, 1, GL_FALSE, &mat[0][0]);
    }
    // ------------------------------------------------------------------------
    void setMat3Direct(GLint id, const glm::mat3& mat) const
    {
        glUniformMatrix3fv(id, 1, GL_FALSE, &mat[0][0]);
    }
    // ------------------------------------------------------------------------
    void setMat4Direct(GLint id, const glm::mat4& mat) const
    {
        glUniformMatrix4fv(id, 1, GL_FALSE, &mat[0][0]);
    }





    void setBool(const std::string& name, bool value) const
    {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
    }
    // ------------------------------------------------------------------------
    void setInt(const std::string& name, int value) const
    {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
    }
    // ------------------------------------------------------------------------
    void setFloat(const std::string& name, float value) const
    {
        glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
    }
    // ------------------------------------------------------------------------
    void setVec2(const std::string& name, const glm::vec2& value) const
    {
        glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }
    void SetIVec2(const std::string& name, const glm::ivec2& value) const {
        glUniform2iv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }
    void setVec2(const std::string& name, float x, float y) const
    {
        glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
    }
    // ------------------------------------------------------------------------
    void setVec3(const std::string& name, const glm::vec3& value) const
    {
        glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }
    void setVec3(const std::string& name, float x, float y, float z) const
    {
        glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
    }
    // ------------------------------------------------------------------------
    void setVec4(const std::string& name, const glm::vec4& value) const
    {
        glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }
    void setVec4(const std::string& name, float x, float y, float z, float w)
    {
        glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
    }
    // ------------------------------------------------------------------------
    void setMat2(const std::string& name, const glm::mat2& mat) const
    {
        glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    // ------------------------------------------------------------------------
    void setMat3(const std::string& name, const glm::mat3& mat) const
    {
        glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    // ------------------------------------------------------------------------
    void setMat4(const std::string& name, const glm::mat4& mat) const
    {
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    // ------------------------------------------------------------------------
    void setUniformBuffer(
        const std::vector<void*>& srcDatas,
        const std::vector<size_t>& srcSizes,
        const std::vector<const char*>& blockVariableNames,
        const std::string& blockName
    ) {
        //doesnt work properly
        const size_t NUM_VARS = blockVariableNames.size();
        use();
        GLuint blockIndex = glGetUniformBlockIndex(ID, "ParticleAttributesBlock");
        GLint blockSize;
        glGetActiveUniformBlockiv(ID, blockIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &blockSize);
        std::vector<GLbyte> blockBuffer(blockSize);
        std::vector<GLuint> indices(NUM_VARS);
        glGetUniformIndices(ID, blockVariableNames.size(), blockVariableNames.data(), indices.data());
        std::vector<GLint> offsets(NUM_VARS);
        glGetActiveUniformsiv(ID, NUM_VARS, indices.data(), GL_UNIFORM_OFFSET, offsets.data());
        for (int i = 0; i < NUM_VARS; i++) {
            memcpy(blockBuffer.data() + offsets[i], (GLbyte*)srcDatas[i], srcSizes[i]);
        }
        GLuint uboHandle;
        glGenBuffers(1, &uboHandle);
        glBindBuffer(GL_UNIFORM_BUFFER, uboHandle);
        glBufferData(GL_UNIFORM_BUFFER, blockSize, blockBuffer.data(), GL_DYNAMIC_DRAW);
        glBindBufferBase(GL_UNIFORM_BUFFER, 0, uboHandle);
    }
    /*
   void setUniformBuffer(const UniformBufferDescription& bd) {

       GLuint blockIndex = glGetUniformBlockIndex(ID, bd.BlockName.c_str());
       GLint blockSize;
       glGetActiveUniformBlockiv(ID, blockIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &blockSize);
       std::vector<GLbyte> blockBuffer(blockSize);

#define NUM_PARTICLE_ATTRIBUTES 3
        std::vector<const GLchar*> names(bd.BufferVariables.size());
        for (int i = 0; i < bd.BufferVariables.size(); i++) {
            names[i] = bd.BufferVariables[i].Name.c_str();
        }

        std::vector<GLuint> indices(names.size());
        glGetUniformIndices(ID, names.size(), names.data(), indices.data());
        std::vector<GLuint> offset(names.size());
        glGetActiveUniformsiv(ID, names.size(), indices.data(), GL_UNIFORM_OFFSET, offset.data());
        memcpy(blockBuffer + offset[0], explodeDirections.data(), explodeDirections.size() * sizeof(glm::vec2));
        memcpy(blockBuffer + offset[1], explodeRotations.data(), explodeRotations.size() * sizeof(GLfloat));
        memcpy(blockBuffer + offset[2], explodeSpeeds.data(), explodeSpeeds.size() * sizeof(GLfloat));


        glGenBuffers(1, &uboHandle);
        glBindBuffer(GL_UNIFORM_BUFFER, uboHandle);
        glBufferData(GL_UNIFORM_BUFFER, blockSize, blockBuffer.data(), GL_DYNAMIC_DRAW);
        glBindBufferBase(GL_UNIFORM_BUFFER, 0, uboHandle);
        GLPrintErrors();

    }
    */

private:
    // utility function for checking shader compilation/linking errors.
    // ------------------------------------------------------------------------
    void checkCompileErrors(GLuint shader, std::string type)
    {
        GLint success;
        GLchar infoLog[1024];
        if (type != "PROGRAM")
        {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
        else
        {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success)
            {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
    }
};