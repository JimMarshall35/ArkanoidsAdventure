#include "pch.h"
#include "TexturePreviewStatic.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "EditorClient.h"
#include "StringHelpers.h"
#include <wingdi.h>

BEGIN_MESSAGE_MAP(TexturePreviewStatic, CStatic)
	//ON_WM_PAINT()
	//ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

TexturePreviewStatic::TexturePreviewStatic()
{
}

void TexturePreviewStatic::LoadTextureAsync(const char* assetsFolderLocalPath)
{
	m_SelectedPath = assetsFolderLocalPath;
	m_State = State::Loading;
	EditorClient::GetAssetFolderPathAsync([this](const char* folder) {
		std::string sFolder = folder;
		std::string path = sFolder + std::string("\\") + m_SelectedPath;
		int x,y,n;
		if (unsigned char* data = stbi_load(path.c_str(), &x, &y, &n, 4))
		{
			m_nLoadedTextureW = x;
			m_nLoadedTextureH = y;
			m_nLoadedTextureN = n;

			m_hPreviewBmp.DeleteObject();
			m_hPreviewBmp.CreateBitmap(x, y, 1, 4 * 8, data);
			SetBitmap(m_hPreviewBmp);
			stbi_image_free(data);
			m_State = State::Loaded;
			InvalidateRect(NULL);
		}
		else
		{
			m_State = State::Unloaded;
			ASSERT(false);
			// TODO: log error
		}
	});
}

void TexturePreviewStatic::OnPaint()
{
	//switch (m_State)
	//{
	//case State::Loaded:
	//{
	//	CRect rect;
	//	GetClientRect(&rect);
	//	float w = rect.Width();
	//	float h = rect.Height();
	//	PAINTSTRUCT ps;
	//	CDC* dc = BeginPaint(&ps);

	//	// Create a memory device compatible with the above DC variable
	//	HDC MemDC = CreateCompatibleDC(dc->GetSafeHdc());

	//	// Select the new bitmap
	//	HGDIOBJ hOldBmp = ::SelectObject(MemDC, (HGDIOBJ)m_hPreviewBmp);

	//	// Copy the bits from the memory DC into the current dc
	//	BitBlt(dc->GetSafeHdc(), 0, 0, m_nLoadedTextureW, m_nLoadedTextureH, MemDC, 0, 0, SRCCOPY);
	//	::SelectObject(MemDC, hOldBmp);
	//	::DeleteDC(MemDC);
	//	EndPaint(&ps);
	//	break;
	//}
	//}
	


}

BOOL TexturePreviewStatic::OnEraseBkgnd(CDC* pCDC)
{
	return FALSE;
}
