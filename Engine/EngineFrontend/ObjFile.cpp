
#include "OBJ/ObjFile.h"
#include <fstream>
#include <cassert>
#include <stdlib.h>     /* atof */
#include "..\..\include\OBJ\ObjFile.h"


bool TryReadFileLines(const char* path, std::vector<std::string>& lines)
{
	std::ifstream fs(path);
	if (fs.is_open())
	{
		std::string line;
		while (std::getline(fs, line)) {
			lines.push_back(line);
		}
		fs.close();
		
		return lines.size() > 0;
	}
	return false;
}

void StripCommentLines(std::vector<std::string>& lines)
{
	std::vector<std::string> newVector;
	auto firstNonWhitespaceCharacter = [](const char* inString) -> char
	{
		const char* onChar = inString;
		while (*onChar != '\0' && (*onChar == ' ' || *onChar == '\t'))
		{
			++onChar;
		}
		return *onChar;
	};
	for (const std::string& line : lines)
	{
		if (line.size() != 0 && firstNonWhitespaceCharacter(line.c_str()) != '#')
		{
			newVector.push_back(line);
		}
	}
	lines = newVector;
}

ObjFile::ObjFile(const char* path)
	: m_filePath(path)
{
	if (TryReadFileLines(path, m_fileLines))
	{
		StripCommentLines(m_fileLines);
		ParseLines();
	}
}

void Tokenize(const char* inString, std::vector<std::string>& outTokens)
{
	outTokens.clear();
	const char* onChar = inString;
	std::string s = "";
	while (*onChar != '\0')
	{
		char c = *onChar++;
		if (c != ' ' && c != '\t')
		{
			s += c;
			if (*onChar == ' ' || *onChar == '\t')
			{
				outTokens.push_back(s);
				s = "";
			}
		}
	}
	if (s.length() > 0)
	{
		outTokens.push_back(s);
	}
}


void ObjFile::ParseLines()
{
	static std::map<std::string, OBJFileLine> s_tokenToEnum =
	{
		{"v", OBJFileLine::VPos},
		{"vt", OBJFileLine::VTexCoords},
		{"vn", OBJFileLine::VNormal },
		{"vp", OBJFileLine::VParamSpace},
		{"f", OBJFileLine::Face},
		{"l", OBJFileLine::Line},
		{"mtllib", OBJFileLine::IncludeMtlLib},
		{"usemtl", OBJFileLine::UseMtl},
		{"o", OBJFileLine::ObjectName},
		{"g", OBJFileLine::GroupName},
		{"s", OBJFileLine::SmoothShading}
	};
	Group* activeGroup = nullptr;
	Object* activeObject = nullptr;

	for (const std::string& line : m_fileLines)
	{
		std::vector<std::string> tokens;
		Tokenize(line.c_str(), tokens);
		assert(tokens.size() > 0);
		assert(s_tokenToEnum.find(tokens[0]) != s_tokenToEnum.end());
		std::string firstToken = tokens[0];
		OBJFileLine lineType = s_tokenToEnum[firstToken];
		switch (lineType)
		{
		case OBJFileLine::VParamSpace:
		case OBJFileLine::VNormal:
		case OBJFileLine::VPos:
			assert(activeObject);
			activeObject->vertexInfo[(int)lineType].push_back(vec3(atof(tokens[1].c_str()), atof(tokens[2].c_str()), atof(tokens[3].c_str())));
			break;
		case OBJFileLine::VTexCoords:
			activeObject->vertexInfo[(int)lineType].push_back(vec3(atof(tokens[1].c_str()), atof(tokens[2].c_str()), 0.0f));
			break;
		case OBJFileLine::Face:
			assert(activeGroup);
			for (int i = 1; i < tokens.size(); i++)
			{
				activeGroup->indices.push_back(FaceArgumentToIndex(tokens[i].c_str()));
			}
			break;
		case OBJFileLine::Line:
			break;
		case OBJFileLine::IncludeMtlLib:
			ParseMtlFile(tokens[1].c_str());
			break;
		case OBJFileLine::UseMtl:
			assert(m_materialMap.find(tokens[1]) != m_materialMap.end());
			activeGroup->material = &m_materialMap[tokens[1]];
			break;
		case OBJFileLine::ObjectName:
			{
				Object obj;
				obj.name = tokens[1];
				m_objects.push_back(obj);
				activeObject = &m_objects[m_objects.size() - 1];
			}
			break;
		case OBJFileLine::GroupName:
			{
				assert(activeObject);
				activeObject->groups[tokens[1]] = Group{};
				activeGroup = &activeObject->groups[tokens[1]];
			}
			break;
		case OBJFileLine::SmoothShading:
			assert(activeGroup);
			activeGroup->bSmoothShading = true;
			break;
		}
		
	}
}

void GetAbsolutePath(char* absPathBuffer, const char* relativePath)
{
	size_t absLen = strlen(absPathBuffer);
	char* endChar = &absPathBuffer[absLen];
	while (*endChar != '/' && *endChar != '\\')
	{
		--endChar;
	}
	++endChar;
	strcpy(endChar, relativePath);
}

void ObjFile::ParseMtlFile(const char* mtlRelativePath)
{
	static std::map<std::string, MTLFileLine> s_tokenToEnum =
	{
		{"newmtl", MTLFileLine::NewMtl},
		{"Ka", MTLFileLine::Ka},
		{"Kd", MTLFileLine::Kd},
		{"Ks", MTLFileLine::Ks},
		{"Ns", MTLFileLine::Ns},
		{"d", MTLFileLine::d},
		{"Tr", MTLFileLine::Tr},
		{"Tf", MTLFileLine::Tf},
		{"Ni", MTLFileLine::Ni},
		{"illum", MTLFileLine::illum},
		{"map_Ka", MTLFileLine::map_Ka},
		{"map_Kd", MTLFileLine::map_Kd},
		{"map_Ks", MTLFileLine::map_Ks},
		{"map_Ns", MTLFileLine::map_Ns},
		{"map_dmap_d", MTLFileLine::map_d},
		{"map_bump", MTLFileLine::map_bump},// # some implementations use 'map_bump' instead of 'bump' below
		{"bump", MTLFileLine::map_bump},
		{"disp", MTLFileLine::disp},
		{"decal", MTLFileLine::decal},
		{"refl", MTLFileLine::refl},

	};

	static char s_buf[512];
	strcpy(s_buf, m_filePath.c_str());
	GetAbsolutePath(s_buf, mtlRelativePath);
	std::vector<std::string> mtlLines;
	if (TryReadFileLines(s_buf, mtlLines))
	{
		Material* mat = nullptr;
		StripCommentLines(mtlLines);
		for (const std::string& line : mtlLines)
		{
			std::vector<std::string> tokens;
			Tokenize(line.c_str(), tokens);
			assert(tokens.size() > 0);
			assert(s_tokenToEnum.find(tokens[0]) != s_tokenToEnum.end());
			MTLFileLine lineType = s_tokenToEnum[tokens[0]];
			switch (lineType)
			{
			case MTLFileLine::NewMtl:
				assert(m_materialMap.find(tokens[1]) == m_materialMap.end());
				mat = &m_materialMap[tokens[1]];
				mat->name = tokens[1];
				break;
			case MTLFileLine::Ka:
				mat->Ka = vec3{ atof(tokens[1].c_str()),atof(tokens[2].c_str()),atof(tokens[3].c_str()) };
				break;
			case MTLFileLine::Kd:
				mat->Kd = vec3{ atof(tokens[1].c_str()),atof(tokens[2].c_str()),atof(tokens[3].c_str()) };
				break;
			case MTLFileLine::Ks:
				mat->Ks = vec3{ atof(tokens[1].c_str()),atof(tokens[2].c_str()),atof(tokens[3].c_str()) };
				break;
			case MTLFileLine::Ns:
				mat->Ns = atof(tokens[1].c_str());
				break;
			case MTLFileLine::d:
				mat->d = atof(tokens[1].c_str());
				break;
			case MTLFileLine::Tr:
				mat->d = -(atof(tokens[1].c_str())-1.0f);
				break;
			case MTLFileLine::Tf:
				mat->Tf = vec3{ atof(tokens[1].c_str()),atof(tokens[2].c_str()),atof(tokens[3].c_str()) };
				break;
			case MTLFileLine::Ni:
				mat->Ni = atof(tokens[1].c_str());
				break;
			case MTLFileLine::illum:
				mat->illum = (Illum)atoi(tokens[1].c_str());
				break;
			case MTLFileLine::map_Ka:
				mat->texturesMapPaths[(int)TextureMapType::map_Ka] = tokens[1];
				break;
			case MTLFileLine::map_Kd:
				mat->texturesMapPaths[(int)TextureMapType::map_Kd] = tokens[1];
				break;
			case MTLFileLine::map_Ks:
				mat->texturesMapPaths[(int)TextureMapType::map_Ks] = tokens[1];
				break;
			case MTLFileLine::map_Ns:
				mat->texturesMapPaths[(int)TextureMapType::map_Ns] = tokens[1];
				break;
			case MTLFileLine::map_d:
				mat->texturesMapPaths[(int)TextureMapType::map_d] = tokens[1];
				break;
			case MTLFileLine::map_bump:
				mat->texturesMapPaths[(int)TextureMapType::map_bump] = tokens[1];
				break;
			case MTLFileLine::disp:
				mat->texturesMapPaths[(int)TextureMapType::disp] = tokens[1];
				break;
			case MTLFileLine::decal:
				mat->texturesMapPaths[(int)TextureMapType::decal] = tokens[1];
				break;
			case MTLFileLine::refl:
				mat->texturesMapPaths[(int)TextureMapType::refl] = tokens[1];
				break;
			}
		}
	}
}

ObjFile::Index ObjFile::FaceArgumentToIndex(const char* argString) const
{
	Index index;
	index.type = Index_Vertex;
	char buf[256];
	size_t bufCount = 0;
	const char* onChar = argString;
	int slashCount = 0;
	while (*onChar != '\0')
	{
		if (*onChar == '/' && *(onChar + 1) == '/')
		{
			assert(slashCount == 0);
			buf[bufCount++] = '\0';
			bufCount = 0;
			index.vertexIndex = atoi(buf);
			slashCount += 2;
			onChar += 2;

		}
		else if (*onChar == '/')
		{
			slashCount++;
			buf[bufCount++] = '\0';
			bufCount = 0;
			switch (slashCount)
			{
			case 1:
				index.vertexIndex = atoi(buf);
				index.type |= Index_UV;
				break;
			case 2:
				index.uvIndex = atoi(buf);
				index.type |= Index_Norm;
				break;
			}
			onChar++;
		}
		else
		{
			assert(*onChar <= '9' && *onChar >= '0');
			buf[bufCount++] = *onChar++;
		}
	}

	buf[bufCount++] = '\0';
	bufCount = 0;
	switch (slashCount)
	{
	case 0:
		index.vertexIndex = atoi(buf);
		index.type |= Index_Vertex;
		break;
	case 1:
		index.vertexIndex = atoi(buf);
		index.type |= Index_UV;
		break;
	case 2:
		index.uvIndex = atoi(buf);
		index.type |= Index_Norm;
		break;
	}
	return index;
}
