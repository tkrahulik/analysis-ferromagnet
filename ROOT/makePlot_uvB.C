#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <sstream>

#include "TROOT.h"
#include "TFile.h"
#include "TH1F.h"
#include "TF1.h"
#include "TCanvas.h"
#include "TTree.h"
#include "TGraph.h"
#include "TVector.h"
#include "TMath.h"
#include "TLegend.h"

using namespace std;

/* =====================================
 * Calibration Measurements and Analysis
   ===================================== */
/*
 * The Calibration function reads in the
 * calibration file and returns a graph
 * that is used to find the relation
 * between current and magentic field
 * for the Helmholtz coil.
 */
TF1* Calibration(
		    const char* f_calib
		    )
{
  /*Read in Calibration File*/
  cout<< "processing file " << f_calib <<endl;
  TTree *TCalib = new TTree();
  TCalib->ReadFile(f_calib, "t/D:I:B");
  int n = TCalib->Draw("I:TMath::Abs(B)", "", "goff");
  TGraph *g_calib = new TGraph(n, TCalib->GetV1(), TCalib->GetV2() );
  g_calib->SetTitle("");
  //g_calib->Draw("AP");
  g_calib->Fit("pol1");
  TF1* calib_fit = g_calib->GetFunction("pol1");

  return calib_fit;

}

/* =====================================
 *      Ferromagnet Scans Analysis
   ===================================== */
/*
 * The Ferromagnet_Scan function reads in
 * a data file of magentic field scans
 * from within the ferromagnet and returns
 * a graph of the magnetic permeability
 * of the ferromagnet vs the external field
 * provided by the Helmholtz coil.
 */
TGraph* Ferromagnet_Scan(
			 const TString scan_file,
			 TF1* calib_fit,
			 double R
)
{

  /*Read in Data File to ROOT Tree*/
  cout<< "processing file " << scan_file <<endl;
  TTree *TData = new TTree();
  TData->ReadFile(scan_file, "t/D:I:B");
  /*Use TTree Draw command to write branches to usable arrays*/
  int n = TData->Draw("I:TMath::Abs(B)", "", "goff");
  TGraph *g_uvB = new TGraph(n, TData->GetV1(), TData->GetV2() );
  Double_t B_ext[n], B_in[n], Bratio[n], u[n];
  for (int i = 0; i<g_uvB->GetN(); i++)
    { 
      /*Calculate External Field at each data point*/
      B_ext[i] = calib_fit->Eval(g_uvB->GetX()[i]);
      /*Calculate Internal Field at each data point*/
      B_in[i] = g_uvB->GetY()[i];
      /*Calculate Ratio of Fields at each data point*/
      Bratio[i] = B_in[i] / B_ext[i];
      /*Calculate Permeability at each data point*/
      u[i] = ( Bratio[i] * pow(R,2) + Bratio[i] - 2 - 2*sqrt(pow(Bratio[i],2)*pow(R,2) - Bratio[i]*pow(R,2) - Bratio[i] + 1 ) ) / (Bratio[i]*pow(R,2) - Bratio[i]);
      /*Reset each data point to graph calculated u vs B*/
      g_uvB->SetPoint(i, B_ext[i], u[i]);
    }
  /*
  g_uvB->Fit("pol1", "", "", 10, 60);
  cout << "Permeability at 50: " << g_uvB->GetFunction("pol1")->Eval(50) << endl;
  cout << "Permeability at 40: " << g_uvB->GetFunction("pol1")->Eval(40) << endl;
  cout << "Permeability at 30: " << g_uvB->GetFunction("pol1")->Eval(30) << endl;
  */
  return g_uvB;
}

/* ====================================
 * Plot Magnetic Permeability vs Field
   ====================================*/
void makePlot_uvB()
{
  /*Define inner and outer radii of ferromagnet*/
  double ri = 25.1;
  double  ro = 41.21;
  /*Calculate ratio of inner radius vs outer radius*/
  double R = ri / ro;
  double R2 = 1.25/2.1;
  cout << "Thickness Ratio: " << R << endl;
  /*Calculate theoretical permeability of ferromagnet*/
  double u_cloak = (1 + pow(R2,2)) / (1 - pow(R2,2));
  cout << "Permeability: " << u_cloak << endl;

  /*
   * Permeability Plot for First Ferromagnet
   */
  /*Run calibration*/
  TF1 *calib_fit1 = Calibration("../Data/Calibration_Data/DataFile_160621_182804.txt");
  TCanvas *c_uvB = new TCanvas();
  TH1 *h_uvB = c_uvB->DrawFrame(0, 0.5, 60, 5.0);
  h_uvB->SetTitle("Permeability of Epoxy/Steel F_{v} = 0.15");
  h_uvB->GetXaxis()->SetTitle("B_{0} (mT)");
  h_uvB->GetXaxis()->SetTitleOffset(1.5);
  h_uvB->GetYaxis()->SetTitle("#mu_{r}");
  h_uvB->GetYaxis()->SetTitleOffset(1.5);

  TGraph *fm_scan1 = Ferromagnet_Scan("../Data/Ferromagnet_Scan_Data/DataFile_160621_182201.txt", calib_fit1, R2);
  fm_scan1->Draw("LP");
  fm_scan1->SetLineColor(kBlue);
  fm_scan1->SetMarkerColor(kBlue);

  /*
   * Permeability Plot for Second Ferromagnet
   */
  /*Run Calibration*/
  TF1 *calib_fit2 = Calibration("../Data/Calibration_Data/DataFile_160331_174922.txt");
  TGraph *fm_scan2 = Ferromagnet_Scan("../Data/Ferromagnet_Scan_Data/DataFile_160331_174304.txt", calib_fit2, R);
  //fm_scan2->Draw("LP");
  fm_scan2->SetLineColor(kRed+1);
  fm_scan2->SetMarkerColor(kRed+1);

  /*
   * Permeability Plot for Third Ferromagnet
   */
  /*Run Calibration*/
  TF1 *calib_fit3 = Calibration("../Data/Calibration_Data/DataFile_160331_174922.txt");
  TGraph *fm_scan3 = Ferromagnet_Scan("../Data/Ferromagnet_Scan_Data/DataFile_160331_181010.txt", calib_fit3, R);
  //fm_scan3->Draw("LP");
  fm_scan3->SetLineColor(kGreen+2);
  fm_scan3->SetMarkerColor(kGreen+2);

  /*
   * Permeability Plot for Fourth Ferromagnet
   */
  /*Run Calibration*/
  TF1 *calib_fit4 = Calibration("../Data/Calibration_Data/DataFile_150618_104728.txt");
  TGraph *fm_scan4 = Ferromagnet_Scan("../Data/Ferromagnet_Scan_Data/DataFile_150611_122014.txt", calib_fit4, R);
  //fm_scan4->Draw("LP");
  //fm_scan4->SetLineColor(kGreen+3);
  //fm_scan4->SetMarkerColor(kGreen+3);
  
  TLegend *l_FMscan = new TLegend(0.45,0.7,0.8,0.85);
  l_FMscan->SetNColumns(1);
  l_FMscan->AddEntry( fm_scan1 , "Thomas" , "lp");
  l_FMscan->AddEntry( fm_scan2 , "Gordon 1" , "lp");
  l_FMscan->AddEntry( fm_scan3 , "Gordon 2" , "lp");
  l_FMscan->AddEntry( fm_scan4 , "Raphael" , "lp");
  l_FMscan->Draw();

  //  c_uvB->Print("../../Plots/uvB/uvB_EpoxySteel_fv15_NewvOld.png");
  return;
}
