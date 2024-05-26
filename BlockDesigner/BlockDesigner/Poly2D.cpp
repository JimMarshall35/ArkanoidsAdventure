#include "pch.h"
#include "Poly2D.h"
#include "polypartition.h"
#include <list>

void Poly2D::PushPoint(const glm::vec2& pt)
{
	m_Points.push_back(pt);
}

void Poly2D::Clear()
{
	m_Points.clear();
	m_Indices.clear();
}

void Poly2D::Triangulate()
{
	TPPLPartition pp;
	TPPLPoly poly;
	poly.Init(m_Points.size());
	for (int i=0; i < m_Points.size(); i++)
	{
		poly[i].x = m_Points[i].x;
		poly[i].y = m_Points[i].y;
		poly[i].id = i;
	}
	poly.SetHole(false);
	poly.SetOrientation(TPPLOrientation::TPPL_ORIENTATION_CCW);
	TPPLPolyList result;

	/* triangulate by ear clipping method */
	pp.Triangulate_EC(&poly, &result);

	m_Indices.clear();
	for (const TPPLPoly& poly : result)
	{
		for (int i = 0; i < poly.GetNumPoints(); i++)
		{
			const TPPLPoint& pt = poly[i];
			m_Indices.push_back(pt.id);
		}
	}
	ASSERT(m_Indices.size() % 3 == 0);
}
