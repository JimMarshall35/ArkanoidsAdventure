#pragma once
#include <vector>
#include <glm.hpp>

class Poly2D;
struct ExtrudeParameters;

class PrismMesh
{
public:
	void ExtrudeFromPoly2D(const Poly2D& poly, const ExtrudeParameters& params);

	const std::vector<glm::vec3>& GetPositions() const { return m_Positions; }
	const std::vector<glm::vec3>& GetNormals() const { return m_Normals; }
	const std::vector<glm::vec2>& GetUVs() const { return m_UVs; }
	const std::vector<int>& GetIndices() const { return m_Indices; }
	uint8_t* GetTextureData() { return m_Texture.size() > 0 ? m_Texture.data() : nullptr; }
	int GetTextureWidth() { return m_TextureWidth; }
	int GetTextureHeight() { return m_TextureHeight; }
	int GetTextureBytes() { return m_Texture.size(); }

private:
	void PushVert(const glm::vec3& pos, const glm::vec3& normal);
	void PushIndex(int index);

	void DesignateBottomCapStart();
	void DesignateBottomCapEnd();
	
	void DesignateTopCapStart();
	void DesignateTopCapEnd();
	
	void DesignateSidesStart();
	void DesignateSidesEnd();

	void ScaleAndTranslateTopCap(const ExtrudeParameters& params);
	void ScaleAndTranslateBottomCap(const ExtrudeParameters& params);

	void GenerateUVsAndTexture();
private:
	std::vector<glm::vec3> m_Positions;
	std::vector<glm::vec3> m_Normals;
	std::vector<glm::vec2> m_UVs;
	std::vector<int> m_Indices;
	
	std::vector<uint8_t> m_Texture;
	int m_TextureWidth = 0;
	int m_TextureHeight = 0;

	size_t m_BottomCapVertsStart = 0;
	size_t m_BottomCapVertsEnd = 0;

	size_t m_BottomCapIndsStart = 0;
	size_t m_BottomCapIndsEnd = 0;

	size_t m_TopCapVertsStart = 0;
	size_t m_TopCapVertsEnd = 0;

	size_t m_TopCapIndsStart = 0;
	size_t m_TopCapIndsEnd = 0;

	size_t m_SidesVertsStart = 0;
	size_t m_SidesVertsEnd = 0;

	size_t m_SidesIndsStart = 0;
	size_t m_SidesIndsEnd = 0;
};

