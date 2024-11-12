#pragma once
#define DEFAULT_PORT "666"
#define EDITOR_GUI_RECIEVE_QUEUE_SIZE 128
#define EDITOR_GUI_SEND_QUEUE_SIZE 128
#define EDITOR_GUI_RECIEVE_MSG_BUFFER_SIZE 200000

#define ENGINE_RECIEVE_MSG_BUFFER_SIZE 200000
#define ENGINE_RECIEVE_QUEUE_SIZE 128
#define ENGINE_SEND_QUEUE_SIZE 128

enum class ArchiveType
{
	Float,
	Double,
	I64,
	U64,
	I32,
	U32,
	I16,
	U16,
	I8,
	U8,
	Bool,
	String,
	Vec2,
	Vec3,
	IVec2,
	IVec3,
	Vec4,
	Mat4x4,
	Entity,
	Texture,
	Mesh,
	Pipeline,
	Button,
	Axis,
	Drawable,
	Array
};
