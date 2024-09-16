#include "XMLArchive.h"
#include "ComponentReg.h"
#include "FrontendError.h"
#include <stdio.h>
#include <sstream>

XMLArchive::XMLArchive(const char* path, bool bStoring)
    :m_bStoring(bStoring), m_strPath(path)
{
    if (bStoring)
    {
        m_Root = m_Doc.append_child("Scene");
    }
    else
    {
        m_Doc.load_file(path);
        m_Root = m_Doc.child("Scene");
    }
}

XMLArchive::~XMLArchive()
{
}

bool XMLArchive::IsStoring()
{
    return m_bStoring;
}

void XMLArchive::operator<<(float f)
{
    EAssert(m_bStoring);
    pugi::xml_node n = TopNode();
    pugi::xml_attribute a = n.append_attribute("floatVal");
    a.set_value(f);
}

void XMLArchive::operator<<(double d)
{
    EAssert(m_bStoring);
    pugi::xml_node n = TopNode();
    pugi::xml_attribute a = n.append_attribute("doubleVal");
    a.set_value(d);
}

void XMLArchive::operator<<(i32 i)
{
    EAssert(m_bStoring);
    pugi::xml_node n = TopNode();
    pugi::xml_attribute a = n.append_attribute("i32Val");
    a.set_value(i);
}

void XMLArchive::operator<<(u32 i)
{
    EAssert(m_bStoring);
    pugi::xml_node n = TopNode();
    pugi::xml_attribute a = n.append_attribute("u32Val");
    a.set_value(i);
}

void XMLArchive::operator<<(i64 i)
{
    EAssert(m_bStoring);
    pugi::xml_node n = TopNode();
    pugi::xml_attribute a = n.append_attribute("i64Val");
    a.set_value((i64)i);
}

void XMLArchive::operator<<(u64 i)
{
    EAssert(m_bStoring);
    pugi::xml_node n = TopNode();
    pugi::xml_attribute a = n.append_attribute("u64Val");
    a.set_value((u64)i);
}

void XMLArchive::operator<<(i16 i)
{
    EAssert(m_bStoring);
    pugi::xml_node n = TopNode();
    pugi::xml_attribute a = n.append_attribute("i16Val");
    a.set_value((int)i);
}

void XMLArchive::operator<<(u16 i)
{
    EAssert(m_bStoring);
    pugi::xml_node n = TopNode();
    pugi::xml_attribute a = n.append_attribute("u16Val");
    a.set_value((unsigned int)i);
}

void XMLArchive::operator<<(u8 i)
{
    EAssert(m_bStoring);
    pugi::xml_node n = TopNode();
    pugi::xml_attribute a = n.append_attribute("u8Val");
    a.set_value((unsigned int)i);
}

void XMLArchive::operator<<(i8 i)
{
    EAssert(m_bStoring);
    pugi::xml_node n = TopNode();
    pugi::xml_attribute a = n.append_attribute("i8Val");
    a.set_value((int)i);
}

void XMLArchive::operator<<(bool b)
{
    EAssert(m_bStoring);
    pugi::xml_node n = TopNode();
    pugi::xml_attribute a = n.append_attribute("boolVal");
    a.set_value(b);
}





void XMLArchive::operator>>(float& f)
{
    EAssert(!m_bStoring);
    pugi::xml_node n = TopNode();
    pugi::xml_attribute a;
    a = n.attribute("floatVal");
    f = a.as_float();
}

void XMLArchive::operator>>(double& d)
{
    EAssert(!m_bStoring);
    pugi::xml_node n = TopNode();
    pugi::xml_attribute a;
    a = n.attribute("doubleVal");
    d = a.as_double();
}

void XMLArchive::operator>>(i32& i)
{
    EAssert(!m_bStoring);
    pugi::xml_node n = TopNode();
    pugi::xml_attribute a;
    a = n.attribute("i32Val");
    i = a.as_int();
}

void XMLArchive::operator>>(u32& i)
{
    EAssert(!m_bStoring);
    pugi::xml_node n = TopNode();
    pugi::xml_attribute a;
    a = n.attribute("u32Val");
    i = a.as_uint();
}

void XMLArchive::operator>>(i64& i)
{
    EAssert(!m_bStoring);
    pugi::xml_node n = TopNode();
    pugi::xml_attribute a;
    a = n.attribute("i64Val");
    i = a.as_llong();
}

void XMLArchive::operator>>(u64& i)
{
    EAssert(!m_bStoring);
    pugi::xml_node n = TopNode();
    pugi::xml_attribute a;
    a = n.attribute("u64Val");
    i = a.as_ullong();
}

void XMLArchive::operator>>(i16& i)
{
    EAssert(!m_bStoring);
    pugi::xml_node n = TopNode();
    pugi::xml_attribute a;
    a = n.attribute("i16Val");
    i = (i16)a.as_int();
}

void XMLArchive::operator>>(u16& i)
{
    EAssert(!m_bStoring);
    pugi::xml_node n = TopNode();
    pugi::xml_attribute a;
    a = n.attribute("u16Val");
    i = (u16)a.as_uint();
}

void XMLArchive::operator>>(u8& i)
{
    EAssert(!m_bStoring);
    pugi::xml_node n = TopNode();
    pugi::xml_attribute a;
    a = n.attribute("u8Val");
    i = (u8)a.as_uint();
}

void XMLArchive::operator>>(i8& i)
{
    EAssert(!m_bStoring);
    pugi::xml_node n = TopNode();
    pugi::xml_attribute a;
    a = n.attribute("i8Val");
    i = (i8)a.as_int();
}

void XMLArchive::operator>>(bool& b)
{
    EAssert(!m_bStoring);
    pugi::xml_node n = TopNode();
    pugi::xml_attribute a;
    a = n.attribute("boolVal");
    b = a.as_bool();
}

void XMLArchive::operator>>(EString& s)
{
    EAssert(!m_bStoring);
    pugi::xml_node n = TopNode();
    pugi::xml_attribute a;
    a = n.attribute("str");
    s = a.as_string();

}

void XMLArchive::operator<<(const EString& s)
{
    EAssert(m_bStoring);
    pugi::xml_node n = TopNode();
    pugi::xml_attribute a = n.append_attribute("str");
    a.set_value(s.c_str());
}

void XMLArchive::operator>>(glm::vec2& v)
{
    EAssert(!m_bStoring);
    pugi::xml_node n = TopNode();
    pugi::xml_attribute a;
    a = n.attribute("x");
    v.x = a.as_float();
    a = n.attribute("y");
}

void XMLArchive::operator<<(const glm::vec2& v)
{
}

void XMLArchive::operator>>(glm::vec3& v)
{
    EAssert(!m_bStoring);
    pugi::xml_node n = TopNode();
    pugi::xml_attribute a;
    a = n.attribute("x");
    v.x = a.as_float();
    a = n.attribute("y");
    v.y = a.as_float();
    a = n.attribute("z");
    v.z = a.as_float();

}


void XMLArchive::operator<<(const glm::vec3& v)
{
    EAssert(m_bStoring);
    pugi::xml_node n = TopNode();
    pugi::xml_attribute a;
    a = n.append_attribute("x");
    a.set_value(v.x);
    a = n.append_attribute("y");
    a.set_value(v.y);
    a = n.append_attribute("z");
    a.set_value(v.z);
}


void XMLArchive::operator>>(glm::vec4& v)
{
    EAssert(!m_bStoring);
    pugi::xml_node n = TopNode();
    pugi::xml_attribute a;
    a = n.attribute("x");
    v.x = a.as_float();
    a = n.attribute("y");
    v.y = a.as_float();
    a = n.attribute("z");
    v.z = a.as_float();
    a = n.attribute("w");
    v.w = a.as_float();
}

void XMLArchive::operator<<(const glm::vec4& v)
{
    EAssert(m_bStoring);
    pugi::xml_node n = TopNode();
    pugi::xml_attribute a;
    a = n.append_attribute("x");
    a.set_value(v.x);
    a = n.append_attribute("y");
    a.set_value(v.y);
    a = n.append_attribute("z");
    a.set_value(v.z);
    a = n.append_attribute("w");
    a.set_value(v.w);
}



void XMLArchive::operator>>(glm::ivec2& v)
{
    EAssert(!m_bStoring);
    pugi::xml_node n = TopNode();
    pugi::xml_attribute a;
    a = n.attribute("ix");
    v.x = a.as_int();
    a = n.attribute("iy");
    v.y = a.as_int();
}

void XMLArchive::operator<<(const glm::ivec2& v)
{
    EAssert(m_bStoring);
    pugi::xml_node n = TopNode();
    pugi::xml_attribute a;
    a = n.append_attribute("ix");
    a.set_value(v.x);
    a = n.append_attribute("iy");
    a.set_value(v.y);

}
void XMLArchive::operator>>(glm::ivec3& v)
{
    EAssert(!m_bStoring);
    pugi::xml_node n = TopNode();
    pugi::xml_attribute a;
    a = n.attribute("ix");
    v.x = a.as_int(); 
    a = n.attribute("iy");
    v.y = a.as_int();
    a = n.attribute("iz");
    v.z = a.as_int();
}

void XMLArchive::operator<<(const glm::ivec3& v)
{
    EAssert(m_bStoring);
    pugi::xml_node n = TopNode();
    pugi::xml_attribute a;
    a = n.append_attribute("ix");
    a.set_value(v.x);
    a = n.append_attribute("iy");
    a.set_value(v.y);
    a = n.append_attribute("iz");
    a.set_value(v.z);
}


void XMLArchive::operator<<(const glm::mat4x4& v)
{
    EAssert(m_bStoring);
    pugi::xml_node node = TopNode();
    pugi::xml_attribute attr = node.append_attribute("v0v0");  attr.set_value(v[0][0]);

    attr = node.append_attribute("v0v0");attr.set_value(v[0][0]);
    attr = node.append_attribute("v0v1");attr.set_value(v[0][1]);
    attr = node.append_attribute("v0v2");attr.set_value(v[0][2]);
    attr = node.append_attribute("v0v3");attr.set_value(v[0][3]);

    attr = node.append_attribute("v1v0");attr.set_value(v[1][0]);
    attr = node.append_attribute("v1v1");attr.set_value(v[1][1]);
    attr = node.append_attribute("v1v2");attr.set_value(v[1][2]);
    attr = node.append_attribute("v1v3");attr.set_value(v[1][3]);

    attr = node.append_attribute("v2v0");attr.set_value(v[2][0]);
    attr = node.append_attribute("v2v1");attr.set_value(v[2][1]);
    attr = node.append_attribute("v2v2");attr.set_value(v[2][2]);
    attr = node.append_attribute("v2v3");attr.set_value(v[2][3]);

    attr = node.append_attribute("v3v0");attr.set_value(v[3][0]);
    attr = node.append_attribute("v3v1");attr.set_value(v[3][1]);
    attr = node.append_attribute("v3v2");attr.set_value(v[3][2]);
    attr = node.append_attribute("v3v3");attr.set_value(v[3][3]);
}

void XMLArchive::operator>>(glm::mat4x4& v)
{
    EAssert(!m_bStoring);
    pugi::xml_node node = TopNode();
    v[0][0] = node.attribute("v0v0").as_float();
    v[0][1] = node.attribute("v0v1").as_float();
    v[0][2] = node.attribute("v0v2").as_float();
    v[0][3] = node.attribute("v0v3").as_float();

    v[1][0] = node.attribute("v1v0").as_float();
    v[1][1] = node.attribute("v1v1").as_float();
    v[1][2] = node.attribute("v1v2").as_float();
    v[1][3] = node.attribute("v1v3").as_float();

    v[2][0] = node.attribute("v2v0").as_float();
    v[2][1] = node.attribute("v2v1").as_float();
    v[2][2] = node.attribute("v2v2").as_float();
    v[2][3] = node.attribute("v2v3").as_float();

    v[3][0] = node.attribute("v3v0").as_float();
    v[3][1] = node.attribute("v3v1").as_float();
    v[3][2] = node.attribute("v3v2").as_float();
    v[3][3] = node.attribute("v3v3").as_float();
}

bool XMLArchive::PushObj(const char* name)
{
    if (m_bStoring)
    {
        m_NodeStack.push_back(TopNode().append_child(name));
        return true;
    }
    else
    {
        //Autolist<Comp::ComponentMeta>* pList = Autolist<Comp::ComponentMeta>::GetHead();
        auto itr = TopNode().children();
        for (pugi::xml_node node : itr)
        {
            if (strcmp(node.name(), name) == 0)
            {
                m_NodeStack.push_back(node);
                return true;
            }
        }
        return false;
    }
}

void XMLArchive::PopObj()
{
    m_NodeStack.pop_back();
}

void XMLArchive::Store()
{
    m_Doc.save_file(m_strPath.c_str());
}

EString XMLArchive::AsString()
{
    std::ostringstream ss;
    m_Doc.save(ss);
    return ss.str();
}

pugi::xml_node XMLArchive::TopNode() const
{
    if (m_NodeStack.empty())
    {
        return m_Root;
    }
    else
    {
        return m_NodeStack[m_NodeStack.size() - 1];
    }
}

void XMLArchive::operator<<(Entity e)
{
    pugi::xml_attribute a = TopNode().append_attribute("ID");
    a.set_value((ENTT_ID_TYPE)e);
}

void XMLArchive::operator>>(Entity& e)
{
    pugi::xml_attribute a = TopNode().attribute("ID");
    e = (Entity)a.as_uint();
}

int XMLArchive::CountChildren()
{
    auto t = TopNode().children();
    int size = 0;

    for (auto it = t.begin(); it != t.end(); it++)
    {
        size++;
    }
    return size;
}

void XMLArchive::PushChild(int c)
{
    int i=0;
    for (const pugi::xml_node n : TopNode().children())
    {
        if (i++ == c)
        {
            m_NodeStack.push_back(n);
            return;
        }
    }
}
