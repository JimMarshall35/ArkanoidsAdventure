#include "pch.h"
#include "Poly2D.h"
#include "polypartition.h"
#include "MiscFunctions.h"
#include <list>
#include <set>

IMPLEMENT_SERIAL(Poly2D, CObject, 1)

Poly2D::Poly2D(const Poly2D& other)
{
	operator=(other);
}

void Poly2D::operator=(const Poly2D& other)
{
	m_Points = other.m_Points;
	m_Indices = other.m_Indices;
}

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
	//bool test = ContainsPoint({ 0,0 });
}

bool Poly2D::ContainsPoint(glm::vec2 pt) const
{
	LineSegment2D lineFromOutside({ INFINITY, INFINITY }, pt);
	std::vector<glm::vec2> intersects;
	auto GetNextIndex = [this](int thisI) -> int
	{
		if (thisI + 1 >= m_Points.size())
		{
			return 0;
		}
		return thisI + 1;
	};	
	auto intersectsContains = [&intersects](const glm::vec2& pt)
	{
		for (const glm::vec2& inter : intersects)
		{
			if (AreSame(inter.x, pt.x) && AreSame(inter.y, pt.y))
			{
				return true;
			}
		}
		return  false;
	};

	int intersectCount = 0;

	for (int i = 0; i < m_Points.size(); i++)
	{
		LineSegment2D line(m_Points[i], m_Points[GetNextIndex(i)]);
		glm::vec2 pt;
		if (LineSegment2D::Intersects(lineFromOutside, line, &pt))
		{
			if (!intersectsContains(pt))
			{
				intersects.push_back(pt);
				++intersectCount;
			}
		}
	}
	return (intersectCount % 2) != 0;
}

Poly2D Poly2D::GetScaledAndTranslated(float scaleX, float scaleY, float offsetX, float offsetY) const
{
	Poly2D r = *this;
	float xTotal = 0;
	float yTotal = 0;
	int num = 0;
	for (const glm::vec2& v : r.m_Points)
	{
		xTotal += v.x;
		yTotal += v.y;
		num++;
	}
	glm::vec2 centroid = { xTotal / (float)num, yTotal / (float)num };
	for (glm::vec2& v : r.m_Points)
	{
		v += -centroid;
		v.x *= scaleX;
		v.y *= scaleY;
		v += centroid;
		v += glm::vec2{ offsetX, offsetY };
	}
	return r;
}

void Poly2D::Serialize(CArchive& ar)
{
	const static WORD sVersion = 1;
	if (ar.IsStoring())
	{
		ar << sVersion;
		ar << m_Indices.size();
		for (int i : m_Indices)
		{
			ar << i;
		}
		ar << m_Points.size();
		for (const glm::vec2& v2 : m_Points)
		{
			ar << v2.x;
			ar << v2.y;
		}
	}
	else
	{
		WORD ver;
		ar >> ver;
		switch (ver)
		{
		case 1:
		{
			size_t arraySize;
			ar >> arraySize;
			m_Indices.resize(arraySize);
			for (int i = 0; i < arraySize; i++)
			{
				ar >> m_Indices[i];
			}
			ar >> arraySize;
			m_Points.resize(arraySize);
			for (int i = 0; i < arraySize; i++)
			{
				ar >> m_Points[i].x;
				ar >> m_Points[i].y;
			}
			break;
		}
		default:
			break;
		}
	}
}
