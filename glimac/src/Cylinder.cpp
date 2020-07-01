#include <cmath>
#include <vector>
#include <iostream>
#include "glimac/common.hpp"
#include "glimac/Cylinder.hpp"

namespace glimac {

void Cylinder::build(GLfloat height, GLfloat r, GLsizei discLat, GLsizei discHeight) {
	// Equation paramétrique en (r, phi, h) du cylindre
	// avec r >= 0, 0 <= phi <= 2 * PI, 0 <= h <= height
    //
    // x(r, phi, h) = r sin(phi)
    // y(r, phi, h) = h
    // z(r, phi, h) = r cos(phi)
    //
    // Discrétisation:
    // dPhi = 2PI / discLat, dH = height / discHeight
    //
    // x(r, i, j) = r * sin(i * dPhi)
    // y(r, i, j) = j * dH
    // z(r, i, j) = r * cos(i * dPhi)

    GLfloat rcpLat = 1.f / discLat, rcpH = 1.f / discHeight;
    GLfloat dPhi = 2 * glm::pi<float>() * rcpLat, dH = height * rcpH;
    
    std::vector<ShapeVertex> data;
    
    // Construit l'ensemble des vertex
    for(GLsizei j = 0; j <= discHeight; ++j) {
        for(GLsizei i = 0; i < discLat; ++i) {
            ShapeVertex vertex;
            
            vertex.texCoords.x = i * rcpLat;
            vertex.texCoords.y = j * rcpH;
            
            vertex.position.x = r * sin(i * dPhi);
            vertex.position.y = j * dH;
            vertex.position.z = r * cos(i * dPhi);
            
            vertex.normal.x = sin(i * dPhi);
            vertex.normal.y = r;
            vertex.normal.z = cos(i * dPhi);
            vertex.normal = glm::normalize(vertex.normal);
            
            data.push_back(vertex);
        }
    }

    m_nVertexCount = discLat * discHeight * 6;

    GLuint idx = 0;
    // Construit les vertex finaux en regroupant les données en triangles:
    // Pour une longitude donnée, les deux triangles formant une face sont de la forme:
    // (i, i + 1, i + discLat + 1), (i, i + discLat + 1, i + discLat)
    // avec i sur la bande correspondant à la longitude
    for(GLsizei j = 0; j < discHeight; ++j) {
        GLsizei offset = j * discLat;
        for(GLsizei i = 0; i < discLat; ++i) {
            m_Vertices.push_back(data[offset + i]);
            m_Vertices.push_back(data[offset + (i + 1) % discLat]);
            m_Vertices.push_back(data[offset + discLat + (i + 1) % discLat]);
            m_Vertices.push_back(data[offset + i]);
            m_Vertices.push_back(data[offset + discLat + (i + 1) % discLat]);
            m_Vertices.push_back(data[offset + i + discLat]);
        }
    }
}
	
}