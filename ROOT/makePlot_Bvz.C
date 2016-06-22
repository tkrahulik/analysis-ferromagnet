/*==============B vs z Plotting Macro=============
 * Project: A Compact Magnetic Field Cloaking Device
 * Author: Thomas Krahulik
 * Date: June 22, 2016
 * Purpose: To plot B vs z measurements
 * To run macro: root -l makePlot_Bvz.C
 =================================================*/

/* =================================
 * B vs z Analysis Plotting Function
   =================================*/
/*
 * This functions reads in a data
 * file of B vs z measurements
 * and generates a plot.
 */

TGraphErrors* plot_Bvz(
		const TString f_data,
		double offset
)
{
  /*Read in Data File*/
  cout << "Processing File " << f_data << endl;
  /*Read in B vs z data to a ROOT tree*/
  TTree *TData = new TTree();
  TData->ReadFile(f_data, "z/D:B:B_err:I:I_err");
  /*Obtain number of points and set branches of tree to arrays*/
  Int_t n = TData->Draw("z:TMath::Abs(B):0.5:B_err" , "", "goff");
  /*Graph B vs z data points*/
  TGraphErrors *g_Bvz = new TGraphErrors(n, TData->GetV1(), TData->GetV2(), TData->GetV3(), TData->GetV4());

  /*Center */
  for (int i = 0; i < g_Bvz->GetN(); i++)
    {
      g_Bvz->SetPoint( i, g_Bvz->GetX()[i] - offset, g_Bvz->GetY()[i] );
      g_Bvz->SetPointError(i , sqrt((g_Bvz->GetEX()[i])**2 + (0.5)**2) , g_Bvz->GetEY()[i] );
    }

  return g_Bvz;

}

/*===============================================
 * Plotting B vs z for Collection of Measurements
 ================================================*/
/*
 * Plot and compare several measurements of B vs z
 * Calling format function for each plot:
 * plot_Bvz("/path/to/data_file.txt", z_offset)
 */

void makePlot_Bvz()
{
  /*Create blank histogram to have a method of modifying axes and labels*/
  TCanvas *c_data = new TCanvas;
  /*To set ranges: DrawFrame(x_min, y_min, x_max, y_max)*/
  TH1 *h_data = c_data->DrawFrame(-100.0, 23.0, 100.0, 26.0);
  h_data->SetTitle("");
  h_data->GetXaxis()->SetTitle("z (mm)");
  h_data->GetXaxis()->SetTitleOffset(1.0);
  h_data->GetYaxis()->SetTitle("B_{0} (mT)");
  h_data->GetYaxis()->SetTitleOffset(1.0);

  /*Add Vertical Lines at Important Points*/
  /*To set locations of lines: TLine(x_min, y_min, x_max, y_max)*/
  TLine *FM_start = new TLine(-62.5, 24.0,-62.5, 25.0);
  FM_start->SetLineStyle(2);
  FM_start->SetLineColor(kGreen+2);
  FM_start->Draw();
  TLine *FM_end = new TLine(62.5, 24.0 , 62.5, 25.0);
  FM_end->SetLineStyle(2);
  FM_end->SetLineColor(kGreen+2);
  FM_end->Draw();
  TLine *center = new TLine(0.0, 24.0 , 0.0, 25.0);
  center->SetLineStyle(2);
  center->Draw();


  /*Plot B vs z for a Reference Field*/  
  TGraphErrors *g_Ref = plot_Bvz("../Data/Bvz_Scan_Data/DataFile_160622_104925.txt", 192.0);
  g_Ref->Draw("LP");
  g_Ref->SetLineColor(kBlue);
  g_Ref->SetMarkerColor(kBlue);

  /*Plot B vs z for First Measurement*/
  TGraphErrors *g_BvzScan1 = plot_Bvz("../Data/Bvz_Scan_Data/DataFile_160622_110013.txt", 192.0);
  g_BvzScan1->Draw("LP");
  g_BvzScan1->SetLineColor(kGreen+2);
  g_BvzSacn1->SetMarkerColor(kGreen+2);
  g_BvzScan1->Fit("pol2", "", "", -50, 50);

  /*Plot B vs z for Second Measurement*/
  /*
  TGraph *g_BvzScan2 = plot_Bvz("../Data/Bvz_Scan_Data/DataFile_150611_141440.txt", 205.0);
  g_BvzScan2->Draw("LP");
  g_BvzScan2->SetLineColor(kRed+1);
  g_BvzScan2->SetMarkerColor(kRed+1);
  */

  /*Create Legend*/
  TLegend *l_FMscan = new TLegend(0.5,0.75,0.8,0.85);
  l_FMscan->SetNColumns(1);
  l_FMscan->AddEntry( g_Ref , "Reference Field" , "lp");
  l_FMscan->AddEntry( g_FMscan_1 , "Ferromagnet" , "lp");
  l_FMscan->AddEntry( FM_start , "End of Ferromagnet" , "l");
  l_FMscan->Draw();
  
  /*Save plots to png and eps files*/
  /*
   c_data->Print("../../Plots/Bvz/Bvz_SC_Shield.png");
   c_data->Print("../../Plots/Bvz/Bvz_SC_Shield.eps");
  */
  return;
}
