//
//  CompFab.cpp
//  voxelizer
//
//
//

#include "includes/CompFab.h"
#include <iostream>
#include <string>
#include <cassert>
#include <sstream>
#include <fstream>
#include <vector>
using namespace CompFab;





CompFab::Vec3Struct::Vec3Struct()
{
    m_x = m_y = m_z = 0.0;
}

CompFab::Vec3Struct::Vec3Struct(precision_type x, precision_type y, precision_type z)
{
    m_x = x;
    m_y = y;
    m_z = z;
}

void CompFab::Vec3Struct::normalize() {
    
    precision_type magnitude = sqrt(m_x*m_x+m_y*m_y+m_z*m_z);
    
    if(magnitude > EPSILON)
    {
        m_x /= magnitude;
        m_y /= magnitude;
        m_z /= magnitude;
    }
}

//Data Types
CompFab::Vec3iStruct::Vec3iStruct()
{
    m_x = m_y = m_z = 0.0;
}

CompFab::Vec3iStruct::Vec3iStruct(precision_type x, precision_type y, precision_type z)
{
    m_x = x;
    m_y = y;
    m_z = z;
}

CompFab::Vec2fStruct::Vec2fStruct()
{
    m_x = m_y = 0.0;
}

CompFab::Vec2fStruct::Vec2fStruct(precision_type x, precision_type y)
{
    m_x = x;
    m_y = y;
}

CompFab::RayStruct::RayStruct()
{
    m_origin[0] = m_origin[1] = m_origin[2] = 0.0;
    m_direction[0] = 1.0;
    m_direction[1] = m_direction[2] = 0.0;
}

CompFab::RayStruct::RayStruct(Vec3 &origin, Vec3 &direction)
{
    m_origin = origin;
    m_direction = direction;
}

CompFab::TriangleStruct::TriangleStruct(Vec3 &v1, Vec3 &v2,Vec3 &v3)
{
    m_v1 = v1;
    m_v2 = v2;
    m_v3 = v3;
}

CompFab::Vec3 CompFab::operator-(const Vec3 &v1, const Vec3 &v2)
{
    Vec3 v3;
    v3[0] = v1[0] - v2[0];
    v3[1] = v1[1] - v2[1];
    v3[2] = v1[2] - v2[2];

    return v3;
}

CompFab::Vec3 CompFab::operator+(const Vec3 &v1, const Vec3 &v2)
{
    Vec3 v3;
    v3[0] = v1[0] + v2[0];
    v3[1] = v1[1] + v2[1];
    v3[2] = v1[2] + v2[2];
    
    return v3;
}


//Cross Product
Vec3 CompFab::operator%(const Vec3 &v1, const Vec3 &v2)
{
    Vec3 v3;
    v3[0] = v1[1]*v2[2] - v1[2]*v2[1];
    v3[1] = v1[2]*v2[0] - v1[0]*v2[2];
    v3[2] = v1[0]*v2[1] - v1[1]*v2[0];

    return v3;
}

//Dot Product
precision_type CompFab::operator*(const Vec3 &v1, const Vec3 &v2)
{
    return v1.m_x*v2.m_x + v1.m_y*v2.m_y+v1.m_z*v2.m_z;
}


//Grid structure for Voxels
CompFab::VoxelGridStruct::VoxelGridStruct(Vec3 lowerLeft, unsigned int dimX, unsigned int dimY, unsigned int dimZ, precision_type spacing)
{
    m_lowerLeft = lowerLeft;
    m_dimX = dimX;
    m_dimY = dimY;
    m_dimZ = dimZ;
    m_size = dimX*dimY*dimZ;
    m_spacing = spacing;

    //Allocate Memory
    m_insideArray = new bool[m_size];

    for(unsigned int ii=0; ii<m_size; ++ii)
    {
        m_insideArray[ii] = false;
    }
    
}

CompFab::VoxelGridStruct::~VoxelGridStruct()
{
    delete[] m_insideArray;
}

// void CompFab::VoxelGridStruct::save_binvox(const char * filename)
// {
    
// }



// void write_binvox(const char * filename) 
void CompFab::VoxelGridStruct::save_binvox(const char * filename)
{
    // Open file
    std::ofstream output(filename, std::ios::out | std::ios::binary);
    assert(output);
    
    // Write ASCII header
    output << "#binvox 1" << std::endl;
    output << "dim " << m_dimX << " " << m_dimY << " " << m_dimZ << "" << std::endl;
    output << "translate " << m_lowerLeft.m_x << " " << m_lowerLeft.m_y << " " << m_lowerLeft.m_z << "" << std::endl;
    output << "scale " <<  m_spacing << std::endl;
    output << "data" << std::endl;

    // Write first voxel
    char currentvalue = char(isInside(0,0,0));
    output.write((char*)&currentvalue, 1);
    char current_seen = 1;

    // Write BINARY Data
    for (size_t x = 0; x < m_dimX; x++){
        for (size_t z = 0; z < m_dimY; z++){
            for (size_t y = 0; y < m_dimZ; y++){
                if (x == 0 && y == 0 && z == 0){
                    continue;
                }
                char nextvalue = char(isInside(x, y, z));
                if (nextvalue != currentvalue || current_seen == (char) 255){
                    output.write((char*)&current_seen, 1);
                    current_seen = 1;
                    currentvalue = nextvalue;
                    output.write((char*)&currentvalue, 1);
                }
                else {
                    current_seen++;
                }
            }
        }
    }

    // Write rest
    output.write((char*)&current_seen, 1);
    output.close();
}

