// CarDetectorDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "CarDetector.h"
#include "CarDetectorDlg.h"
#include "Locate.h"
#include "ml.h"
#include "ProgressWnd.h"
#include <fstream>


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CCarDetectorDlg 对话框

CCarDetectorDlg::CCarDetectorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCarDetectorDlg::IDD, pParent)
	, m_TimeOne(_T(""))
	, m_TimeAll(_T(""))
	, m_CarNum(_T(""))
	, m_AlwaysDetect(FALSE)
	, m_CarLocation(_T(""))
	, m_Score(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CCarDetectorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_PICTURES, m_PictureList);
	DDX_Text(pDX, IDC_TIME_ONE, m_TimeOne);
	DDX_Text(pDX, IDC_TIME_ALL, m_TimeAll);
	DDX_Text(pDX, IDC_EDIT1, m_CarNum);
	DDX_Check(pDX, IDC_ALWAYS, m_AlwaysDetect);
	DDX_Text(pDX, IDC_LOCATION, m_CarLocation);
	DDX_Text(pDX, IDC_EDIT3, m_Score);
}

BEGIN_MESSAGE_MAP(CCarDetectorDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_OPEN_FILE, &CCarDetectorDlg::OnBnClickedOpenFile)
	ON_BN_CLICKED(IDC_PROCESS, &CCarDetectorDlg::OnBnClickedProcess)
	ON_LBN_SELCHANGE(IDC_LIST_PICTURES, &CCarDetectorDlg::OnLbnSelchangeListPictures)
	ON_BN_CLICKED(IDC_CHOOSE_FOLDER, &CCarDetectorDlg::OnBnClickedChooseFolder)
	ON_BN_CLICKED(IDC_PROCESS_ALL, &CCarDetectorDlg::OnBnClickedProcessAll)
	ON_BN_CLICKED(IDC_ALWAYS, &CCarDetectorDlg::OnBnClickedAlways)
	ON_BN_CLICKED(IDC_MYHELP, &CCarDetectorDlg::OnBnClickedMyhelp)
	ON_BN_CLICKED(IDC_EVALUATE, &CCarDetectorDlg::OnBnClickedEvaluate)
END_MESSAGE_MAP()


// CCarDetectorDlg 消息处理程序

BOOL CCarDetectorDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	m_bProcessed = 0;
	m_pInputImage = NULL;
	m_pResultImage = NULL;
	m_pInputDispImage = NULL;
	m_pResultDispImage = NULL;

	FILE * ret = fopen("CarSVM.xml","rb");
	if (ret == 0) {
		MessageBox(_T("CarSVM.xml not found! - Please put a usable CarSVM.xml in current directory and run again"));
		exit(1);
	} else {
		fclose(ret);
	}

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CCarDetectorDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CCarDetectorDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	CDialog::OnPaint();

	if (m_pInputDispImage != NULL) {
		drawImage(m_pInputDispImage, IDC_PIC_INPUT);
	}

	if (m_pResultDispImage != NULL) {
		drawImage(m_pResultDispImage, IDC_PIC_RESULT);
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CCarDetectorDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CCarDetectorDlg::OnInputChanged() {
	CRect rect;
	GetDlgItem(IDC_PIC_INPUT) ->GetClientRect( &rect );
	int dw = rect.right - rect.left;			
	int dh = rect.bottom - rect.top;

	float iw = (float) m_pInputImage->width;	
	float ih = (float) m_pInputImage->height;

	float sw = dw / iw, sh = dh / ih;
	float scale = (sw < sh) ? sw : sh;
	//if (scale > 1) scale = 1;
	int nw = (int)( iw * scale );
	int nh = (int)( ih * scale );

	CvSize imgSize;
	imgSize.height = nh;
	imgSize.width = nw;
	if (m_pInputDispImage != NULL) {
		cvReleaseImage(&m_pInputDispImage);
		m_pInputDispImage = NULL;
	}
	m_pInputDispImage = cvCreateImage( imgSize, IPL_DEPTH_8U, 3);
	cvResize( m_pInputImage, m_pInputDispImage );

	if (m_pResultDispImage != NULL) {
		cvReleaseImage(&m_pResultDispImage);
		m_pResultDispImage = NULL;
	}

	m_bProcessed = 0;
	m_pResultImage = NULL;
	m_TimeOne = "";
	m_CarLocation = "";

	UpdateData(false);

	// redraw image area
	GetDlgItem(IDC_PIC_INPUT) ->GetWindowRect( &rect );
	ScreenToClient(rect);
	InvalidateRect(rect);

	GetDlgItem(IDC_PIC_RESULT) ->GetWindowRect( &rect );
	ScreenToClient(rect);
	InvalidateRect(rect);

	if(m_AlwaysDetect) {
		this->OnBnClickedProcess();
	}
}

void CCarDetectorDlg::OnResultChanged() {

	CRect rect;
	GetDlgItem(IDC_PIC_RESULT) ->GetClientRect( &rect );
	int dw = rect.right - rect.left;			
	int dh = rect.bottom - rect.top;

	float iw = (float) m_pInputImage->width;	
	float ih = (float) m_pInputImage->height;

	float sw = dw / iw, sh = dh / ih;
	float scale = (sw < sh) ? sw : sh;
	//if (scale > 1) scale = 1;
	int nw = (int)( iw * scale );
	int nh = (int)( ih * scale );

	CvSize imgSize;
	imgSize.height = nh;
	imgSize.width = nw;
	if (m_pResultDispImage != NULL) {
		cvReleaseImage(&m_pResultDispImage);
		m_pResultDispImage = NULL;
	}
	m_pResultDispImage = cvCreateImage( imgSize, IPL_DEPTH_8U, 3);
	cvResize( m_pResultImage, m_pResultDispImage);

	m_bProcessed = 1;
	UpdateData(false);

	GetDlgItem(IDC_PIC_RESULT) ->GetWindowRect( &rect );
	ScreenToClient(rect);
	InvalidateRect(rect);
}

int CCarDetectorDlg::LoadInputImageFromPath(CString path) {
	if (m_pInputImage) {
		cvReleaseImage(&m_pInputImage);
		m_pInputImage = NULL;
	}
	m_pInputImage = cvLoadImage(path);
	if( !m_pInputImage ) {
		CString str;
		str.Format("Image Not Opened! - Now only .pgm file type supported. Check:\n\r %s",path);
		MessageBox(str);
		return -1;
	}

	if (path.Find("Scale") != -1) {
		m_ScaleOne = 1;
	} else {
		m_ScaleOne = 0;
	}

	OnInputChanged();

	return 0;
}


void CCarDetectorDlg::OnBnClickedOpenFile()
{

	CFileDialog dlg(
		TRUE, _T("*.pgm"), NULL,
		OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY,
		_T("image files (*.pgm)|*.pgm|All Files (*.*)|*.*||"), NULL
		);

	dlg.m_ofn.lpstrTitle = _T("Open Image");

	if( dlg.DoModal() != IDOK )	
		return;

	CString path = dlg.GetPathName();

	LoadInputImageFromPath(path);

}

void CCarDetectorDlg::OnBnClickedProcess()
{

	if (m_bProcessed) {
		return;
	}
	if (m_pInputImage == NULL) {
		MessageBox(_T("No Input Image! - Choose an image first"));
		return ;
	}

	if (m_pResultImage != NULL) {
		cvReleaseImage(&m_pResultImage);
		m_pResultImage = NULL;
	}

	clock_t start, end;
	start = clock();
	m_pResultImage = cvCloneImage(m_pInputImage);
	vector<CarLocation> locations;
	if (m_ScaleOne) {
		locations = LocateScaledCars(m_pInputImage);
	} else {
		locations = LocateScaledCars(m_pInputImage);
	}
	end = clock();
	double sec = (double)(end - start) / CLK_TCK;
	m_TimeOne.Format("%.3f s", sec);

	CString str;
	CvPoint p1, p2;
	for (unsigned int i = 0; i < locations.size(); i++) {
		CarLocation l = locations.at(i);
		str.Format("(%d, %d, %d, %d)", l.left, l.top, l.width, l.height);
		m_CarLocation += str;
		p1.x = l.left;
		p1.y = l.top;
		p2.x = p1.x + l.width;
		p2.y = p1.y + l.height;
		cvRectangle(this->m_pResultImage, p1, p2, CV_RGB(255, 255, 60*i));
	}

	m_bProcessed = 1;
	this->OnResultChanged();
}

void CCarDetectorDlg::drawImage(IplImage*  pImg, UINT ctrlID)
{
	// 获得显示控件的 DC
	CDC* pDC = GetDlgItem( ctrlID ) ->GetDC();
	// 获取 HDC(设备句柄) 来进行绘图操作		
	HDC hDC = pDC ->GetSafeHdc();
	CRect rect;
	GetDlgItem(ctrlID) ->GetClientRect( &rect );

	// 求出图片控件的宽和高
	int rw = rect.right - rect.left;			
	int rh = rect.bottom - rect.top;
	// 读取图片的宽和高
	int iw = pImg->width;	
	int ih = pImg->height;

	// 使图片的显示位置正好在控件的正中
	int tx = (int)(rw - iw)/2;	
	int ty = (int)(rh - ih)/2;
	SetRect( rect, tx, ty, tx+iw, ty+ih );

	// 复制图片
	CImage cimg;
	cimg.CopyOf( pImg );
	// 将图片绘制到显示控件的指定区域内	
	cimg.DrawToHDC( hDC, &rect );	
	ReleaseDC( pDC );
}


void CCarDetectorDlg::UpdatePictureList() {
	
	m_PictureList.ResetContent();

	
	for (unsigned int i = 0; i < m_Images.size(); i++) {
		pair<CString, CString> p = m_Images.at(i);
		m_PictureList.AddString(p.first);
	}

	m_TimeAll = "";
	m_CarNum = "";
	m_Score = "";

	UpdateData(false);
	if (m_PictureList.GetCount() <= 0) {
		MessageBox(_T("No Acceptable File! - Choose a folder containing .pgm files."));
	}
}

void CCarDetectorDlg::OnLbnSelchangeListPictures()
{
	int idx = m_PictureList.GetCurSel();
	CString path = m_Images.at(idx).second;
	LoadInputImageFromPath(path);
}


// not used yet
static int compareFunc(const void* arg1, const void* arg2)
{
    pair<CString, CString> *p1 = (pair<CString, CString> *)arg1;
	pair<CString, CString> *p2 = (pair<CString, CString> *)arg2;
	CString name1 = p1->first, name2 = p2->first;
	return 0;
}

void CCarDetectorDlg::OnBnClickedChooseFolder()
{
	char szPath[MAX_PATH];
    CString dir;

    ZeroMemory(szPath, sizeof(szPath));   

    BROWSEINFO bi;   
    bi.hwndOwner = m_hWnd;   
    bi.pidlRoot = NULL;   
    bi.pszDisplayName = szPath;   
    bi.lpszTitle = "Select Folder";   
    bi.ulFlags = 0;   
    bi.lpfn = NULL;   
    bi.lParam = 0;   
    bi.iImage = 0;   

    LPITEMIDLIST lp = SHBrowseForFolder(&bi);   

    if(lp && SHGetPathFromIDList(lp, szPath))   
    {
        dir.Format("%s",  szPath);
        //MessageBox(dir);

		CFileFind ff;
		if (dir.Right(1) != "\\") dir += "\\";
		int scale = 0;
		if (dir.Right(6) == "Scale\\") {
			scale = 1;
		}

		m_Images.clear();
		m_Scale = scale;
		
		BOOL ret = ff.FindFile(dir + "*.pgm");
		while (ret)
		{
			ret = ff.FindNextFile();

			if (ff.IsDirectory() && !ff.IsDots())
			{
				//CString path = ff.GetFilePath();
				continue;
			}
			else if (!ff.IsDirectory() && !ff.IsDots())
			{
				CString name = ff.GetFileName();
				CString path = ff.GetFilePath();
				pair<CString, CString> p(name, path);
				this->m_Images.push_back(p);
			}
		}
		// need to sort

		if ((scale==1 && m_Images.size()!=108) || (scale==0 && m_Images.size()!=170)) {
			MessageBox(_T("You may have not choosen the TestImages or TestImages_Scale folder"));
		}
		m_Images.clear();

		int picNum = (scale ? 107 : 169);
		CString name, path;
		for (int i = 0; i <= picNum; i++) {
			name.Format("test-%d.pgm",i);
			path = dir + name;
			this->m_Images.push_back(make_pair(name, path));
		}
		
		this->UpdatePictureList();

	} else if(lp) { 
        MessageBox(_T("Directory Not Valid! - Choose a common Windows folder"));   
	}
}

void CCarDetectorDlg::OnBnClickedProcessAll()
{
	if (m_Images.size() <= 0) {
		MessageBox(_T("No Item! - Choose a folder to fill the list on the right"));
		return ;
	}

	unsigned int num = m_Images.size();

	CProgressWnd wndProgress(this, "Progress",true);
	wndProgress.GoModal();
	wndProgress.SetRange(0,num);
	wndProgress.SetText("Processing...\n\n"					 
						"Please wait or hit Cancel to abort.");	

	IplImage *pInputImage;
	ofstream fout;
	if (m_Scale) {
		fout.open("foundLocations_Scale.txt");
	} else {
		fout.open("foundLocations.txt");
	}
	CString line, str;
	int carNum = 0, processedPicNum = num;
	clock_t start, end;
	start = clock();
	unsigned int i;
	bool abort = false;
	for (i = 0; i < num; i++) {
		if (abort) {
			line.Format("%d:", i);
			fout<<line<<endl;
			continue;
		}
		pair<CString, CString> p = m_Images.at(i);
		CString path = p.second;
		pInputImage = cvLoadImage( path, CV_LOAD_IMAGE_COLOR);
		vector<CarLocation> locations;
		if (m_Scale) {
			locations = LocateScaledCars(pInputImage);
		} else {
			locations = LocateScaledCars(pInputImage);
		}
		carNum += locations.size();
		line.Format("%d:", i);
		for (unsigned int j = 0; j < locations.size(); j++) {
			CarLocation l = locations.at(j);
			if (m_Scale) {
				str.Format(" (%d, %d, %d)", l.top, l.left, l.width);
			} else {
				str.Format(" (%d, %d)", l.top, l.left);
			}
			line += str;
		}

		fout<<line<<endl;
		cvReleaseImage(&pInputImage);
		wndProgress.StepIt();
		wndProgress.PeekAndPump();

		if (wndProgress.Cancelled()) {
			MessageBox("You've aborted the detecting progress. The remaining images haven't been processed, so it is bad for the score.");
			abort = true;
			processedPicNum = i + 1;
			continue;
		}
	}
	end = clock();
	fout.close();

	double sec = (double)(end - start) / CLK_TCK;
	m_TimeAll.Format("%.3f / %d = %.3f s", sec, processedPicNum, sec / processedPicNum);
	m_CarNum.Format("%d  (in %d images)", carNum, processedPicNum);

	UpdateData(false);
}

void CCarDetectorDlg::OnBnClickedAlways()
{
	m_AlwaysDetect = !m_AlwaysDetect;
}

void CCarDetectorDlg::OnBnClickedMyhelp()
{
	CString help = "(1) Choose an image to detect cars in it. Now only support .pgm file type.\n\n";
	help += "(2) Choose a directory containing test images to detect cars in all of them.\n\n";
	help += "(3) If you want detection to always start immediately when an image loaded, check the Always box. \n\n";
	help += "(4) Evaluate can only be used for test images, of which the true car locations are known. And it needs java.exe in the system path.";
	MessageBox(help);
}

void CCarDetectorDlg::OnBnClickedEvaluate()
{
	// TODO: 在此添加控件通知处理程序代码
	if (m_Scale) {
		WinExec("java Evaluator_Scale trueLocations_Scale.txt foundLocations_Scale.txt",SW_HIDE);
	} else {
		WinExec("java Evaluator trueLocations.txt foundLocations.txt",SW_HIDE);
	}
	Sleep(2000); // wait a while for the result
	ifstream fin;
	if (m_Scale) {
		fin.open("result_Scale.txt");
	} else {
		fin.open("result.txt");
	}
	int posNum = -1, falseNum = -1, totalNum = -1;
	fin>>posNum>>falseNum>>totalNum;
	fin.close();

	if (posNum < 0 || falseNum < 0 || totalNum < 0) {
		MessageBox(_T("Evaluation failed! - Please try to run the following cmd yourself: \n java Evaluator(_Scale) trueLocations.txt foundLocations(_Scale).txt"));
		return ;
	}

	double precision = (posNum + 0.0) / (posNum + falseNum), recall = (posNum + 0.0 ) / totalNum, score = (2 * precision * recall) / (precision + recall);
	m_Score.Format("%.1f%%, %.1f%%, %.1f%%", precision * 100, recall * 100, score * 100);
	
	UpdateData(false);
}
