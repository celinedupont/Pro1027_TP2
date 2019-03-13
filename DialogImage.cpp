// DialogImage.cpp : implementation file
//

#include "stdafx.h"
#include "PgmImage2.h"
#include "DialogImage.h"
#include <iostream>
#include <string>
#include "math.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CDialogImage dialog


CDialogImage::CDialogImage(CImageReelle* pImage, CWnd* pParent /*=NULL*/)
	: CDialog(CDialogImage::IDD, pParent)
	, m_filtre(1)

	, m_Max1(0)
	, m_Max2(0)
	, m_seuil(0)
	, m_threshold(0)
{
	//{{AFX_DATA_INIT(CDialogImage)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	ptImage = pImage;
	width = pImage->NbColonnes;
	length = pImage->NbLignes;

	m_departX = H_OFFSET_I;
	m_departY = V_OFFSET_I;
	m_finX = width + H_OFFSET_I;
	m_finY = length + V_OFFSET_I;
	m_filtre = 3;
	m_seuil = 0;
	m_threshold = 0;

	zoneValide = TRUE;
	hist = Util::vector(1, 256);
	histLisse = Util::vector(1, 256);
	derive = Util::vector(1, 256);
	derive2 = Util::vector(1, 256);
}
CDialogImage::CDialogImage(int largeur, int longueur, CImageReelle* pImage, CWnd* pParent /*=NULL*/)
	: CDialog(CDialogImage::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogImage)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	ptImage = pImage;
	width = largeur;
	length = longueur;

	m_departX = H_OFFSET_I;
	m_departY = V_OFFSET_I;
	m_finX = width + H_OFFSET_I;
	m_finY = length + V_OFFSET_I;
	m_filtre = 3;
	m_seuil = 0;
	m_threshold = 0;

	zoneValide = TRUE;
	hist = Util::vector(1, 256);
	histLisse = Util::vector(1, 256);
	derive = Util::vector(1, 256);
	derive2 = Util::vector(1, 256);

}


void CDialogImage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogImage)
	// NOTE: the ClassWizard will add DDX and DDV calls here

	DDX_Text(pDX, IDC_EDIT1, m_departX);
	DDX_Text(pDX, IDC_EDIT2, m_departY);
	DDX_Text(pDX, IDC_EDIT3, m_finX);
	DDX_Text(pDX, IDC_EDIT4, m_finY);

	//}}AFX_DATA_MAP
	DDX_Text(pDX, IDC_EDIT5, m_filtre);
	DDX_Text(pDX, IDC_EDIT7, m_Max1);
	DDX_Text(pDX, IDC_EDIT8, m_Max2);
	DDX_Text(pDX, IDC_EDIT9, m_seuil);
	DDX_Text(pDX, IDC_EDIT6, m_threshold);
}


BEGIN_MESSAGE_MAP(CDialogImage, CDialog)
	//{{AFX_MSG_MAP(CDialogImage)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_BN_CLICKED(IDC_BUTTON2, &CDialogImage::OnBnClickedHistogramme)
	ON_BN_CLICKED(IDC_BUTTON3, &CDialogImage::OnBnClickeButtonReset)
	ON_BN_CLICKED(IDC_BUTTON4, &CDialogImage::OnBnClickedButtonLissage)
	ON_BN_CLICKED(IDCANCEL, &CDialogImage::OnBnClickedCancel)
	ON_BN_CLICKED(IDOK, &CDialogImage::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON6, &CDialogImage::OnBnClickedButtonCalculSeuil)
	ON_BN_CLICKED(IDC_BUTTON1, &CDialogImage::OnBnClickedButtonAppliquerFiltre)
	ON_BN_CLICKED(IDC_BUTTON7, &CDialogImage::OnBnClickedButtonDerive)
	ON_BN_CLICKED(IDC_BUTTON5, &CDialogImage::OnBnClickedButtonMethodeOtsu)
	ON_BN_CLICKED(IDC_BUTTON8, &CDialogImage::OnBnClickedButtonAppliquerOtsu)
	ON_BN_CLICKED(IDC_BUTTON9, &CDialogImage::OnBnClickedButtonCalculAuto)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogImage message handlers

void CDialogImage::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	int HorizontalShift = H_OFFSET_I;  //decalages entre les bords de la boite de dialogue et l'image
	int VerticalShift = V_OFFSET_I;
	int i, j; // compteurs
	float valeur; //valeur du niveau de gris en un point
	CClientDC ClientDC(this);

	for (j = 0; j < width; j++) {
		for (i = 0; i < length; i++) {
			if (ptImage->matrice[i][j] >= 0)
				valeur = ptImage->matrice[i][j];
			else
				valeur = 256 + ptImage->matrice[i][j];
			ClientDC.SetPixelV(j + HorizontalShift, i + VerticalShift, RGB(valeur, valeur, valeur));
		}
	}
	InitialiserGraphique();
}

void CDialogImage::AffichePoint(int x, int y, string etiquette) {

	CClientDC ClientDC(this);
	CBrush brush;
	brush.CreateSolidBrush(RGB(10, 255, 10));
	ClientDC.SelectObject(&brush);

	ClientDC.Ellipse(x - 5, y - 5, x + 5, y + 5);
	CString cstr(etiquette.c_str());
	ClientDC.TextOutA(x + 5, y, cstr);
}

// selection de la ROI
void CDialogImage::OnLButtonDown(UINT nFlags, CPoint point)
{
	CClientDC ClientDC(this);
	CPen pen;
	CPen penLine;
	CBrush brush;

	penLine.CreatePen(PS_SOLID, 2, RGB(10, 10, 255));

	pen.CreatePen(PS_SOLID, 2, RGB(255, 10, 10));
	ClientDC.SelectObject(&pen);

	brush.CreateSolidBrush(RGB(10, 255, 10));
	ClientDC.SelectObject(&brush);

	// selection du point de depart de ROI
	// stocker les points sélectionnés dans un tableau et increment compteur de clic

	//verifier que le click est sur l'image
	if (!(point.x > (width + H_OFFSET_I) || point.y > (length + V_OFFSET_I) ||
		point.x < H_OFFSET_I || point.y < V_OFFSET_I))
	{
		try
		{
			if (m_departX == H_OFFSET_I) {

				// capture 1er point ROI
				UpdateData(TRUE);

				ClientDC.Ellipse(point.x - 5, point.y - 5, point.x + 5, point.y + 5);
				m_departX = point.x;
				m_departY = point.y;

				zoneValide = FALSE; // 1 seul pt selectionné
				UpdateData(FALSE);
			}
			else if (m_finX == (width + H_OFFSET_I)) {

				//capture 2e point ROI
				UpdateData(TRUE);

				ClientDC.Ellipse(point.x - 5, point.y - 5, point.x + 5, point.y + 5);
				m_finX = point.x;
				m_finY = point.y;
				zoneValide = TRUE;

				// ordonnnee les coordonnées
				CPoint t1, t2;
				if (m_departX < m_finX)
				{
					t1.x = m_departX; t2.x = m_finX;
				}
				else
				{
					t1.x = m_finX; t2.x = m_departX;
				}
				if (m_departY < m_finY)
				{
					t1.y = m_departY; t2.y = m_finY;
				}
				else
				{
					t1.y = m_finY; t2.y = m_departY;
				}

				m_departX = t1.x; m_departY = t1.y;
				m_finX = t2.x; m_finY = t2.y;

				//dessiner rectangle autour de la ROI
				ClientDC.SelectObject(&penLine);

				ClientDC.MoveTo(m_departX - 2, m_departY - 2);
				ClientDC.LineTo(m_finX + 2, m_departY - 2);
				ClientDC.LineTo(m_finX + 2, m_finY + 2);
				ClientDC.LineTo(m_departX - 2, m_finY + 2);
				ClientDC.LineTo(m_departX - 2, m_departY - 2);

				UpdateData(FALSE);
			}
			else {
				throw MyException(" ROI selectionnée. Cliquer \"Reset\" pour créer une nouvelle zone effacer et recommencer.");
			}

		}
		catch (MyException e) {

			MessageBox(TEXT((std::string(e.message)).c_str()), TEXT("Erreur !"), NULL);
		}
	}

	CDialog::OnLButtonDown(nFlags, point);
}


void CDialogImage::CreerVectHistogram()
{
	int valeurPixel;

	// initialise vecteur histogramme
	for (int i = 1; i <= 256; i++)
		hist[i] = 0;

	// ajuster les parametre selon offset de l'image
	int departX = m_departX - H_OFFSET_I + 1;
	int departY = m_departY - V_OFFSET_I + 1;
	int finX = m_finX - H_OFFSET_I;
	int finY = m_finY - V_OFFSET_I;

	// parcourir la ROI et stocker les valeur dans hist
	for (int j = departX; j < finX; j++)
		for (int i = departY; i < finY; i++) {
			valeurPixel = ptImage->matrice[i][j];  // comptage des pixels de chaque couleur
			hist[valeurPixel + 1]++; // 1 à 256
		}
}

void CDialogImage::InitialiserGraphique()
{
	// initialise vecteur vide
	for (int i = 1; i <= 256; i++)
		derive2[i] = 256;

	AfficherHistogramme(derive2, RGB(255, 0, 0));
	AffichePoint(HIST_X_OFFSET + (HIST_WIDTH*HIST_SIZE_RATIO / 2) - 30, (HIST_Y_OFFSET + 20), "HISTOGRAMME");

}

void CDialogImage::AfficherHistogramme(float* fonct, COLORREF color)
{
	//debug  todo: retirer du code final
	float test[257];
	for (int i = 1; i <= 256; i++)
		test[i] = fonct[i];

	CClientDC ClientDC(this);
	CPen penLine;

	//penLine.CreatePen(PS_SOLID, 1, color);
	penLine.CreatePen(PS_SOLID, HIST_SIZE_RATIO, color);
	ClientDC.SelectObject(&penLine);

	// determiner le ratio affichage selon valeur maximum à afficher
	float maxVector = 0.1;
	for (int i = 1; i <= 256; i++) {
		if (fonct[i] > maxVector)
			maxVector = fonct[i];
	}

	// dessiner le vecteur fourni
	for (int i = 1; i <= 256; i++) {
		ClientDC.MoveTo(HIST_X_OFFSET - 1 + i * HIST_SIZE_RATIO, HIST_Y_OFFSET);
		ClientDC.LineTo(HIST_X_OFFSET - 1 + i * HIST_SIZE_RATIO, (HIST_Y_OFFSET - ((fonct[i] * (HIST_HEIGTH) / maxVector)*HIST_SIZE_RATIO)));
	}
}

void CDialogImage::AfficherCourbe(float* fonct)
{
	//debug  todo:retirer code final
	float test2[257];
	for (int i = 1; i <= 256; i++)
		test2[i] = fonct[i];

	CClientDC ClientDC(this);
	CPen penLine;
	CPen penLine2;

	//penLine2.CreatePen(PS_SOLID, 2, RGB(255, 255, 2550));
	penLine2.CreatePen(PS_SOLID, HIST_SIZE_RATIO, RGB(255, 255, 2550));
	ClientDC.SelectObject(&penLine2);

	// axe des x de la dérivé
	ClientDC.MoveTo(HIST_X_OFFSET, HIST_Y_OFFSET - (HIST_HEIGTH*HIST_SIZE_RATIO / 2));
	ClientDC.LineTo(HIST_X_OFFSET + HIST_WIDTH * HIST_SIZE_RATIO, HIST_Y_OFFSET - (HIST_HEIGTH*HIST_SIZE_RATIO / 2));

	penLine.CreatePen(PS_SOLID, HIST_SIZE_RATIO, RGB(0, 0, 255));
	ClientDC.SelectObject(&penLine);

	// determiner le ratio affichage
	float maxVector = 0;
	for (int i = 1; i <= 256; i++) {
		if (abs(fonct[i]) > maxVector)
			maxVector = abs(fonct[i]);
	}

	// dessiner la courbe
	ClientDC.MoveTo(HIST_X_OFFSET, HIST_Y_OFFSET - (HIST_HEIGTH*HIST_SIZE_RATIO / 2));
	for (int i = 1; i <= 256; i++) {
		ClientDC.LineTo(HIST_X_OFFSET - 1 + i * HIST_SIZE_RATIO, (HIST_Y_OFFSET - (HIST_HEIGTH*HIST_SIZE_RATIO / 2) - (fonct[i] * (((HIST_HEIGTH)*HIST_SIZE_RATIO) / 2) / maxVector)));
	}
}

// efface la selctionROI et recharge l'image
void CDialogImage::EffacerROI()
{
	MessageBox(TEXT((std::string("Veuillez attendre le rechargement de l'image pour sélectionner de nouveaux points")).c_str()), TEXT("Chargement de l'image"), NULL);

	OnPaint();

	UpdateData(TRUE);

	m_departX = H_OFFSET_I;
	m_departY = V_OFFSET_I;
	m_finX = width + H_OFFSET_I;
	m_finY = length + V_OFFSET_I;

	zoneValide = TRUE;

	UpdateData(FALSE);

}

// effectue le lissage du vecteur histogramme selon valeur de filtre df.
void CDialogImage::CalculLissage(int df)
{
	//initialise le vecteur histogramme lissé
	for (int i = 1; i <= 256; i++)
		histLisse[i] = 0;

	float somme = 0;
	float ratio = 0.5;
	int nbDebordement = 0;

	// lisser les valeur histogramme avec correction si débprdement causé par largeur de filtre
	for (int x = 1; x <= 256; x++) {
		somme = 0; nbDebordement = 0;

		for (int m = ((-df) / 2); m < (df / 2); m++) {
			if (x + m < 1) {			// remplace les valeur hors zone par wrap aroud. pondéré 50%
				somme += (hist[x + m + 256])* ratio;
				nbDebordement++;
			}
			else if (x + m > 256) {  // remplace les valeur hors zone par wrap aroud. pondéré 50%
				somme += (hist[x + m - 256]) * ratio;
				nbDebordement++;
			}
			else
				somme += hist[x + m];
		}
		//fait la moyenne du kernel
		histLisse[x] = somme / (df - nbDebordement * ratio);
	}

	//debug  todo : retirer du code final
	int test[257];
	for (int i = 1; i <= 256; i++)
		test[i] = derive[i];
}

void CDialogImage::CalculerDerive(int df)
{
	// initialise le vecteur de derive
	for (int i = 1; i <= 256; i++)
		derive[i] = 0;

	float somme = 0;
	float sigma = (df - 1) / 8.0;
	float opGaussien;
	float ratio = 0.5;
	int nbDebordement = 0;

	// convolution de l'histograme et de op Gaussien dérivé
	for (int x = 1; x <= 256; x++) {
		somme = 0; nbDebordement = 0;

		for (int m = ((-df) / 2); m < (df / 2); m++) {
			opGaussien = ((sqrt(2) * m)) / (2 * ((sqrt(M_PI*sigma)))* (sigma*sigma));
			opGaussien *= exp(-(m*m) / (2 * sigma*sigma));
			if (x + m < 1) {			// remplace les valeur hors zone par wrap aroud. pondéré 50%
				somme += (hist[x + m + 256])* ratio *opGaussien;
				nbDebordement++;
			}
			else if (x + m > 256) {  // remplace les valeur hors zone par wrap aroud. pondéré 50%
				somme += (hist[x + m - 256]) * ratio *opGaussien;
				nbDebordement++;
			}
			else
				somme += hist[x + m] * opGaussien;
		}
		//fait la moyenne du kernel
		derive[x] = somme / (df - nbDebordement * ratio);
	}

	//debug  todo: retirer du code final
	int test[257];
	for (int i = 1; i <= 256; i++)
		test[i] = derive[i];
	int stop;
}

void CDialogImage::ChercherSeuil()
{
	// S'assurer de calculer les valeurs
	UpdateData(TRUE);
	UpdateData(FALSE);
	CreerVectHistogram();
	CalculerDerive(m_filtre);

	//chercher les 0 de la derivé, en utilisant la methode de bissection
	int posZero1 = 0, posZero2 = 0, min = 0, posMin = 0;
	try
	{
		posZero1 = Util::rtbis(derive, 1, 128, 1);
		posZero2 = Util::rtbis(derive, 129, 256, 1);

		AffichePoint(HIST_X_OFFSET + posZero1 * HIST_SIZE_RATIO, HIST_Y_OFFSET - (HIST_HEIGTH*HIST_SIZE_RATIO / 2), "");
		AffichePoint(HIST_X_OFFSET + posZero2 * HIST_SIZE_RATIO, HIST_Y_OFFSET - (HIST_HEIGTH* HIST_SIZE_RATIO / 2), "");

		// chercher minimum entre max 1 et max 2
		min = histLisse[posZero1];
		for (int i = posZero1; i <= posZero2; i++) {
			if (histLisse[i] < min)
				min = histLisse[i]; posMin = i;
		}

		UpdateData(TRUE);
		m_Max1 = posZero1;
		m_Max2 = posZero2;
		m_seuil = posMin;
		UpdateData(FALSE);

	}
	catch (MyException e)
	{
		// chercher la valeur de filtre qui permet la sélection de seuil
		int filtreRecommande = FiltreOptimal(m_filtre);
		string message = " Écart insuffisant. Sélectionner une nouvelle zone. ";

		if (filtreRecommande == -1)
			MessageBox(TEXT(message.c_str()), TEXT("Erreur !"), NULL);
		else {
			message = " Valeur de filtre recommandée : ";
			message += std::to_string(filtreRecommande);
			MessageBox(TEXT(message.c_str()), TEXT("Erreur !"), NULL);
		}
	}
}

// methode recursive de recherche de filtre permattant de détecter 2 seuils.
int CDialogImage::FiltreOptimal(int filtreTest)
{
	CalculerDerive(filtreTest);

	//chercher les 0 de la derivé, en utilisant la methode de bissection
	int posZero1 = 0, posZero2 = 0, min = 0, posMin = 0;
	try
	{
		posZero1 = Util::rtbis(derive, 1, 128, 1);
		posZero2 = Util::rtbis(derive, 129, 256, 1);
		string message = " Valeur de filtre recommandée : " + std::to_string(filtreTest);
		
	}
	catch (MyException e)
	{
		if (filtreTest < 99) {

			filtreTest += 2; 
			filtreTest = FiltreOptimal(filtreTest);
		}
		else {
			//MessageBox(TEXT((std::string(e.message) + "\n Essayer de selectionner une nouvelle zone").c_str()), TEXT("Erreur !"), NULL);
			filtreTest = -1;
		}
	}
	return filtreTest;
}


void CDialogImage::MethodeOtsu()
{
	int total = (m_finX - m_departX) * (m_finY - m_departY);

	float sum = 0;
	for (int t = 1; t <= 256; t++)
		sum += t * hist[t];

	float sumB = 0;
	int wB = 0;
	int wF = 0;

	float varMax = 0;
	int threshold = 0;

	for (int t = 1; t <= 256; t++) {
		wB += hist[t];
		if (wB == 0) continue;

		wF = total - wB;
		if (wF == 0) break;

		sumB += (float)(t*hist[t]);

		float mB = sumB / wB;
		float mF = (sum - sumB) / wF;

		float varBetween = (float)wB * (float)wF * (mB - mF) * (mB - mF);

		if (varBetween > varMax) {
			varMax = varBetween;
			threshold = t;
		}
	}
	UpdateData(TRUE);
	m_threshold = threshold;
	UpdateData(FALSE);
}


void CDialogImage::FiltrerImage(int seuil)
{
	CPaintDC dc(this); // device context for painting
	int HorizontalShift = H_OFFSET_I;  //decalages entre les bords de la boite de dialogue et l'image
	int VerticalShift = V_OFFSET_I;
	int i, j; // compteurs
	float valeur; //valeur du niveau de gris en un point
	float valeurFiltre;
	CClientDC ClientDC(this);
	UpdateData(TRUE); UpdateData(FALSE);

	for (j = 0; j < width; j++) {
		for (i = 0; i < length; i++) {
			if (ptImage->matrice[i][j] >= 0)
				valeur = ptImage->matrice[i][j];
			else
				valeur = 256 + ptImage->matrice[i][j];
			//appliquer le filtre 2 couleur
			if (valeur > seuil)
				valeurFiltre = 235;
			else
				valeurFiltre = 20;
			ClientDC.SetPixelV(j + HorizontalShift, i + VerticalShift, RGB(valeurFiltre, valeurFiltre, valeurFiltre));
		}
	}
}




void CDialogImage::OnBnClickedCancel()
{
	Util::free_vector(hist, 1, 256);
	Util::free_vector(histLisse, 1, 256);
	Util::free_vector(derive, 1, 256);
	Util::free_vector(derive2, 1, 256);
	CDialog::OnCancel();
}

void CDialogImage::OnBnClickedOk()
{
	Util::free_vector(hist, 1, 256);
	Util::free_vector(histLisse, 1, 256);
	Util::free_vector(derive, 1, 256);
	Util::free_vector(derive2, 1, 256);
	CDialog::OnOK();
}

void CDialogImage::OnBnClickeButtonReset()
{
	EffacerROI();
	//reinitialiser les vecteurs
	for (int i = 1; i <= 256; i++)
		hist[i] = 0;
	for (int i = 1; i <= 256; i++)
		derive[i] = 0;

	UpdateData(TRUE);
	m_filtre = 9;
	m_seuil = 0;
	m_threshold = 0;
	UpdateData(FALSE);

}

void CDialogImage::OnRButtonDown(UINT nFlags, CPoint point)
{
	OnBnClickeButtonReset();
	CDialog::OnRButtonDown(nFlags, point);
}

void CDialogImage::OnBnClickedButtonLissage()
{
	if (zoneValide) {
		// lancer la création d'histogramme
		InitialiserGraphique();
		CreerVectHistogram();

		UpdateData(TRUE);
		UpdateData(FALSE);
		if (m_filtre % 2 == 1) {
			CalculLissage(m_filtre);
			AfficherHistogramme(hist, RGB(0, 0, 0));
			AfficherHistogramme(histLisse, RGB(0, 255, 0));
		}
		else
			MessageBox(TEXT("La valeur du filtre doit être un nombre impair enre 1 et 21"), TEXT("Entree erronee"), NULL);

	}
	else
		MessageBox(TEXT("selection invalide"), TEXT("Erreur !"), NULL);

}

void CDialogImage::OnBnClickedHistogramme()
{
	if (zoneValide) {
		// lancer la création d'histogramme
		InitialiserGraphique();
		CreerVectHistogram();
		AfficherHistogramme(hist, RGB(0, 0, 0));
	}
	else
		MessageBox(TEXT("selection invalide"), TEXT("Erreur !"), NULL);
}

void CDialogImage::OnBnClickedButtonDerive()
{
	if (zoneValide) {
		// lancer la création d'histogramme
		InitialiserGraphique();
		CreerVectHistogram();
		AfficherHistogramme(hist, RGB(0, 0, 0));

		UpdateData(TRUE);
		UpdateData(FALSE);
		if (m_filtre % 2 == 1) {
			CalculerDerive(m_filtre);
			AfficherCourbe(derive);
		}
		else
			MessageBox(TEXT("La valeur du filtre doit être un nombre impaire"), TEXT("Entree erronee"), NULL);

	}
	else
		MessageBox(TEXT("selection invalide"), TEXT("Erreur !"), NULL);

}

void CDialogImage::OnBnClickedButtonCalculSeuil()
{
	if (zoneValide) {
		OnBnClickedButtonDerive();
		ChercherSeuil();
	}
	else
		MessageBox(TEXT("selection invalide"), TEXT("Erreur !"), NULL);
}

void CDialogImage::OnBnClickedButtonAppliquerFiltre()
{
	if (m_seuil > 0) {
		FiltrerImage(m_seuil);
	}
}



void CDialogImage::OnBnClickedButtonMethodeOtsu()
{
	CreerVectHistogram();
	MethodeOtsu();
}


void CDialogImage::OnBnClickedButtonAppliquerOtsu()
{
	if (m_threshold != 0)
		FiltrerImage(m_threshold);
}

void CDialogImage::OnBnClickedButtonCalculAuto()
{

	if (zoneValide) {
		// lancer la création d'histogramme	

		InitialiserGraphique();
		CreerVectHistogram();
		AfficherHistogramme(hist, RGB(0, 0, 0));

		if (m_filtre % 2 == 0) {
			MessageBox(TEXT("La valeur du filtre doit être un nombre impair. Valeur incrementée de 1"), TEXT("Entree erronee"), NULL);
			UpdateData(TRUE);
			m_filtre++;
			UpdateData(FALSE);
		}
		UpdateData(TRUE);
		m_filtre = FiltreOptimal(m_filtre);
		CalculLissage(m_filtre);
		AfficherHistogramme(histLisse, RGB(0, 255, 0));
		CalculerDerive(m_filtre);
		AfficherCourbe(derive);

		UpdateData(FALSE);
		ChercherSeuil();
		MethodeOtsu();

		FiltrerImage(m_seuil);
	}
	else
		MessageBox(TEXT("selection invalide"), TEXT("Erreur !"), NULL);


}


