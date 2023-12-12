#ifndef HEIGHTFIELD_H
#define HEIGHTFIELD_H

#include <iostream>
#include <stdint.h>
#include <vector>
#include <glm/glm.hpp>
#include <gl/glew.h>  // Include GLEW

class HeightField {
public:
    HeightField();

    void setShaderProgram(GLuint program) {
        shaderProgram = program;
    }

    void generateMesh(int tesselation, float spacing);
    void submitTriangles();

    GLuint getVAO() const {
        return m_vao;
    }

    GLuint getNumIndices() const {
        return m_numIndices;
    }

    void loadHeightField(const std::string& heightFieldPath);
    void loadDiffuseTexture(const std::string& diffusePath);

private:
    GLuint m_meshResolution;
    GLuint m_vao;
    GLuint m_positionBuffer;
    GLuint m_texCoordBuffer;
    GLuint m_uvBuffer;
    GLuint m_indexBuffer;
    GLuint m_numIndices;
    GLuint m_texid_hf;
    GLuint m_texid_diffuse;
    std::string m_heightFieldPath;
    std::string m_diffuseTexturePath;
    GLuint shaderProgram;  // Add this member variable
    float spacing;
};

#endif // HEIGHTFIELD_H
