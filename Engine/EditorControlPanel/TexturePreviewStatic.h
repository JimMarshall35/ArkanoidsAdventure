#pragma once
#include <afxdlgs.h>
#include <string>

class TexturePreviewStatic : public CStatic
{
public:
	enum State
	{
		Unloaded,
		Loading,
		Loaded
	};
public:
	TexturePreviewStatic();
public:
	void LoadTextureAsync(const char* assetsFolderLocalPath);
private:
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC*);
private:
	std::string m_SelectedPath = "";
	CBitmap m_hPreviewBmp;
	State m_State = State::Unloaded;
	int m_nLoadedTextureW = 0;
	int m_nLoadedTextureH = 0;
	int m_nLoadedTextureN = 0;
	DECLARE_MESSAGE_MAP()
};

