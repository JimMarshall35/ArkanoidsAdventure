#include "pch.h"
#include "ComponentInspectorPropertyGrid.h"
#include "CommonEditorServerDefines.h"
#include "StringHelpers.h"

bool TryGetXMLNodeArchiveType(ArchiveType& outType, pugi::xml_node node)
{
	using namespace pugi;
	size_t numAttributes = std::distance(node.attributes().begin(), node.attributes().end());
	switch (numAttributes)
	{
	case 1:
	{
		xml_attribute attr = node.first_attribute();
		const char* sName = attr.name();
		if (strcmp(sName, "floatVal") == 0)
		{
			outType = ArchiveType::Float;
			return true;
		}
		else if (strcmp(sName, "doubleVal") == 0)
		{
			outType = ArchiveType::Double;
			return true;
		}
		else if (strcmp(sName, "i32Val") == 0)
		{
			outType = ArchiveType::I32;
			return true;
		}
		else if (strcmp(sName, "u32Val") == 0)
		{
			outType = ArchiveType::U32;
			return true;
		}
		else if (strcmp(sName, "i64Val") == 0)
		{
			outType = ArchiveType::I64;
			return true;
		}
		else if (strcmp(sName, "u64Val") == 0)
		{
			outType = ArchiveType::U64;
			return true;
		}
		else if (strcmp(sName, "i16Val") == 0)
		{
			outType = ArchiveType::I16;
			return true;
		}
		else if (strcmp(sName, "u16Val") == 0)
		{
			outType = ArchiveType::U16;
			return true;
		}
		else if (strcmp(sName, "i8Val") == 0)
		{
			outType = ArchiveType::I8;
			return true;
		}
		else if (strcmp(sName, "u8Val") == 0)
		{
			outType = ArchiveType::U8;
			return true;
		}
		else if (strcmp(sName, "boolVal") == 0)
		{
			outType = ArchiveType::Bool;
			return true;
		}
		else if (strcmp(sName, "str") == 0)
		{
			outType = ArchiveType::String;
			return true;
		}
		else if (strcmp(sName, "ID") == 0)
		{
			outType = ArchiveType::Entity;
			return true;
		}
		break;
	}

	case 2:
	{
		xml_attribute attr = node.first_attribute();
		const char* sName = attr.name();

		if (strcmp(sName, "x") == 0)
		{
			outType = ArchiveType::Vec2;
			return true;
		}
		else if (strcmp(sName, "ix") == 0)
		{
			outType = ArchiveType::IVec2;
			return true;
		}
		break;
	}
	case 3:
	{
		xml_attribute attr = node.first_attribute();
		const char* sName = attr.name();

		if (strcmp(sName, "x") == 0)
		{
			outType = ArchiveType::Vec3;
			return true;
		}
		else if (strcmp(sName, "ix") == 0)
		{
			outType = ArchiveType::IVec3;
			return true;
		}
		break;
	}
	case 4:
		outType = ArchiveType::Vec4;
		return true;
	case 17:
		outType = ArchiveType::Mat4x4;
		return true;
	default:
		printf("idfjidjfi");

	}
	return false;
}

void CComponentInspectorPropertyGrid::OnNewComponentSelected(uint32_t entity, pugi::xml_node node)
{
	RemoveAll();
	using namespace pugi;
	m_hEntity = entity;
	m_Node = node;
	for (xml_node n : node)
	{
		auto ws = s2ws(n.name());
		// these need to be allocated with "new", the property grid control deletes them :')
		CMFCPropertyGridProperty* pProp = new CMFCPropertyGridProperty(ws.c_str());
		AddProperty(pProp);
		ArchiveType type;
		VERIFY(TryGetXMLNodeArchiveType(type, n));
		switch (type)
		{
		case ArchiveType::Float:
			if (xml_attribute attr = n.attribute("floatVal"))
			{
				COleVariant v = std::stof(attr.value());
				CMFCPropertyGridProperty* pAttrProp = new CMFCPropertyGridProperty(L"float", v);
				pAttrProp->SetValue(v);
				pProp->AddSubItem(pAttrProp);
			}
			break;
		case ArchiveType::Double:
			if (xml_attribute attr = n.attribute("doubleVal"))
			{
				COleVariant v = std::stod(attr.value());
				CMFCPropertyGridProperty* pAttrProp = new CMFCPropertyGridProperty(L"double", v);
				pAttrProp->SetValue(v);
				pProp->AddSubItem(pAttrProp);
			}
			break;
		case ArchiveType::I64:
			if (xml_attribute attr = n.attribute("i64Val"))
			{
				COleVariant v = std::stoll(attr.value());
				CMFCPropertyGridProperty* pAttrProp = new CMFCPropertyGridProperty(L"i64", v);
				pProp->AddSubItem(pAttrProp);
			}
			break;
		case ArchiveType::U64:
			if (xml_attribute attr = n.attribute("u64Val"))
			{
				COleVariant v = std::stoull(attr.value());
				CMFCPropertyGridProperty* pAttrProp = new CMFCPropertyGridProperty(L"u64", v);
				pProp->AddSubItem(pAttrProp);
			}
			break;
		case ArchiveType::I32:
			if (xml_attribute attr = n.attribute("i32Val"))
			{
				COleVariant v = std::stol(attr.value());
				CMFCPropertyGridProperty* pAttrProp = new CMFCPropertyGridProperty(L"i32", v);
				pProp->AddSubItem(pAttrProp);
			}
			break;
		case ArchiveType::U32:
			if (xml_attribute attr = n.attribute("i32Val"))
			{
				COleVariant v = std::stol(attr.value());
				CMFCPropertyGridProperty* pAttrProp = new CMFCPropertyGridProperty(L"i32", v);
				pProp->AddSubItem(pAttrProp);
			}
			break;
		case ArchiveType::I16:
			if (xml_attribute attr = n.attribute("i16Val"))
			{
				COleVariant v = std::stol(attr.value());
				CMFCPropertyGridProperty* pAttrProp = new CMFCPropertyGridProperty(L"i16", v);
				pProp->AddSubItem(pAttrProp);
			}
			break;
		case ArchiveType::U16:
			if (xml_attribute attr = n.attribute("u16Val"))
			{
				COleVariant v = std::stol(attr.value());
				CMFCPropertyGridProperty* pAttrProp = new CMFCPropertyGridProperty(L"u16", v);
				pProp->AddSubItem(pAttrProp);
			}
			break;
		case ArchiveType::I8:
			if (xml_attribute attr = n.attribute("i8Val"))
			{
				COleVariant v = std::stol(attr.value());
				CMFCPropertyGridProperty* pAttrProp = new CMFCPropertyGridProperty(L"i8", v);
				pProp->AddSubItem(pAttrProp);
			}
			break;
		case ArchiveType::U8:
			if (xml_attribute attr = n.attribute("u8Val"))
			{
				COleVariant v = std::stol(attr.value());
				CMFCPropertyGridProperty* pAttrProp = new CMFCPropertyGridProperty(L"u8", v);
				pProp->AddSubItem(pAttrProp);
			}
			break;
		case ArchiveType::Bool:
			if (xml_attribute attr = n.attribute("boolVal"))
			{
				COleVariant v;
				v.boolVal = (strcmp(attr.value(), "true") == 0 ? TRUE : FALSE);
				v.vt = VT_BOOL;
				CMFCPropertyGridProperty* pAttrProp = new CMFCPropertyGridProperty(L"bool", v);
				pProp->AddSubItem(pAttrProp);
			}
			break;
		case ArchiveType::String:
			if (xml_attribute attr = n.attribute("str"))
			{
				auto ws = s2ws(attr.value());
				COleVariant v = ws.c_str();
				CMFCPropertyGridProperty* pAttrProp = new CMFCPropertyGridProperty(L"string", v);
				pProp->AddSubItem(pAttrProp);
			}
			break;
		case ArchiveType::Entity:
			if (xml_attribute attr = n.attribute("ID"))
			{
				COleVariant v;
				v.uintVal = std::stoul(attr.value());
				v.vt = VT_UINT;
				CMFCPropertyGridProperty* pAttrProp = new CMFCPropertyGridProperty(L"Entity", v);
				pProp->AddSubItem(pAttrProp);
			}
			break;
		case ArchiveType::Vec2:
		{
			if (xml_attribute x = n.attribute("x"))
			{
				if (xml_attribute y = n.attribute("y"))
				{
					COleVariant vX = std::stof(x.value());
					COleVariant vY = std::stof(y.value());
					CMFCPropertyGridProperty* pX = new CMFCPropertyGridProperty(L"x", vX);
					CMFCPropertyGridProperty* pY = new CMFCPropertyGridProperty(L"y", vY);
					pProp->AddSubItem(pX);
					pProp->AddSubItem(pY);
				}
			}
			break;
		}
		case ArchiveType::Vec3:
		{
			if (xml_attribute x = n.attribute("x"))
			{
				if (xml_attribute y = n.attribute("y"))
				{
					if (xml_attribute z = n.attribute("z"))
					{

						COleVariant vX = std::stof(x.value());
						COleVariant vY = std::stof(y.value());
						COleVariant vZ = std::stof(z.value());
						CMFCPropertyGridProperty* pX = new CMFCPropertyGridProperty(L"x", vX);
						CMFCPropertyGridProperty* pY = new CMFCPropertyGridProperty(L"y", vY);
						CMFCPropertyGridProperty* pZ = new CMFCPropertyGridProperty(L"z", vZ);
						pProp->AddSubItem(pX);
						pProp->AddSubItem(pY);
						pProp->AddSubItem(pZ);
					}
				}
			}
			break;
		}
		case ArchiveType::IVec2:
		{
			if (xml_attribute x = n.attribute("ix"))
			{
				if (xml_attribute y = n.attribute("iy"))
				{
					COleVariant vX = std::stof(x.value());
					COleVariant vY = std::stof(y.value());
					CMFCPropertyGridProperty* pX = new CMFCPropertyGridProperty(L"ix", vX);
					CMFCPropertyGridProperty* pY = new CMFCPropertyGridProperty(L"iy", vY);
					pProp->AddSubItem(pX);
					pProp->AddSubItem(pY);
				}
			}
			break;
		}
		case ArchiveType::IVec3:
		{
			if (xml_attribute x = n.attribute("ix"))
			{
				if (xml_attribute y = n.attribute("iy"))
				{
					if (xml_attribute z = n.attribute("iz"))
					{
						COleVariant vX = std::stof(x.value());
						COleVariant vY = std::stof(y.value());
						COleVariant vZ = std::stof(z.value());
						CMFCPropertyGridProperty* pX = new CMFCPropertyGridProperty(L"ix", vX);
						CMFCPropertyGridProperty* pY = new CMFCPropertyGridProperty(L"iy", vY);
						CMFCPropertyGridProperty* pZ = new CMFCPropertyGridProperty(L"iz", vZ);
						pProp->AddSubItem(pX);
						pProp->AddSubItem(pY);
						pProp->AddSubItem(pZ);
					}
				}
			}
			break;
		}
		case ArchiveType::Vec4:
		{
			if (xml_attribute x = n.attribute("x"))
			{
				if (xml_attribute y = n.attribute("y"))
				{
					if (xml_attribute z = n.attribute("z"))
					{
						if (xml_attribute w = n.attribute("w"))
						{
							COleVariant vX = std::stof(x.value());
							COleVariant vY = std::stof(y.value());
							COleVariant vZ = std::stof(z.value());
							COleVariant vW = std::stof(w.value());
							CMFCPropertyGridProperty* pX = new CMFCPropertyGridProperty(L"x", vX);
							CMFCPropertyGridProperty* pY = new CMFCPropertyGridProperty(L"y", vY);
							CMFCPropertyGridProperty* pZ = new CMFCPropertyGridProperty(L"z", vZ);
							CMFCPropertyGridProperty* pW = new CMFCPropertyGridProperty(L"w", vZ);
							pProp->AddSubItem(pX);
							pProp->AddSubItem(pY);
							pProp->AddSubItem(pZ);
							pProp->AddSubItem(pW);
						}
					}
				}
			}
			break;
		}
		case ArchiveType::Mat4x4:
		{
			break;
		}
		}

	}
}