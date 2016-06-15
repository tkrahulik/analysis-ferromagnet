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
#include "TGraphErrors.h"
#include "TVector.h"
#include "TMath.h"
#include "TLegend.h"
#include "TLine.h"

using namespace std;

/* ============================
 * Ferromagnet B vs z Analysis
   ============================*/
/*
 * This functions reads in a data
 * file of B vs z measurements
 * and generates a plot.
 */

TGraph* plot_Bvz(
		const TString f_calib,
		double scale,
		double offset
)
{
  /*Read in Data File*/
  //Right now stored on Dropbox
  cout<< "processing file " << f_calib <<endl;
  ifstream infile(f_calib);
  /*Initialize Variables*/
  string l_calib;
  double z_temp, I_temp, B_temp, I_err_temp, B_err_temp;
  vector<double> z, z_err , I, I_err, B, B_err;
  /*Read in each line of data file*/
  /*Assign values in each line to data variables*/
  while( getline(infile, l_calib) )
    {
      /*Ignore any lines that start with # or // */
      if ( l_calib[0]=='#' || l_calib.substr(0, 2) == "//" )
	{
	  getline(infile,l_calib);
	  continue;
	}
      istringstream line( l_calib.c_str() );
      line >> z_temp;
      z.push_back( (-1.0)*(z_temp - offset) );
      z_err.push_back(0.5);
      line >> B_temp >> B_err_temp;
      B.push_back( TMath::Abs( B_temp*scale ) );
      B_err.push_back(B_err_temp);
      line >> I_temp >> I_err_temp;
      I.push_back( I_temp );
      I_err.push_back( I_err_temp );
    }
  /*Convert c++ vectors to ROOT TVectors*/
  TVectorD z_real(z.size(), &z[0]);
  TVectorD z_err_real(z.size(), &z_err[0]);
  TVectorD I_real(z.size(), &I[0]);
  TVectorD B_real(z.size(), &B[0]);
  TVectorD B_err_real(z.size(), &B_err[0]);
  /*Create B vs z Graph*/
  TGraphErrors *g_data = new TGraphErrors(z_real, B_real, z_err_real, B_err_real);

  return g_data;
}


void makePlot_Bvz()
{
  /*Create blank histogram to have a method of modifying axes and labels*/
  TCanvas *c_data = new TCanvas;
  TH1 *h_data = c_data->DrawFrame(-750.0, 0.0, 750.0, 5.0);
  //h_data->SetTitle("Epoxy/Steel, F_{v} = 0.3, B_{0} = 27.0mT");
  h_data->SetTitle("");
  h_data->GetXaxis()->SetTitle("z (mm)");
  h_data->GetXaxis()->SetTitleOffset(1.0);
  h_data->GetYaxis()->SetTitle("B_{0} (mT)");
  h_data->GetYaxis()->SetTitleOffset(1.0);

  /*Add Vertical Lines at Ends of Ferromagnet*/
  TLine *FM_start = new TLine(-508, 0.0,-508, 4.5);
  FM_start->SetLineStyle(1);
  FM_start->SetLineColor(kBlue);
  FM_start->Draw();
  TLine *FM_end = new TLine(508, 0.0 , 508, 4.5);
  FM_end->SetLineStyle(1);
  FM_end->SetLineColor(kBlue);
  FM_end->Draw();
  
  TGraphErrors *g_app = plot_Bvz("../Data/Bvz_Scan_Data/DataFile_150506_171251.txt", 27.0/25.0, 150.0);
  g_app->Draw("LP");
  g_app->SetLineColor(kBlue);
  g_app->SetMarkerColor(kBlue);
  TGraphErrors *g_FMscan_1 = plot_Bvz("../Data/Bvz_Scan_Data/DataFile_160613_170851.txt", 1.0, 600.0);
  g_FMscan_1->Draw("LP");
  g_FMscan_1->SetLineColor(kGreen+3);
  g_FMscan_1->SetMarkerColor(kGreen+3);
  TGraph *g_FMscan_2 = plot_Bvz("../Data/Bvz_Scan_Data/DataFile_150611_141440.txt", 1.0, 205.0);
  g_FMscan_2->Draw("LP");
  g_FMscan_2->SetLineColor(kRed+1);
  g_FMscan_2->SetMarkerColor(kRed+1);
  
  /*Create Legend*/
  TLegend *l_FMscan = new TLegend(0.5,0.75,0.8,0.85);
  l_FMscan->SetNColumns(1);
  l_FMscan->AddEntry( g_app , "Reference Field" , "lp");
  l_FMscan->AddEntry( g_FMscan_1 , "Superconductor Shield" , "lp");
  l_FMscan->AddEntry( FM_start , "End of Ferromagnet" , "l");
  l_FMscan->Draw();
  
  //c_data->Print("../../Plots/Bvz/Bvz_SC_Shield.png");
  //c_data->Print("../../Plots/Bvz/Bvz_SC_Shield.eps");
  return;
}
