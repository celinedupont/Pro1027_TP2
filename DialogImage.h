#if !defined(AFX_DIALOGIMAGE_H__7D8FC2BD_E66A_4306_A18B_924A83B05B12__INCLUDED_)
#define AFX_DIALOGIMAGE_H__7D8FC2BD_E66A_4306_A18B_924A83B05B12__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DialogImage.h : header file
//
#include "ImageReelle.h"
#include "Util.h"
#include "math.h"
#include <string>

#define  H_OFFSET_I 20
#define  V_OFFSET_I 20

#define  HIST_X_OFFSET 700
#define	 HIST_Y_OFFSET 680
#define  HIST_HEIGTH 100
#define	 HIST_WIDTH 256
#define  HIST_SIZE_RATIO 2

#define _USE_MATH_DEFINES

/////////////////////////////////////////////////////////////////////////////
// CDialogImage dialog

class CDialogImage : public CDialog
{
// Construction
public:
	CDialogImage(CImageReelle* pImage,CWnd* pParent = NULL);   // standard constructor
	CDialogImage(int, int, CImageReelle* pImage, CWnd* pParent = NULL);   // constructeur avec redimensionnement

// Dialog Data
	//{{AFX_DATA(CDialogImage)
	enum { IDD = IDD_DIALOG_IMAGE };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogImage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogImage)
	afx_msg void OnPaint();
	afx_msg void AffichePoint(int x, int y, string etiquette);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	CImageReelle * ptImage;
	int width, length;

public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	int m_departX;
	int m_departY;
	int m_finX;
	int m_finY;
	bool zoneValide;

	int m_filtre;
	int m_Max1;
	int m_Max2;
	int m_seuil;
	int m_threshold;
	
	float* hist;
	float* histLisse;
	float* derive;
	float* derive2;

	
	void EffacerROI();
	void InitialiserGraphique();
	void CreerVectHistogram();
	void CalculLissage(int df);
	void CalculerDerive(int df);
	void AfficherCourbe(float* fonct);
	void ChercherSeuil();
	int FiltreOptimal(int filtreTest);
	void MethodeOtsu();
	void FiltrerImage(int seuil);

	afx_msg void AfficherHistogramme(float * vect, COLORREF color);
	afx_msg void OnBnClickedHistogramme();
	afx_msg void OnBnClickeButtonReset();
	afx_msg void OnBnClickedButtonLissage();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButtonCalculSeuil();
	afx_msg void OnBnClickedButtonAppliquerFiltre();
	afx_msg void OnBnClickedButtonDerive();
	afx_msg void OnBnClickedButtonMethodeOtsu();
	afx_msg void OnBnClickedButtonAppliquerOtsu();
	afx_msg void OnBnClickedButtonCalculAuto();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOGIMAGE_H__7D8FC2BD_E66A_4306_A18B_924A83B05B12__INCLUDED_)
