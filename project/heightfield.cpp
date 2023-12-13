
#include "heightfield.h"

#include <iostream>
#include <stdint.h>
#include <vector>
#include <glm/glm.hpp>
#include <stb_image.h>

using namespace glm;
using std::string;

HeightField terrain;

HeightField::HeightField(void)
    : m_meshResolution(0)
    , m_vao(UINT32_MAX)
    , m_positionBuffer(UINT32_MAX)
    , m_uvBuffer(UINT32_MAX)
    , m_indexBuffer(UINT32_MAX)
    , m_numIndices(0)
    , m_texid_hf(UINT32_MAX)
    , m_texid_diffuse(UINT32_MAX)
    , m_heightFieldPath("")
    , m_diffuseTexturePath("")
{
}




void HeightField::loadHeightField(const std::string& heigtFieldPath)
{
	int width, height, components;
	stbi_set_flip_vertically_on_load(true);
	float* data = stbi_loadf(heigtFieldPath.c_str(), &width, &height, &components, 1);
	if(data == nullptr)
	{
		std::cout << "Failed to load image: " << heigtFieldPath << ".\n";
		return;
	}

	if(m_texid_hf == UINT32_MAX)
	{
		glGenTextures(1, &m_texid_hf);
	}
	glBindTexture(GL_TEXTURE_2D, m_texid_hf);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, width, height, 0, GL_RED, GL_FLOAT,
	             data); // just one component (float)

	m_heightFieldPath = heigtFieldPath;
	std::cout << "Successfully loaded heigh field texture: " << heigtFieldPath << ".\n";
}

void HeightField::loadDiffuseTexture(const std::string& diffusePath)
{
	int width, height, components;
	stbi_set_flip_vertically_on_load(true);
	uint8_t* data = stbi_load(diffusePath.c_str(), &width, &height, &components, 3);
	if(data == nullptr)
	{
		std::cout << "Failed to load image: " << diffusePath << ".\n";
		return;
	}

	if(m_texid_diffuse == UINT32_MAX)
	{
		glGenTextures(1, &m_texid_diffuse);
	}

	glBindTexture(GL_TEXTURE_2D, m_texid_diffuse);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data); // plain RGB
	glGenerateMipmap(GL_TEXTURE_2D);

	std::cout << "Successfully loaded diffuse texture: " << diffusePath << ".\n";
}


void HeightField::generateMesh(int gridSize, float spacing)
{
    // Generate the height field vertices
    std::vector<float> vertices;
    for (int i = 0; i < gridSize - 1; ++i)
    {
        for (int j = 0; j < gridSize - 1; ++j)
        {
            float x0 = i * spacing - (gridSize * spacing) / 2.0f;
            float z0 = j * spacing - (gridSize * spacing) / 2.0f;
            float y0 = 0.0f;

            float x1 = (i + 1) * spacing - (gridSize * spacing) / 2.0f;
            float z1 = j * spacing - (gridSize * spacing) / 2.0f;
            float y1 = 0.0f;

            float x2 = i * spacing - (gridSize * spacing) / 2.0f;
            float z2 = (j + 1) * spacing - (gridSize * spacing) / 2.0f;
            float y2 = 0.0f;

            float x3 = (i + 1) * spacing - (gridSize * spacing) / 2.0f;
            float z3 = (j + 1) * spacing - (gridSize * spacing) / 2.0f;
            float y3 = 0.0f;

            // Triangle 1
            vertices.push_back(x0);
            vertices.push_back(y0);
            vertices.push_back(z0);

            vertices.push_back(x1);
            vertices.push_back(y1);
            vertices.push_back(z1);

            vertices.push_back(x2);
            vertices.push_back(y2);
            vertices.push_back(z2);

            // Triangle 2
            vertices.push_back(x2);
            vertices.push_back(y2);
            vertices.push_back(z2);

            vertices.push_back(x1);
            vertices.push_back(y1);
            vertices.push_back(z1);

            vertices.push_back(x3);
            vertices.push_back(y3);
            vertices.push_back(z3);
        }
    }

    std::vector<float> texCoords;
    for (int i = 0; i < gridSize; ++i)
    {
        for (int j = 0; j < gridSize; ++j)
        {
            float s = static_cast<float>(i) / (gridSize - 1);
            float t = static_cast<float>(j) / (gridSize - 1);

            texCoords.push_back(s);
            texCoords.push_back(t);
        }
    }

    // Generate index buffer
    std::vector<unsigned int> indices;
    for (int i = 0; i < gridSize - 1; ++i) {
        for (int j = 0; j < gridSize - 1; ++j) {
            // Triangle 1
            indices.push_back(i * gridSize + j);
            indices.push_back((i + 1) * gridSize + j);
            indices.push_back(i * gridSize + j + 1);

            // Triangle 2
            indices.push_back(i * gridSize + j + 1);
            indices.push_back((i + 1) * gridSize + j);
            indices.push_back((i + 1) * gridSize + j + 1);
        }
    }

    // Setup Vertex Buffer Object (VBO) and Vertex Array Object (VAO)
    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_positionBuffer);
    glGenBuffers(1, &m_indexBuffer);  // Add this line to generate the index buffer
    glGenBuffers(1, &m_texCoordBuffer);

    glBindVertexArray(m_vao);

    // Bind and load vertex positions
    glBindBuffer(GL_ARRAY_BUFFER, m_positionBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    // Texture Coordinates Buffer
    glBindBuffer(GL_ARRAY_BUFFER, m_texCoordBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * texCoords.size(), texCoords.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), nullptr);
    glEnableVertexAttribArray(1);

    // Bind and load index data
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), indices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    m_numIndices = indices.size();
}


void HeightField::submitTriangles()
{
    if (m_vao == UINT32_MAX)
    {
        std::cout << "No vertex array is generated, cannot draw anything.\n";
        return;
    }

    glUseProgram(shaderProgram);
    glBindVertexArray(m_vao);

    // Set uniform values if needed
    glUniform1i(glGetUniformLocation(shaderProgram, "hf"), 0);  // Texture unit 0
    glUniform1f(glGetUniformLocation(shaderProgram, "displaceNormal"), 3);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texid_hf);

    // Activate other texture units if needed
    // glActiveTexture(GL_TEXTURE1);
    // glBindTexture(GL_TEXTURE_2D, m_texid_diffuse);

    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);  // Adjust this based on your winding order

    // Use glDrawElements instead of glDrawArrays
    glDrawElements(GL_TRIANGLES, m_numIndices, GL_UNSIGNED_INT, 0);

    glDisable(GL_CULL_FACE);
    glBindVertexArray(0);
}

