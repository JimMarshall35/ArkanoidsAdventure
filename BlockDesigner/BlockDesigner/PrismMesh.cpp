#include "pch.h"
#include "PrismMesh.h"
#include "Poly2D.h"
#include "MiscFunctions.h"
#include "ExtrudeParameters.h"

void PrismMesh::ExtrudeFromPoly2D(const Poly2D& poly, const ExtrudeParameters& params)
{
	m_Positions.clear();
	m_Indices.clear();
	m_Normals.clear();

	DesignateBottomCapStart();
	for (const glm::vec2& pt : poly.GetPoints())
	{
		PushVert(glm::vec3(pt, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f));
	}
	for (int ind : poly.GetIndices())
	{
		PushIndex(ind);
	}
	DesignateBottomCapEnd();
	
	DesignateTopCapStart();
	for (const glm::vec2& pt : poly.GetPoints())
	{
		PushVert(glm::vec3(pt, params.ExtrudeAmount), glm::vec3(0.0f, 0.0f, 1.0f));
	}
	for (int ind : poly.GetIndices())
	{
		PushIndex(ind + m_BottomCapVertsEnd);
	}
	DesignateTopCapEnd();

	ScaleAndTranslateTopCap(params);
	ScaleAndTranslateBottomCap(params);
	
	DesignateSidesStart();
	for (int i = m_BottomCapVertsStart; i < m_BottomCapVertsEnd; i++)
	{
		glm::vec3 b1 = m_Positions[i];
		int b2Ind = i + 1;
		glm::vec3 b2 = m_Positions[b2Ind < m_BottomCapVertsEnd ? b2Ind : m_BottomCapVertsStart];
	
		glm::vec3 t1 = m_Positions[i + m_TopCapVertsStart];
		int t2Ind = i + 1 + m_TopCapVertsStart;
		glm::vec3 t2 = m_Positions[t2Ind < m_TopCapVertsEnd ? t2Ind : m_TopCapVertsStart];
	
		// tri1: b1 -> t1 -> b2
		size_t t1Start = m_Positions.size();
		glm::vec3 normal = CalcNormal(b1, t1, b2);
		PushVert(b1, normal);
		PushVert(t1, normal);
		PushVert(b2, normal);
		m_Indices.push_back(t1Start);
		m_Indices.push_back(t1Start + 1);
		m_Indices.push_back(t1Start + 2);

		// tri2: t2 -> b2 -> t1
		size_t t2Start = m_Positions.size();
		normal = CalcNormal(t2, b2, t1);
		PushVert(t2, normal);
		PushVert(b2, normal);
		PushVert(t1, normal);
		m_Indices.push_back(t2Start);
		m_Indices.push_back(t2Start + 1);
		m_Indices.push_back(t2Start + 2);
	}
	DesignateSidesEnd();

}

void PrismMesh::PushVert(const glm::vec3& pos, const glm::vec3& normal)
{
	m_Normals.push_back(normal);
	m_Positions.push_back(pos);
}

void PrismMesh::PushIndex(int index)
{
	m_Indices.push_back(index);
}

void PrismMesh::DesignateBottomCapStart()
{
	m_BottomCapIndsStart = m_Indices.size();
	ASSERT(m_Positions.size() == m_Normals.size());
	m_BottomCapVertsStart = m_Positions.size();
}

void PrismMesh::DesignateBottomCapEnd()
{
	m_BottomCapIndsEnd = m_Indices.size();
	ASSERT(m_Positions.size() == m_Normals.size());
	m_BottomCapVertsEnd = m_Positions.size();
}

void PrismMesh::DesignateTopCapStart()
{
	m_TopCapIndsStart = m_Indices.size();
	ASSERT(m_Positions.size() == m_Normals.size());
	m_TopCapVertsStart = m_Positions.size();
}

void PrismMesh::DesignateTopCapEnd()
{
	m_TopCapIndsEnd = m_Indices.size();
	ASSERT(m_Positions.size() == m_Normals.size());
	m_TopCapVertsEnd = m_Positions.size();
}

void PrismMesh::DesignateSidesStart()
{
	m_SidesIndsStart = m_Indices.size();
	ASSERT(m_Positions.size() == m_Normals.size());
	m_SidesVertsStart = m_Positions.size();
}

void PrismMesh::DesignateSidesEnd()
{
	m_SidesIndsEnd = m_Indices.size();
	ASSERT(m_Positions.size() == m_Normals.size());
	m_SidesVertsEnd = m_Positions.size();
}

static void ScaleAndTranslateBase(
	std::vector<glm::vec3>& verts, 
	int vertsStart,
	int vertsEnd,
	float scaleX,
	float scaleY,
	float offsetX,
	float offsetY)
{
	float xTotal = 0;
	float yTotal = 0;
	int num = 0;
	for (int i = vertsStart; i < vertsEnd; i++)
	{
		xTotal += verts[i].x;
		yTotal += verts[i].y;
		num++;
	}
	glm::vec2 centroid = { xTotal / (float)num, yTotal / (float)num };
	for (int i = vertsStart; i < vertsEnd; i++)
	{
		glm::vec2 pos = verts[i];
		pos += -centroid;
		pos.x *= scaleX;
		pos.y *= scaleY;
		pos += centroid;
		verts[i] = glm::vec3(pos, verts[i].z);
		verts[i] += glm::vec3{ offsetX, offsetY, 0.0f };
	}
}

void PrismMesh::ScaleAndTranslateTopCap(const ExtrudeParameters& params)
{
	ScaleAndTranslateBase(
		m_Positions, 
		m_TopCapVertsStart,
		m_TopCapVertsEnd, 
		params.TopCapScaleX, 
		params.TopCapScaleY, 
		params.TopCapOffsetX, 
		params.TopCapOffsetY);
}

void PrismMesh::ScaleAndTranslateBottomCap(const ExtrudeParameters& params)
{
	ScaleAndTranslateBase(
		m_Positions,
		m_BottomCapVertsStart,
		m_BottomCapVertsEnd,
		params.BottomCapScaleX,
		params.BottomCapScaleY,
		params.BottomCapOffsetX,
		params.BottomCapOffsetY);
}
