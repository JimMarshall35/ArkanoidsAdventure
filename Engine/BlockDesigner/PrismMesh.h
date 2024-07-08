#pragma once
#include <vector>
#include <glm.hpp>

class Poly2D;
struct ExtrudeParameters;

class PrismMesh : public CObject
{
public:
	virtual void Serialize(CArchive& ar) override;
	void operator=(const PrismMesh& other);
	void ExtrudeFromPoly2D(const Poly2D& poly, const ExtrudeParameters& params);
	const std::vector<glm::vec3>& GetPositions() const { return m_Positions; }
	const std::vector<glm::vec3>& GetNormals() const { return m_Normals; }
	const std::vector<glm::vec2>& GetUVs() const { return m_UVs; }
	const std::vector<int>& GetIndices() const { return m_Indices; }
	uint8_t* GetTextureData() { return m_Texture.size() > 0 ? m_Texture.data() : nullptr; }
	int GetTextureWidth() const { return m_TextureWidth; }
	int GetTextureHeight() const { return m_TextureHeight; }
	int GetTextureBytes() const { return m_Texture.size(); }
	glm::vec3 GetCentroid();
	void SetTexelsPerUnit(float newVal);
	const uint8_t* GetTexture() const { return m_Texture.data(); }
private:
	glm::vec3 CalculateCorrectNormal(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3, const Poly2D& bottomCap, bool& outbFlipped);

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

	float m_TexelsPerUnit = 100;

protected:
	DECLARE_SERIAL(PrismMesh)

};

