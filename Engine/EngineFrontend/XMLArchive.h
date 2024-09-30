#pragma once
#include "IArchive.h"
#include "pugixml.hpp"
#include "ExportMacro.h"
#include <iostream>

class ENGINE_FRONTEND_API XMLArchive : public IArchive
{
public:
	XMLArchive(const char* path, bool bStoring);
	XMLArchive(const char* xml);
	pugi::xml_node GetRoot() { return m_Doc.root(); }
	~XMLArchive();
public:
	// Inherited via IArchive
	virtual bool IsStoring() override;
	virtual void operator<<(float f) override;
	virtual void operator<<(double d) override;
	virtual void operator<<(i32 i) override;
	virtual void operator<<(u32 i) override;
	virtual void operator<<(i64 i) override;
	virtual void operator<<(u64 i) override;
	virtual void operator<<(i16 i) override;
	virtual void operator<<(u16 i) override;
	virtual void operator<<(u8 i) override;
	virtual void operator<<(i8 i) override;
	virtual void operator<<(bool b) override;
	virtual void operator<<(const EString& s) override;
	virtual void operator<<(const glm::vec2& v) override;
	virtual void operator<<(const glm::vec3& v) override;
	virtual void operator<<(const glm::vec4& v) override;
	virtual void operator<<(const glm::ivec2& v) override;
	virtual void operator<<(const glm::ivec3& v) override;
	virtual void operator<<(const glm::mat4x4& v) override;/*
	virtual void operator<<(In::HLogicalAxis axis) override;
	virtual void operator<<(In::HLogicalBtn axis) override;*/

	virtual void operator>>(float& f) override;
	virtual void operator>>(double& d) override;
	virtual void operator>>(i32& i) override;
	virtual void operator>>(u32& i) override;
	virtual void operator>>(i64& i) override;
	virtual void operator>>(u64& i) override;
	virtual void operator>>(i16& i) override;
	virtual void operator>>(u16& i) override;
	virtual void operator>>(u8& i) override;
	virtual void operator>>(i8& i) override;
	virtual void operator>>(bool& b) override;
	virtual void operator>>(EString& s) override;
	virtual void operator>>(glm::vec2& v) override;
	virtual void operator>>(glm::vec3& v) override;
	virtual void operator>>(glm::vec4& v) override;
	virtual void operator>>(glm::ivec2& v) override;
	virtual void operator>>(glm::ivec3& v) override;
	virtual void operator>>(glm::mat4x4& v) override;/*
	virtual void operator>>(In::HLogicalAxis& axis) override;
	virtual void operator>>(In::HLogicalBtn& axis) override;*/

	/*template <typename T, size_t size>
	void operator<<(std::array<T, size>& arr)
	{
		for (const T& : arr)
		{

		}
	}*/
	virtual bool PushObj(const char* name) override;
	virtual void PopObj() override;
	virtual void Store() override;
	EString AsString();
	virtual int CountChildren() override;
	virtual void PushChild(int c) override;

	virtual void HintHandleUsage(HandleType type) override;

	void Test() { m_Doc.print(std::cout); }
private:
	pugi::xml_node TopNode() const;
private:
	pugi::xml_document m_Doc;
	pugi::xml_node m_Root;
	EVec<pugi::xml_node> m_NodeStack;
	bool m_bStoring;
	EString m_strPath;

	// Inherited via IArchive
	virtual void operator<<(Entity e) override;
	virtual void operator>>(Entity& e) override;

	
};

