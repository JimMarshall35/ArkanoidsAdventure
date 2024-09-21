#pragma once
#include <afxdlgs.h>
#include "MainPropertySheet.h"

class MainDialog : public CDialog
{
public:
	MainDialog();
	virtual BOOL OnInitDialog() override;
private:
	MainPropertySheet m_dlgMainPropSheet;
};

