#ifndef SPHEREARRAY_H
#define SPHEREARRAY_H

#include <cmath>
#include <iostream>
#include <vector>

class SphereArray
{
public:
    SphereArray(unsigned int lat, unsigned int lon, float radius = 1.0f)
        : m_verticeCount(3 * ((lat - 1) * (lon + 1) + 2))
        , m_texCoordCount(2 * ((lat - 1) * (lon + 1) + 2))
        , m_indiceCount(5 * m_verticeCount + 1000)
        , m_vertices(new float[m_verticeCount])
        , m_normals(new float[m_verticeCount])
        , m_texCoords(new float[m_texCoordCount])
        , m_indices(new unsigned int[m_indiceCount])
    {
        createSphere(lat, lon, radius);
    }
  
    ~SphereArray()
    {
        delete []m_vertices;
        delete []m_normals;
        delete []m_texCoords;
        delete []m_indices;
    }

    const float *vertices() const { return m_vertices; }
    const float *texCoords() const { return m_texCoords; }
    const unsigned int *indices() const { return m_indices; }
//    const unsigned int *indices() const { return &indexs[0]; }

    unsigned int verticeCount() const { return m_verticeCount; }
    unsigned int texCoordCount() const { return m_texCoordCount; }
    unsigned int indiceCount() const { return m_indiceCount; }

private:
    static constexpr double Pi = 3.14159265358979323846264338328;

    unsigned int m_verticeCount;
    unsigned int m_texCoordCount;
    unsigned int m_indiceCount;

    float *m_vertices;
    float *m_normals;
    float *m_texCoords;
    unsigned int *m_indices;
    //std::vector<unsigned int> indexs;

    void createSphere(unsigned int lat, unsigned int lon, float r) 
    {
        const double vstep = 1.0 / (double)lat;
        const double ustep = 1.0 / (double)lon;

        setPosition(0, 0, r, &m_vertices[0], &m_vertices[1], &m_vertices[2]);
        m_texCoords[0] = 0.5f;
        m_texCoords[1] = 1;

        int c = 1;
        for (int i = 1; i < lat; ++i) {
            for (int j = 0; j <= lon; ++j) { 
                setPosition(ustep * j, vstep * i, r, &m_vertices[3 * c], &m_vertices[3 * c + 1], &m_vertices[3 * c + 2]);
                m_texCoords[2 * c] = 1 - ustep * j;
                m_texCoords[2 * c + 1] = vstep * i;
                ++c;
            }
        }

        setPosition(1, 1, r, &m_vertices[3 * c], &m_vertices[3 * c + 1], &m_vertices[3 * c + 2]);
//        m_texCoords[2 * c] = 0.5f;
//        m_texCoords[2 * c + 1] = 0;
        m_texCoords[c] = 0.5f;
        m_texCoords[c + 1] = 0;

        int x = 0, start, last;
        for (int i = 0; i <= lon; ++i) { //球体上最顶层
            m_indices[x] = 0;
            m_indices[x + 1] = i + 1;
            m_indices[x + 2] = 1 + (i + 1) % (lon + 1);
            x += 3;
        }
        std::cout << "x1 = " << x << std::endl;
        for (int i = 1; i < lat - 1; ++i) {
            int start = 1 + (i - 1) * (lon + 1);
            for (int j = start; j < start + (1 + lon); ++j) {
                m_indices[x] = j;
                m_indices[x + 1] = j + (1 + lon);
                m_indices[x + 2] = start + (1 + lon) + (j + 1 - start) % (1 + lon);
                m_indices[x + 3] = m_indices[x];
                m_indices[x + 4] = start + (j + 1 - start) % (1 + lon);
                m_indices[x + 5] = m_indices[x + 2];
                x += 6;
            }
        }std::cout << "x2 = " << x << std::endl;

        last = 1 + (lon + 1) * (lat - 1);
        start = 1 + (lon + 1) * (lat - 2);
        for (int i = start; i < 1 + (lon + 1) * (lat - 1); ++i) //球体上最底层
        {
            m_indices[x] = i;
            m_indices[x + 1] = last; //逆时针排列
            m_indices[x + 2] = start + ((1 + i) - start) % (1 + lon);

            x += 3;
        }
        std::cout << "c = " << c << " x = " << x << " indiceCount = " << m_indiceCount << std::endl;
        m_indiceCount = x + 1;
    }

    inline void setPosition(double u, double v, float r, float *x, float *y ,float *z)
    {
        *x = r * sin(Pi * v) * cos(2 * Pi * u);
        *y = r * sin(Pi * v) * sin(2 * Pi * u);
        *z = r * cos(Pi * v);
    }
};


#endif // SPHEREARRAY_H
