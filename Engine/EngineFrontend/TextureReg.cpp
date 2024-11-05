#include "TextureReg.h"
#include "Texture.h"
#include "FrontendError.h"
#include "IArchive.h"
#include "EngineDLLInterface.h"
#include "IBackendApp.h"
#include "stb_image.h"
#include "Scene.h"
#include "FrontendError.h"

bool TextureReg::UploadTextureFile(const char* assetFolderPath, const char* textureName, const UploadTextureFileOptions* options /*= nullptr*/)
{
	const BackendAPI& api = Engine::GetAPI();
	UploadTextureFileOptions defaultOptions;
	if (!options)
	{
		options = &defaultOptions;
	}
	Scn::Scene& scn = Scn::GetScene();

	// Textures
	int width, height, nrChannels;
	const char* path = api.GetAssetFullPathFn(assetFolderPath);
	unsigned char* data = stbi_load(path, &width, &height, &nrChannels, options->requiredComponents);
	if (!data)
	{
		Err::LogError("can't load texture file: '%s'", path);
		return true;
	}
	TextureData td;
	td.HeightPx = height;
	td.WidthPx = width;
	td.pData = options->bRetainDataCPUMemory ? data : nullptr;
	td.DataSize = height * width * nrChannels;
	td.Name = textureName;
	td.Path = assetFolderPath;

	td.MinifyFiltering = options->MinifyFiltering;
	td.MagnifyFiltering = options->MagnifyFiltering;
	td.bGenerateMipMaps = options->bGenerateMipMaps;
	td.TextureClampS = options->TextureClampS;
	td.TextureClampT = options->TextureClampT;
	/*
		behavior of stbi_load:
			- if requiredChannels arg is passed in as 0, nrChannels out arg is set to the number of channels
			- if requiredChannels is not 0, nrChannels out arg will be set to same value as it would be if 0 was passed in for requiredChannels,
			  but the number of channels will actually be requiredChannels
	
		hence the expression in the switch statement below - we want to switch the actual number of channels in the loaded data
	*/
	switch (options->requiredComponents ? options->requiredComponents : nrChannels)
	{
	case 3:
		td.Format = TexFormat::R8G8B8;
		break;
	case 4:
		td.Format = TexFormat::R8G8B8A8;
		break;
	default:
		Err::LogWarning("unsuppored number of channels (%i) in image '%s'");
		break;
	}

	if (options->bRetainDataCPUMemory)
	{
		scn.textureReg.RegisterTexture(td, [](void* pData) {stbi_image_free(pData); });
	}
	else
	{
		scn.textureReg.RegisterTexture(td, [](void* pData) {});
		stbi_image_free(data);
	}
	return false;
}

bool TextureReg::RegisterTexture(const TextureData& data, TextureDataFreeFn freer)
{
	if (m_NameMap.find(data.Name) != m_NameMap.end())
	{
		Err::LogError("Texture name '%s' taken", data.Name);
		return false;
	}
	HTexture t = Engine::GetAPI().UploadTexture(data);
	if (t == ENGINE_NULL_HANDLE)
	{
		Err::LogError( "Texture name '%s' upload failed", data.Name);
		return false;
	}
	m_Textures.push_back({ data, t });
	EAssert(m_Freeers.find(data.Name) == m_Freeers.end());
	m_Freeers[data.Name] = freer;
	EAssert(m_NameMap.find(data.Name) == m_NameMap.end());
	m_NameMap[data.Name] = &m_Textures[m_Textures.size() - 1];
	return true;
}
void TextureReg::Clear()
{
	for (EPair<TextureData, HTexture>& p : m_Textures)
	{
		Engine::GetAPI().DestroyTexture(p.second);
		m_Freeers[p.first.Name](p.first.pData);
	}
	m_Textures.clear();
	m_NameMap.clear();
	m_Freeers.clear();
}
void TextureReg::Serialize(IArchive& archive)
{
	const int version = 1;
	if (archive.IsStoring())
	{
		archive.PushObj("TextureReg");
		archive << version;
		archive.PushObj("Textures");
		archive << m_Textures.size();
		for (EPair<TextureData, HTexture>& p : m_Textures)
		{
			archive.PushObj("Pair");
			archive.PushObj("Handle");
			archive << p.second;
			archive.PopObj();
			p.first.Serialize(&archive);
			archive.PopObj();
		}
		archive.PopObj();
		archive.PopObj();
	}
	else
	{
		archive.PushObj("TextureReg");
		int v;
		archive >> v;
		switch (v)
		{
		case 1:
		{
			archive.PushObj("Textures");
			size_t sz;
			archive >> sz;
			m_Textures.resize(sz);

			for (int i = 0; i < sz; i++)
			{
				EPair<TextureData, HTexture>& p = m_Textures[i];
				archive.PushChild(i);
				archive.PushObj("Handle");
				archive >> p.second;
				archive.PopObj();
				p.first.Serialize(&archive);
				m_NameMap[p.first.Name] = &p;
				archive.PopObj();
			}
			archive.PopObj();
			break;
		}
		default:
			break;
		}
		archive.PopObj();

	}
}
void TextureReg::ReloadTextures(EMap<HTexture, HTexture>& outOldToNewHandleMap)
{
	const BackendAPI& api = Engine::GetAPI();
	for (EPair<TextureData, HTexture>& p : m_Textures)
	{
		const char* absolutePath = api.GetAssetFullPathFn(p.first.Path.c_str());
		int width, height, nrChannels;
		unsigned char* data = stbi_load(absolutePath, &width, &height, &nrChannels, 0);

		TextureData& td = p.first;;
		td.HeightPx = height;
		td.WidthPx = width;
		td.pData = data;
		td.DataSize = height * width * nrChannels;
		m_Freeers[p.first.Name] = [](void* ptr)
		{
			stbi_image_free(ptr);
		};
		HTexture t = Engine::GetAPI().UploadTexture(p.first);
		outOldToNewHandleMap[p.second] = t;
		p.second = t;
	}
}

HTexture TextureReg::GetTexture(const char* name) const
{
	if (m_NameMap.find(name) == m_NameMap.end())
	{
		return ENGINE_NULL_HANDLE;
	}
	return m_NameMap.at(name)->second;
}

const TextureData* TextureReg::GetTextureData(const char* name) const
{
	if (m_NameMap.find(name) == m_NameMap.end())
	{
		return nullptr;
	}
	return &(m_NameMap.at(name)->first);
}

TextureData* TextureReg::GetTextureData(const char* name)
{
	if (m_NameMap.find(name) == m_NameMap.end())
	{
		return nullptr;
	}
	return &(m_NameMap.at(name)->first);
}
