#pragma once
#include <glm/glm.hpp>
#include "EngineLib.h"
#include "InputFrontend.h"

enum class HandleType
{
	Entity,
	Mesh,
	Drawable,
	Texture,
	LogicalAxis,
	LogicalBtn
}

#define AR_STORE(ComponentPtr, Property)\
				ar->PushObj(#Property);\
					*ar << ComponentPtr##->##Property;\
				ar->PopObj();

#define AR_LOAD(ComponentPtr, Property)\
	ar->PushObj(#Property);\
		*ar >> ComponentPtr##->##Property;\
	ar->PopObj();

#define AR_STORE_ENT(ComponentPtr, Property)\
				ar->PushObj(#Property);\
					*ar << ComponentPtr##->##Property;\
					ar->HintHandleUsage(HandleType::Entity);\
				ar->PopObj();

#define AR_STORE_MESH(ComponentPtr, Property)\
				ar->PushObj(#Property);\
					*ar << ComponentPtr##->##Property;\
					ar->HintHandleUsage(HandleType::Mesh);\
				ar->PopObj();

#define AR_STORE_DRAWABLE(ComponentPtr, Property)\
				ar->PushObj(#Property);\
					*ar << ComponentPtr##->##Property;\
					ar->HintHandleUsage(HandleType::Drawable);\
				ar->PopObj();

#define AR_STORE_TEXTURE(ComponentPtr, Property)\
				ar->PushObj(#Property);\
					*ar << ComponentPtr##->##Property;\
					ar->HintHandleUsage(HandleType::Texture);\
				ar->PopObj();

#define AR_STORE_AXIS(ComponentPtr, Property)\
				ar->PushObj(#Property);\
					*ar << ComponentPtr##->##Property;\
					ar->HintHandleUsage(HandleType::LogicalAxis);\
				ar->PopObj();

#define AR_STORE_BTN(ComponentPtr, Property)\
				ar->PushObj(#Property);\
					*ar << ComponentPtr##->##Property;\
					ar->HintHandleUsage(HandleType::LogicalBtn);\
				ar->PopObj();
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

	virtual void HintHandleUsage(HandleType type) = 0;
};

