#pragma once
#include <glm.hpp>
#include <vector>

class Poly2D
{
public:
	void PushPoint(const glm::vec2& pt);
	void Clear();
	const std::vector<glm::vec2>& GetPoints() const { return m_Points; }
	const std::vector<int>& GetIndices() const { return m_Indices; }
	bool HasPoints() const { return !m_Points.empty(); }
	void Triangulate();
	bool ContainsPoint(glm::vec2 pt) const;
private:
	std::vector<glm::vec2> m_Points;
	std::vector<int> m_Indices;
};

