// CarDetectorDlg.h : 头文件
//

#pragma once

#include "cv.h"
#include "highgui.h"
#include "afxwin.h"
#include <vector>
using namespace std;

// CCarDetectorDlg 对话框
class CCarDetectorDlg : public CDialog
{
// 构造
public:
	CCarDetectorDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_CARDETECTOR_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

	IplImage* m_pInputImage;
	IplImage* m_pInputDispImage;
	IplImage* m_pResultImage;
	IplImage* m_pResultDispImage;
	int m_bProcessed;

	vector<pair<CString,CString>> m_Images;
	int m_Scale;
	int m_ScaleOne;
	
	void drawImage(IplImage*  pImg, UINT ctrlID);
	void resizeImage(IplImage* pImg, IplImage* pDstImg, float scale);
	void resizeToDisplay(IplImage* pImg);
	void CCarDetectorDlg::OnInputChanged();
	void CCarDetectorDlg::OnResultChanged();
	void CCarDetectorDlg::UpdatePictureList();
	int CCarDetectorDlg::LoadInputImageFromPath(CString path);

public:
	afx_msg void OnBnClickedOpenFile();
	afx_msg void OnBnClickedProcess();
	CListBox m_PictureList;
	afx_msg void OnLbnSelchangeListPictures();
	afx_msg void OnBnClickedChooseFolder();

	CString m_TimeOne;
	afx_msg void OnBnClickedProcessAll();
	CString m_TimeAll;
	CString m_CarNum;
	BOOL m_AlwaysDetect;
	afx_msg void OnBnClickedAlways();
	CString m_CarLocation;
	afx_msg void OnBnClickedMyhelp();
	CString m_Score;
	afx_msg void OnBnClickedEvaluate();
};
