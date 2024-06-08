#include "SerializeObj.h"
#include <stdio.h>
#include "pch.h"
#include <stdlib.h>
#include "PrismMesh.h"
#include <windows.h>


void SaveAsObj(const PrismMesh& prismMesh, const char* filePath)
{
	FILE* fp;
	fopen_s(&fp, filePath, "w");

	for (const glm::vec3& p : prismMesh.GetPositions())
	{
		fprintf(fp, "v %f %f %f\n", p.x, p.y, p.z);
	}

	for (const glm::vec2& uv : prismMesh.GetUVs())
	{
		fprintf(fp, "vt %f %f\n", uv.x, uv.y);
	}

	for (const glm::vec3& p : prismMesh.GetNormals())
	{
		fprintf(fp, "vn %f %f %f\n", p.x, p.y, p.z);
	}

	const std::vector<int>& indices = prismMesh.GetIndices();
	ASSERT((indices.size() % 3) == 0);

	for (int i = 0; i < indices.size(); i += 3)
	{
		fprintf(fp, "f %i/%i/%i %i/%i/%i %i/%i/%i\n", 
			indices[i] + 1,
			indices[i] + 1,
			indices[i] + 1,

			indices[i + 1] + 1,
			indices[i + 1] + 1,
			indices[i + 1] + 1,

			indices[i + 2] + 1,
			indices[i + 2] + 1,		
			indices[i + 2] + 1
		);
	}

	fclose(fp);
}
