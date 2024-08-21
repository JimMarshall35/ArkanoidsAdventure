#pragma once
#include <glm/glm.hpp>
#include "EngineLib.h"

class IArchive
{
public:
	virtual bool IsStoring() = 0;
	virtual void operator<<(float f) = 0;
	virtual void operator<<(double d) = 0;
	virtual void operator<<(i32 i) = 0;
	virtual void operator<<(u32 i) = 0;
	virtual void operator<<(i64 i) = 0;
	virtual void operator<<(u64 i) = 0;
	virtual void operator<<(i16 i) = 0;
	virtual void operator<<(u16 i) = 0;
	virtual void operator<<(u8 i) = 0;
	virtual void operator<<(i8 i) = 0;
	virtual void operator<<(bool b) = 0;
	virtual void operator<<(const EString& s) = 0;
	virtual void operator<<(const glm::vec2& v) = 0;
	virtual void operator<<(const glm::vec3& v) = 0;
	virtual void operator<<(const glm::vec4& v) = 0;
	virtual void operator<<(const glm::ivec2& v) = 0;
	virtual void operator<<(const glm::ivec3& v) = 0;
	virtual void operator<<(const glm::mat4x4& v) = 0;
	virtual void operator<<(Entity e) = 0;


	virtual void operator>>(float& f) = 0;
	virtual void operator>>(double& d) = 0;
	virtual void operator>>(i32& i) = 0;
	virtual void operator>>(u32& i) = 0;
	virtual void operator>>(i64& i) = 0;
	virtual void operator>>(u64& i) = 0;
	virtual void operator>>(i16& i) = 0;
	virtual void operator>>(u16& i) = 0;
	virtual void operator>>(u8& i) = 0;
	virtual void operator>>(i8& i) = 0;
	virtual void operator>>(bool& b) = 0;
	virtual void operator>>(EString& s) = 0;
	virtual void operator>>(glm::vec2& v) = 0;
	virtual void operator>>(glm::vec3& v) = 0;
	virtual void operator>>(glm::vec4& v) = 0;
	virtual void operator>>(glm::ivec2& v) = 0;
	virtual void operator>>(glm::ivec3& v) = 0;
	virtual void operator>>(glm::mat4x4& v) = 0;
	virtual void operator>>(Entity& e) = 0;

	virtual bool PushObj(const char* name) = 0;
	virtual void PopObj() = 0;
	virtual void Store() = 0;

	virtual int CountChildren() = 0;
	virtual void PushChild(int c) = 0;
};