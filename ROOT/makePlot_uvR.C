
void makePlot_uvR()
{

  double r = 10;

  TF1 *f_mu = new TF1("f_mu", "(x**2 + [0]**2) / (x**2 - [0]**2)", 10 ,15);
  f_mu->SetParameter(0, r);  
  f_mu->SetTitle("Magnetic Permeability as a Function of Outer Radius, R_{2}");
  f_mu->GetXaxis()->SetTitle("R_{2} (mm)");
  f_mu->GetYaxis()->SetTitle("#mu_{r}");
  TCanvas *c_fmu = new TCanvas();
  f_mu->Draw();
  f_mu->SetMaximum(45.0);

  return;
}
