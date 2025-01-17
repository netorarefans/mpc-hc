/*
 * (C) 2021-2022 see Authors.txt
 *
 * This file is part of MPC-BE.
 *
 * MPC-BE is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * MPC-BE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "stdafx.h"
#include "AddCommandDlg.h"
#include "mplayerc.h"

// CAddCommandDlg dialog

//IMPLEMENT_DYNAMIC(CAddCommandDlg, CMPCThemeResizableDialog)
CAddCommandDlg::CAddCommandDlg(CWnd* pParent)
	: CMPCThemeResizableDialog(CAddCommandDlg::IDD, pParent)
{
}

CAddCommandDlg::~CAddCommandDlg()
{
}

void CAddCommandDlg::FillList()
{
	CAppSettings& s = AfxGetAppSettings();

    POSITION pos = s.wmcmds.GetHeadPosition();
    int i = 0;
    while (pos) {
        wmcmd& wc = s.wmcmds.GetNext(pos);

		m_list.InsertItem(i, wc.GetName());
		CString str_id;
		str_id.Format(L"%d", wc.cmd);
		m_list.SetItemText(i, COL_ID, str_id);
		m_list.SetItemData(i, wc.cmd);
        i++;
	}
}

void CAddCommandDlg::FilterList()
{
	m_okButton.EnableWindow(FALSE);

	CString filter;
	m_FilterEdit.GetWindowText(filter);

	m_list.SetRedraw(false);
	m_list.DeleteAllItems();

	CAppSettings& s = AfxGetAppSettings();

	if (filter.IsEmpty()) {
		FillList();
	}
	else {
		auto LowerCase = [](CString& str) {
			if (!str.IsEmpty()) {
				::CharLowerBuffW(str.GetBuffer(), str.GetLength());
			}
		};
		LowerCase(filter);
		int n = 0;

        POSITION pos = s.wmcmds.GetHeadPosition();
        while (pos) {
            wmcmd& wc = s.wmcmds.GetNext(pos);

			CString name = wc.GetName();
			LowerCase(name);
			CString str_id;
			str_id.Format(L"%d", wc.cmd);

			if (name.Find(filter) >= 0 || str_id.Find(filter) >= 0) {
				m_list.InsertItem(n, wc.GetName());
				m_list.SetItemText(n, COL_ID, str_id);
				m_list.SetItemData(n, wc.cmd);
			}
		}
	}

	m_list.SetRedraw(true);
	m_list.RedrawWindow();
}

void CAddCommandDlg::DoDataExchange(CDataExchange* pDX)
{
	CMPCThemeResizableDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_EDIT1, m_FilterEdit);
	DDX_Control(pDX, IDC_LIST1, m_list);
	DDX_Control(pDX, IDOK, m_okButton);
}

BEGIN_MESSAGE_MAP(CAddCommandDlg, CMPCThemeResizableDialog)
	ON_WM_TIMER()
	ON_EN_CHANGE(IDC_EDIT1, OnChangeFilterEdit)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST1, OnItemchangedList)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()

// CAddCommandDlg message handlers

BOOL CAddCommandDlg::OnInitDialog()
{
	CMPCThemeResizableDialog::OnInitDialog();

	//m_list.SetExtendedStyle(m_list.GetExtendedStyle() | LVS_EX_COLUMNSNAPPOINTS | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER);
    m_list.setAdditionalStyles(LVS_EX_COLUMNSNAPPOINTS | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER);

	m_list.InsertColumn(COL_CMD, ResStr(IDS_MOUSE_COMMAND));
	m_list.InsertColumn(COL_ID, L"ID");

	CAppSettings& s = AfxGetAppSettings();

	FillList();

	for (int nCol = 0; nCol < COL_COUNT; nCol++) {
		m_list.SetColumnWidth(nCol, LVSCW_AUTOSIZE_USEHEADER);
		const int headerSize = m_list.GetColumnWidth(nCol);

		LVCOLUMNW col;
		col.mask = LVCF_MINWIDTH;
		col.cxMin = headerSize;
		m_list.SetColumn(nCol, &col);
	}

	m_okButton.EnableWindow(FALSE);

    AddAnchor(IDC_EDIT1, TOP_LEFT);
    AddAnchor(IDC_LIST1, TOP_LEFT, BOTTOM_RIGHT);
    AddAnchor(IDOK, BOTTOM_RIGHT);
    AddAnchor(IDCANCEL, BOTTOM_RIGHT);
    fulfillThemeReqs();

	return TRUE;
}

void CAddCommandDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == m_nFilterTimerID) {
		KillTimer(m_nFilterTimerID);
		FilterList();
	} else {
		__super::OnTimer(nIDEvent);
	}
}

void CAddCommandDlg::OnChangeFilterEdit()
{
	KillTimer(m_nFilterTimerID);
	m_nFilterTimerID = SetTimer(2, 100, nullptr);
}

void CAddCommandDlg::OnItemchangedList(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	if(pNMLV->uChanged & LVIF_STATE) {
		if (pNMLV->uNewState & (LVIS_SELECTED | LVNI_FOCUSED)) {
			m_okButton.EnableWindow(TRUE);
		}
		else {
			m_okButton.EnableWindow(FALSE);
		}
	}

	*pResult = 0;
}


void CAddCommandDlg::OnBnClickedOk()
{
	UpdateData();

	int selected = m_list.GetSelectionMark();
	if (selected != -1) {
		m_selectedID = m_list.GetItemData(selected);
	}

	OnOK();
}
