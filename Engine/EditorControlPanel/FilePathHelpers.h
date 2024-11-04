#pragma once
#include <string>

namespace FilePath
{
	bool IsFilepath(const wchar_t* path);
	std::wstring GetFileExtension(const wchar_t* path);
	bool IsFilepathWithinFolder(const wchar_t* folderPath, const wchar_t* filePath);
	std::wstring GetFilepathRelativeToFolder(const wchar_t* folder, const wchar_t* filePath);
	std::wstring GetFileNameWithoutExtension(const wchar_t* path);
}