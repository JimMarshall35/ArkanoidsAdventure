#include "pch.h"
#include "FilePathHelpers.h"
#include <fstream>
#include <filesystem>
namespace fs = std::filesystem;

namespace FilePath
{
	bool IsFilepath(const wchar_t* path)
	{
		fs::path p(path);
		return fs::exists(p) && fs::is_regular_file(p);
	}

	std::wstring GetFileExtension(const wchar_t* path)
	{
		if (!IsFilepath(path))
		{
			return nullptr;
		}
		return fs::path(path).extension();
	}

	static bool IsFilepathWithinFolderInternal(const wchar_t* folderPath, const wchar_t* filePath, std::wstring* outRelativePath = nullptr)
	{
		fs::path foP(folderPath);
		fs::path fiP(filePath);
		if (!fs::exists(foP) || !fs::exists(foP))
		{
			return false;
		}
		if (!fs::is_regular_file(fiP) || !fs::is_directory(foP))
		{
			return false;
		}
		fs::path aFoP = fs::absolute(foP);
		fs::path aFiP = fs::absolute(fiP);

		const wchar_t* absFile = aFiP.c_str();
		const wchar_t* absFolder = aFoP.c_str();

		if (wcslen(absFile) <= wcslen(absFolder))
		{
			return false;
		}

		while (*absFolder != L'\0')
		{
			if (*absFolder++ != *absFile++)
			{
				return false;
			}
		}

		if (outRelativePath)
		{
			*outRelativePath = absFile;
		}

		return true;
	}

	bool IsFilepathWithinFolder(const wchar_t* folderPath, const wchar_t* filePath)
	{
		return IsFilepathWithinFolderInternal(folderPath, filePath);
	}

	std::wstring GetFilepathRelativeToFolder(const wchar_t* folderPath, const wchar_t* filePath)
	{
		std::wstring r;
		if (!IsFilepathWithinFolderInternal(folderPath, filePath, &r))
		{
			return L"";
		}

		return r;
	}
	std::wstring GetFileNameWithoutExtension(const wchar_t* path)
	{
		if (!IsFilepath(path))
		{
			return nullptr;
		}
		return fs::path(path).stem();
	}
}


