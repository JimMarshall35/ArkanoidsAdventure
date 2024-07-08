#pragma once
#include <glm.hpp>
#include <vector>

class Poly2D : public CObject
{
public:
	Poly2D() {}
	Poly2D(const Poly2D& other);
	void operator=(const Poly2D& other);
	void PushPoint(const glm::vec2& pt);
	void Clear();
	const std::vector<glm::vec2>& GetPoints() const { return m_Points; }
	const std::vector<int>& GetIndices() const { return m_Indices; }
	bool HasPoints() const { return !m_Points.empty(); }
	void Triangulate();
	bool ContainsPoint(glm::vec2 pt) const;
	Poly2D GetScaledAndTranslated(float scaleX, float scaleY, float offsetX, float offsetY) const;
	virtual void Serialize(CArchive& ar) override;
private:
	std::vector<glm::vec2> m_Points;
	std::vector<int> m_Indices;
protected:
	DECLARE_SERIAL(Poly2D)
};

