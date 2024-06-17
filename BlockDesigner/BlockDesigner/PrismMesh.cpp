#include "pch.h"
#include "PrismMesh.h"
#include "Poly2D.h"
#include "MiscFunctions.h"
#include "ExtrudeParameters.h"
#include "xatlas.h"
#include "Rasterize.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

static void RandomColor(uint8_t* color)
{
	for (int i = 0; i < 3; i++)
		color[i] = uint8_t((rand() % 255 + 192) * 0.5f);
}

static glm::vec3 CalculateMeshCenter(const std::vector<glm::vec3>& vertPositions)
{
	float xTotal = 0.0f;
	float yTotal = 0.0f;
	float zTotal = 0.0f;
	for (const glm::vec3& v : vertPositions)
	{
		xTotal += v.x;
		yTotal += v.y;
		zTotal += v.z;
	}
	return glm::vec3
	{
		xTotal / (float)vertPositions.size(),
		yTotal / (float)vertPositions.size(),
		zTotal / (float)vertPositions.size()
	};
}

static glm::vec3 CalculateTriangleCenter(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3)
{
	return glm::vec3
	{
		(p1.x + p2.x + p3.x) / 3.0f,
		(p1.y + p2.y + p3.y) / 3.0f,
		(p1.z + p2.z + p3.z) / 3.0f,
	};
}

glm::vec3 PrismMesh::GetCentroid()
{
	return CalculateMeshCenter(m_Positions);
}

glm::vec3 PrismMesh::CalculateCorrectNormal(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3, const Poly2D& bottomCap, bool& outbFlipped)
{
	auto GetBottomCapPoint = [&]() -> glm::vec3
	{
		if (AreSame(p1.z, 0)) return p1;
		if (AreSame(p2.z, 0)) return p2;
		if (AreSame(p3.z, 0)) return p3;
		ASSERT(false);
		return { 0,0,0 };
	};
	outbFlipped = false;
	glm::vec3 normal = CalcNormal(p1, p2, p3);
	glm::vec3 bottomCapPt = GetBottomCapPoint();
	glm::vec2 ptAsVec2 = { bottomCapPt.x, bottomCapPt.y };
	glm::vec2 normAsVec2 = glm::normalize(glm::vec2(normal.x, normal.y));
	const float smallNum = 10e-3;
	glm::vec2 pt2Test = ptAsVec2 + (normAsVec2 * smallNum);
	if (!bottomCap.ContainsPoint(pt2Test))
	{
		normal *= -1.0f;
		outbFlipped = true;
	}
	return normal;
}

void PrismMesh::ExtrudeFromPoly2D(const Poly2D& poly, const ExtrudeParameters& params)
{
	m_Positions.clear();
	m_Indices.clear();
	m_Normals.clear();

	DesignateBottomCapStart();
	Poly2D transformedBottom = poly.GetScaledAndTranslated(params.TopCapScaleX,
		params.TopCapScaleY,
		params.TopCapOffsetX,
		params.TopCapOffsetY);

	for (const glm::vec2& pt : poly.GetPoints())
	{
		PushVert(glm::vec3(pt, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f));
	}
	glm::vec3 meshCenterForBottomCap = { 0, 0, 1000.0f };
	const std::vector<int>& polyIndices = poly.GetIndices();
	for (int ind = 0; ind < polyIndices.size(); ind += 3)
	{
		PushIndex(polyIndices[ind + 2]);
		PushIndex(polyIndices[ind + 1]);
		PushIndex(polyIndices[ind + 0]);
	}
	DesignateBottomCapEnd();

	DesignateTopCapStart();
	for (const glm::vec2& pt : poly.GetPoints())
	{
		PushVert(glm::vec3(pt, params.ExtrudeAmount), glm::vec3(0.0f, 0.0f, 1.0f));
	}
	for (int ind = 0; ind < polyIndices.size(); ind += 3)
	{
		PushIndex(polyIndices[ind + 0] + m_BottomCapVertsEnd);
		PushIndex(polyIndices[ind + 1] + m_BottomCapVertsEnd);
		PushIndex(polyIndices[ind + 2] + m_BottomCapVertsEnd);
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
		bool bFlipped = false;
		glm::vec3 normal = CalculateCorrectNormal(b1, t1, b2, transformedBottom, bFlipped);
		PushVert(b1, normal);
		PushVert(b2, normal);
		PushVert(t1, normal);
		if (bFlipped)
		{
			m_Indices.push_back(t1Start);
			m_Indices.push_back(t1Start + 1);
			m_Indices.push_back(t1Start + 2);
		}
		else
		{

			m_Indices.push_back(t1Start);
			m_Indices.push_back(t1Start + 2);
			m_Indices.push_back(t1Start + 1);
		}

		// tri2: t2 -> b2 -> t1
		size_t t2Start = m_Positions.size();
		normal = CalculateCorrectNormal(t2, b2, t1, transformedBottom, bFlipped);
		PushVert(t2, normal);
		if (bFlipped)
		{
			m_Indices.push_back(t2Start);
			m_Indices.push_back(t1Start + 2);
			m_Indices.push_back(t1Start + 1);
		}
		else
		{
			m_Indices.push_back(t2Start);
			m_Indices.push_back(t1Start + 1);
			m_Indices.push_back(t1Start + 2);
		}
	}
	DesignateSidesEnd();

	GenerateUVsAndTexture();

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

void PrismMesh::GenerateUVsAndTexture()
{
	m_UVs.resize(m_Positions.size());

	xatlas::Atlas* pAtlas = xatlas::Create();
	xatlas::MeshDecl meshDecl;
	meshDecl.vertexPositionData = m_Positions.data();
	meshDecl.vertexNormalData = m_Indices.data();
	meshDecl.indexData = m_Indices.data();
	meshDecl.indexFormat = xatlas::IndexFormat::UInt32;
	meshDecl.vertexCount = m_Positions.size();
	meshDecl.indexCount = m_Indices.size();
	meshDecl.vertexNormalStride = sizeof(glm::vec3);
	meshDecl.vertexPositionStride = sizeof(glm::vec3);
	xatlas::AddMeshError error = xatlas::AddMesh(pAtlas, meshDecl);
	xatlas::AddMeshJoin(pAtlas);

	xatlas::ChartOptions co;
	xatlas::PackOptions po;
	po.texelsPerUnit = 100;
	xatlas::Generate(pAtlas, co, po);
	xatlas::Mesh& m = pAtlas->meshes[0];

	assert(m.indexCount == (uint32_t)m_Indices.size());

	for (int i = 0; i < m.vertexCount; i++)
	{
		const xatlas::Vertex& v = m.vertexArray[i];
		m_UVs[v.xref] = glm::vec2(v.uv[0] / pAtlas->width,  v.uv[1] / pAtlas->height);
	}

	m_TextureWidth = pAtlas->width;
	m_TextureHeight = pAtlas->height;
	const uint32_t imageDataSize = pAtlas->width * pAtlas->height * 3;
	m_Texture.resize(pAtlas->atlasCount * imageDataSize);
	ASSERT(pAtlas->atlasCount == 1);
	memset(m_Texture.data(), 0, sizeof(uint8_t) * m_Texture.size());

	const uint8_t white[] = { 255, 255, 255 };
	const uint32_t faceCount = m.indexCount / 3;
	uint32_t faceFirstIndex = 0;
	for (uint32_t f = 0; f < faceCount; f++)
	{
		int32_t atlasIndex = -1;
		int verts[255][2];
		for (int i = 0; i < 3; i++)
		{
			const xatlas::Vertex& v = m.vertexArray[m.indexArray[faceFirstIndex + i]];
			atlasIndex = v.atlasIndex; // The same for every vertex in the face.
			verts[i][0] = int(v.uv[0]);
			verts[i][1] = int(v.uv[1]);
		}

		if (atlasIndex < 0)
		{
			continue; // Skip faces that weren't atlased.
		}

		uint8_t color[3];
		RandomColor(color);
		uint8_t* imageData = &m_Texture[atlasIndex * imageDataSize];

		Rasterizer::RasterizeTriangle(imageData, pAtlas->width, verts[0], verts[1], verts[2], color);
		for (uint32_t j = 0; j < 3; j++)
		{
			Rasterizer::RasterizeLine(imageData, pAtlas->width, verts[j], verts[(j + 1) % 3], white);
		}
		faceFirstIndex += 3;
	}


	for (uint32_t i = 0; i < pAtlas->atlasCount; i++) {
		char filename[256];
		snprintf(filename, sizeof(filename), "example_tris%02u.tga", i);
		stbi_write_tga(filename, pAtlas->width, pAtlas->height, 3, &m_Texture[i * imageDataSize]);
	}

	xatlas::Destroy(pAtlas);
}
