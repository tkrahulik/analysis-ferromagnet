/*==========Permeability Plotting Macro===========
 * Project: A Compact Magnetic Field Cloaking Device
 * Author: Thomas Krahulik
 * Date Updated: September 21, 2016
 * Purpose: To plot relative magnetic permeability
 *          of ferromagnets as a function of
 *          external magnetic field
 * To run macro: root -l makePlot_uvB.C
 =================================================*/
/*
 * Quantify Uncertainty
 */
double ratio(const char* f_inner, const char* f_outer)
{
  TTree *TDiIn = new TTree();
  TDiIn->ReadFile(f_inner, "di/D");
  TH1D *h_din = new TH1D("h_din", "", 10000, 0, 100);
  TDiIn->Draw("di>>h_din", "", "goff");
  Double_t d_inner = h_din->GetMean(1);
  //  Double_t sig_inner = h_din->GetMeanError(1);
  //  cout << "Mean Inner Diameter: " << d_inner  << " +/- " << sig_inner << endl;

  //  cout<< "Reading Outer Diameters..." << f_outer <<endl;
  TTree *TDiOut = new TTree();
  TDiOut->ReadFile(f_outer, "do/D");
  TH1D *h_dout = new TH1D("h_dout", "", 10000, 0, 100);
  TDiOut->Draw("do>>h_dout", "", "goff");
  Double_t d_outer = h_dout->GetMean(1);
  //  Double_t sig_outer = h_dout->GetMeanError(1);
  //  cout << "Mean Outer Diameter: " << d_outer << " +/- " << sig_outer << endl;

  double Ratio = d_inner/d_outer;
  h_din->Delete();
  h_dout->Delete();
  return Ratio;
}

double r_sig(const char* f_inner, const char* f_outer)
{
  TTree *TDiIn = new TTree();
  TDiIn->ReadFile(f_inner, "di/D");
  TH1D *h_din = new TH1D("h_din", "", 10000, 0, 100);
  TDiIn->Draw("di>>h_din", "", "goff");
  Double_t d_inner = h_din->GetMean(1);
  Double_t sig_inner = h_din->GetMeanError(1);
  //  cout << "Mean Inner Diameter: " << d_inner  << " +/- " << sig_inner << endl;

  //  cout<< "Reading Outer Diameters..." << f_outer <<endl;
  TTree *TDiOut = new TTree();
  TDiOut->ReadFile(f_outer, "do/D");
  TH1D *h_dout = new TH1D("h_dout", "", 10000, 0, 100);
  TDiOut->Draw("do>>h_dout", "", "goff");
  Double_t d_outer = h_dout->GetMean(1);
  Double_t sig_outer = h_dout->GetMeanError(1);
  //  cout << "Mean Outer Diameter: " << d_outer << " +/- " << sig_outer << endl;

  double Ratio = d_inner/d_outer;
  double Sigma = Ratio*sqrt( pow((sig_inner/d_inner),2) + pow((sig_outer/d_outer),2)  );
  //  cout << Sigma << endl;
  h_din->Delete();
  h_dout->Delete();

  return Sigma;
}

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
TF1* Calibrate(
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
  // g_calib->Draw("AP");
  g_calib->Fit("pol1", "q");
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
TGraphErrors* plot_uvB(
			 const TString scan_file,
			 TF1* calib_fit,
			 double R,
			 double R_sig
)
{

  /*Read in Data File to ROOT Tree*/
  cout<< "processing file " << scan_file <<endl;
  TTree *TData = new TTree();
  TData->ReadFile(scan_file, "t/D:I:B");
  /*Use TTree Draw command to write branches to usable arrays*/
  int n = TData->Draw("I:TMath::Abs(B):0.0:0.005", "", "goff");

  vector<double> B_ext, B_in, Bratio, u;
  for(int i = 0; i < n; i++)
    {
      B_ext.push_back( calib_fit->Eval(TData->GetV1()[i]) );
      B_in.push_back( TData->GetV2()[i] );
      Bratio.push_back( B_in[i] / B_ext[i]);
      u.push_back( (Bratio[i]*(R**2) + Bratio[i] - 2 - 2*sqrt((Bratio[i]**2)*(R**2) - Bratio[i]*(R**2) - Bratio[i] + 1 ) ) / (Bratio[i]*(R**2) - Bratio[i]) );

    }

  TGraphErrors *g_uvB = new TGraphErrors(n, &B_ext[0], &u[0], TData->GetV3(), TData->GetV4());

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


  const bool ideal_cloak = true;

  const bool plot_FM_Fm602 = true;
  const bool plot_FM_Fm625 = false;
  const bool plot_FM_Fm612 = true;
  const bool plot_FM_Fm651 = true;

  TCanvas *c_uvB = new TCanvas();
  TH1 *h_uvB = c_uvB->DrawFrame(0, 1.0, 60, 4.5);
  h_uvB->SetTitle("Permeability of Epoxy/Steel Ferromagnets");
  h_uvB->GetXaxis()->SetTitle("B_{0} (mT)");
  h_uvB->GetXaxis()->SetTitleOffset(1.5);
  h_uvB->GetYaxis()->SetTitle("#mu_{r}");
  h_uvB->GetYaxis()->SetTitleOffset(1.0);
  /*Create Legend*/
  TLegend *leg_uvB = new TLegend(0.55,0.72,0.8,0.87);
  leg_uvB->SetNColumns(1);
  leg_uvB->Draw();

  vector<double> u, Fm;

  // u.push_back(1.0);
  // Fm.push_back(0.0);
  // u.push_back(550.);
  // Fm.push_back(1.0);

  if(plot_FM_Fm651)
    {
      /*
       * Permeability Plot for New FM = 0.602 Ferromagnet
       */
      /*Run calibration*/
      const TString calib_fm651 = "../Data/Calib_Data/DataFile_160916_211714.txt";
      const TString fmscan_fm651 = "../Data/FMScan_Data/DataFile_160916_212729_Part2.txt";
      TF1 *calib_fit1 = Calibrate(calib_fm651);
      /*Calculate Radius Ratio and Uncertainty*/
      const TString di_file = "../Data/Calib_Data/fm503_di.txt";
      const TString do_file = "../Data/Calib_Data/fm503_do.txt";
      double R_fm651 = ratio(di_file, do_file);
      double R_sig_fm651 = r_sig(di_file, do_file);
      /*Plot u vs B for FM*/
      TGraphErrors *g_fm651 = plot_uvB(fmscan_fm651, calib_fit1, R_fm651, R_sig_fm651);
      g_fm651->Draw("LP");
      g_fm651->SetLineColor(kViolet);
      g_fm651->SetMarkerColor(kViolet);
      TF1 *fit_fm651 = new TF1("fit_fm651", "[0]/x + [1]", 0, 60);
      fit_fm651->SetLineStyle(2);
      fit_fm651->SetLineColor(kViolet);
      g_fm651->Fit("fit_fm651");
      leg_uvB->AddEntry( g_fm651 , "F_{m} = 0.651" , "lp");

      u.push_back( fit_fm651->Eval(50.) );
      Fm.push_back( 0.651 );
    }

  if(plot_FM_Fm602)
    {
      /*
       * Permeability Plot for New FM = 0.602 Ferromagnet
       */
      /*Run calibration*/
      const TString calib_fm602 = "../Data/Calib_Data/DataFile_160727_162618.txt";
      const TString fmscan_fm602 = "../Data/FMScan_Data/DataFile_160727_163533.txt";
      TF1 *calib_fit1 = Calibrate(calib_fm602);
      /*Calculate Radius Ratio and Uncertainty*/
      const TString di_file = "../Data/Calib_Data/fm612_di.txt";
      const TString do_file = "../Data/Calib_Data/fm612_do.txt";
      double R_fm602 = ratio(di_file, do_file);
      double R_sig_fm602 = r_sig(di_file, do_file);
      /*Plot u vs B for FM*/
      TGraphErrors *g_fm602 = plot_uvB(fmscan_fm602, calib_fit1, R_fm602, R_sig_fm602);
      g_fm602->Draw("LP");
      g_fm602->SetLineColor(kRed);
      g_fm602->SetMarkerColor(kRed);
      TF1 *fit_fm602 = new TF1("fit_fm602", "[0]/x + [1]", 0, 60);
      fit_fm602->SetLineStyle(2);
      fit_fm602->SetLineColor(kRed);
      g_fm602->Fit("fit_fm602");
      leg_uvB->AddEntry( g_fm602 , "F_{m} = 0.602" , "lp");

      u.push_back( fit_fm602->Eval(50.) );
      Fm.push_back( 0.602 );
    }

  if(plot_FM_Fm612)
    {
      /*
       * Permeability Plot for New FM = 0.612 Ferromagnet
       */
      /*Run calibration*/
      const TString calib_fm612 = "../Data/Calib_Data/DataFile_160805_142716.txt";
      const TString fmscan_fm612 = "../Data/FMScan_Data/DataFile_160805_143508.txt";
      TF1 *calib_fit1 = Calibrate(calib_fm612);
      /*Calculate Radius Ratio and Uncertainty*/
      const TString di_file = "../Data/Calib_Data/fm612_di.txt";
      const TString do_file = "../Data/Calib_Data/fm612_do.txt";
      double R_fm612 = ratio(di_file, do_file);
      double R_sig_fm612 = r_sig(di_file, do_file);
      /*Plot u vs B for FM*/
      TGraphErrors *g_fm612 = plot_uvB(fmscan_fm612, calib_fit1, R_fm612, R_sig_fm612);
      g_fm612->Draw("LP");
      g_fm612->SetLineColor(kBlue+2);
      g_fm612->SetMarkerColor(kBlue+2);
      TF1 *fit_fm612 = new TF1("fit_fm612", "[0]/x + [1]", 0, 60);
      fit_fm612->SetLineStyle(2);
      fit_fm612->SetLineColor(kBlue+2);
      g_fm612->Fit("fit_fm612");
      // cout << fit_fm612->Eval(500) << endl;
      leg_uvB->AddEntry( g_fm612 , "F_{m} = 0.612" , "lp");
      u.push_back( fit_fm612->Eval(50.) );
      Fm.push_back( 0.612 );
    }

  if(plot_FM_Fm625)
    {
      /*
       * Permeability Plot for New FM = 0.625 Ferromagnet
       */
      /*Run calibration*/
      const TString calib_fm625 = "../Data/Calib_Data/DataFile_160801_121348.txt";
      const TString fmscan_fm625 = "../Data/FMScan_Data/DataFile_160801_122800.txt";
      TF1 *calib_fit1 = Calibrate(calib_fm625);
      /*Calculate Radius Ratio and Uncertainty*/
      const TString di_file = "../Data/Calib_Data/fm612_di.txt";
      const TString do_file = "../Data/Calib_Data/fm612_do.txt";
      double R_fm625 = ratio(di_file, do_file);
      double R_sig_fm625 = r_sig(di_file, do_file);
      /*Plot u vs B for FM*/
      TGraphErrors *g_fm625 = plot_uvB(fmscan_fm625, calib_fit1, R_fm625, R_sig_fm625);
      g_fm625->Draw("LP");
      g_fm625->SetLineColor(kGreen+2);
      g_fm625->SetMarkerColor(kGreen+2);
      // TF1 *fit_fm625 = new TF1("fit_fm625", "[0]/x + [1]", 0, 60);
      // fit_fm625->SetLineStyle(2);
      // fit_fm625->SetLineColor(kBlue+2);
      // g_fm625->Fit("fit_fm625");
      leg_uvB->AddEntry( g_fm625 , "F_{m} = 0.625" , "lp");
    }

  if(ideal_cloak)
    {
      /*Calculate Radius Ratio and Uncertainty*/
      const TString di_file = "../Data/Calib_Data/fm503_di.txt";
      const TString do_file = "../Data/Calib_Data/fm503_do.txt";
      double R_fv30 = ratio(di_file, do_file);
      // cout << R_fv30 << endl;
      double R_sig_fv30 = r_sig(di_file, do_file);
      // cout << R_sig_fv30 << endl;
      /*Calculate theoretical permeability of ferromagnet*/
      double u_cloak = (1 + pow(R_fv30,2)) / (1 - pow(R_fv30,2));
      cout << "Desired Permeability: " << u_cloak << endl;
      /*Draw Line for Permeability of Ideal Cloak*/
      TLine *l_ucloak = new TLine(0.0, u_cloak, 60.0, u_cloak);
      l_ucloak->SetLineStyle(2);
      //      l_ucloak->SetLineColor(kRed);
      l_ucloak->Draw();
      leg_uvB->AddEntry( l_ucloak , "Ideal Cloak" , "l");
    }

  /*
   * Plot of Permeability vs Fractional Mass of Steel Powder in Ferromagnet
   */
  TGraph *g_uvFm = new TGraph(Fm.size(), &(Fm[0]), &(u[0]) );
  TCanvas *c_uvFm = new TCanvas();
  g_uvFm->Draw("AP");
  // g_uvFm->Fit("expo");
  g_uvFm->SetTitle("#mu_{r} vs F_{m}");
  g_uvFm->GetXaxis()->SetTitle("F_{m}");
  g_uvFm->GetYaxis()->SetTitle("#mu_{r}");

  /*Save plots to png and eps files*/     
  /*    
  c_uvB->Print("../../Plots/uvB/EpoxySteel_uvB_160916.png");
  c_uvB->Print("../../Plots/uvB/EpoxySteel_uvB_160916.eps");
  */
  return;
}
