/*
*       a___d
*       |\  |
*       | \ |
*       |__\|
*       b   c
*/
#ifndef SPHEREARRAY_H
#define SPHEREARRAY_H

#include <cmath>
#include <iostream>
#include <vector>

class SphereArray
{
public:
    SphereArray(unsigned int lat, unsigned int lon, float radius = 1.0f)
        : m_numberOfPoint((lat + 1) * (lon + 1))
        , m_verticeCount(3 * m_numberOfPoint)
        , m_texCoordCount(2 * m_numberOfPoint)
        , m_indiceCount(6 * m_numberOfPoint)
        , m_vertices(new float[m_verticeCount])
        , m_texCoords(new float[m_texCoordCount])
        , m_indices(new unsigned int[m_indiceCount])
    {
        generateSphere(lat, lon, radius);
    }
  
    ~SphereArray()
    {
        delete []m_vertices;
        delete []m_texCoords;
        delete []m_indices;
    }

    const float *vertices() const { return m_vertices; }
    const float *texCoords() const { return m_texCoords; }
    const unsigned int *indices() const { return m_indices; }

    unsigned int verticeCount() const { return m_verticeCount; }
    unsigned int texCoordCount() const { return m_texCoordCount; }
    unsigned int indiceCount() const { return m_indiceCount; }

private:
    static constexpr double Pi = 3.14159265358979323846264338328;

    unsigned int m_numberOfPoint;

    unsigned int m_verticeCount;
    unsigned int m_texCoordCount;
    unsigned int m_indiceCount;

    float *m_vertices;
    float *m_texCoords;
    unsigned int *m_indices;

    void generateSphere(unsigned int lat, unsigned int lon, float r)
    {
        const double vstep = 1.0 / (double)lat;
        const double ustep = 1.0 / (double)lon;
        const unsigned int lonPlusOne = lon + 1;
        unsigned int v = 0, t = 0, idx = 0;

        for (int i = 0; i < lat + 1; ++i) {
            for (int j = 0; j < lon + 1; ++j) { 
                // x坐标, y坐标, z坐标 依顺序赋值
                m_vertices[v++] = r * sin(Pi * i * vstep) * cos(2 * Pi * j * ustep);
                m_vertices[v++] = r * cos(Pi * i * vstep);
                m_vertices[v++] = r * sin(Pi * i * vstep) * sin(2 * Pi * j * ustep);
                // 纹理坐标
                m_texCoords[t++] = j * ustep;
                m_texCoords[t++] = i * vstep;            
            }
        }
        // 索引坐标
        for (int i = 0; i < lat; ++i) {
            for (int j = 0; j < lon; ++j) {                 
                m_indices[idx++] = i * lonPlusOne + j; // a        
                m_indices[idx++] = (i + 1) * lonPlusOne + j; // b 
                m_indices[idx++] = i * lonPlusOne + j + 1;  // c 
                m_indices[idx++] = i * lonPlusOne + j + 1; // c  
                m_indices[idx++] = (i + 1) * lonPlusOne + j; // b
                m_indices[idx++] = (i + 1) * lonPlusOne + j + 1; // d 
            }
        }
    }
};


#endif // SPHEREARRAY_H
