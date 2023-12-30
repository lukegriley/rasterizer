#pragma once

#include <vector>
#include <glm/glm.hpp>
#include "sceneparser.h"

class Cube
{
public:
    void updateParams(int param1, RenderShapeData prim);
    std::vector<float> generateShape() { return m_vertexData; }

private:
    void insertVec3(std::vector<float> &data, glm::vec3 v);
    void setVertexData();
    void makeTile(glm::vec3 topLeft,
                  glm::vec3 topRight,
                  glm::vec3 bottomLeft,
                  glm::vec3 bottomRight,
                  glm::vec3 normal);
    void makeFace(glm::vec3 topLeft,
                  glm::vec3 topRight,
                  glm::vec3 bottomLeft,
                  glm::vec3 bottomRight,
                  glm::vec3 normal);

    std::vector<float> m_vertexData;
    int m_param1;
    RenderShapeData m_prim;
};
