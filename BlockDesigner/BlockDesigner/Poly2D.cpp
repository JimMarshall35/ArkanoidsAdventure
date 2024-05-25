#include "pch.h"
#include "Poly2D.h"

void Poly2D::PushPoint(const glm::vec2& pt)
{
	m_Points.push_back(pt);
}

void Poly2D::Clear()
{
	m_Points.clear();
	m_Indices.clear();
}
