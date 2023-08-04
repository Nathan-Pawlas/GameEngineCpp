#pragma once
#include "Vec3.h"
#include "Vec2.h"

class VertexMesh
{
public:
	VertexMesh() :m_pos(), m_texcoord(), m_normal()
	{
	}
	VertexMesh(Vec3 pos, Vec2 m_texcoord, Vec3 normal) :m_pos(pos), m_texcoord(m_texcoord), m_normal(normal)
	{
	}
	VertexMesh(const VertexMesh& vert_mesh) :m_pos(vert_mesh.m_pos), m_texcoord(vert_mesh.m_texcoord), m_normal(vert_mesh.m_normal)
	{
	}
	~VertexMesh()
	{
	}

public:
	Vec3 m_pos;
	Vec2 m_texcoord;
	Vec3 m_normal;
};
