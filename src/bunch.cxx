
//_____________________________________________________________________________
//
// Bunch in simulation
//
//_____________________________________________________________________________

//C++
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

//Boost
#include <boost/tokenizer.hpp>
#include <boost/program_options.hpp>

//ROOT
#include "TGraph.h"
#include "TDatabasePDG.h"
#include "TMath.h"
#include "TH1D.h"
#include "TF1.h"

//local classes
#include "bunch.h"

using namespace std;
using namespace boost;

//_____________________________________________________________________________
bunch::bunch(const char *n, const char *conf): nam(n) {

  cout << "bunch::bunch: " << n << endl;

  //configuration file
  program_options::options_description opt("opt");
  opt.add_options()
    ((nam+".input").c_str(), program_options::value<string>(), "Waveform input")
    ("main.tmin", program_options::value<double>(), "Minimal time, ns")
    ("main.tmax", program_options::value<double>(), "Maximal time, ns")
    ("main.Ep", program_options::value<double>(), "Beam energy, GeV")
    ("main.epsilon", program_options::value<double>(), "Emittance, nm")
    ("main.betas", program_options::value<double>(), "Beta*, cm")
    ("main.np", program_options::value<int>(), "Number of particles")
    ("main.nx", program_options::value<int>(), "Bins in x")
    ("main.xmin", program_options::value<double>(), "Minimum in x")
    ("main.xmax", program_options::value<double>(), "Maximum in x")
    ("main.ny", program_options::value<int>(), "Bins in y")
    ("main.ymin", program_options::value<double>(), "Minimum in y")
    ("main.ymax", program_options::value<double>(), "Maximum in y")
    ("main.nz", program_options::value<int>(), "Bins in z")
    ("main.zmin", program_options::value<double>(), "Minimum in z")
    ("main.zmax", program_options::value<double>(), "Maximum in z")
  ;

  //load the configuration file
  ifstream config(conf);
  program_options::variables_map opt_map;
  program_options::store(program_options::parse_config_file(config, opt, true), opt_map);

  //waveform input
  ifstream in( get_str(opt_map, nam+".input") );

  //time range, ns
  double tmin = opt_map["main.tmin"].as<double>();
  double tmax = opt_map["main.tmax"].as<double>();

  //lines to skip before the waveform
  int nskip = 27;

  double dt = 1./20; // sample rate is 20 samples per ns

  //input loop
  vector<double> wsig; // signal
  vector<double> wtim; // time
  string line; // read line
  int ilin = 0; // line index
  int ival = 0; // value index for time
  char_separator<char> sep(" "); // for tokenizer
  while( getline(in, line) ) {
    if( ++ilin < nskip ) continue;

    //split the line to individual numbers
    tokenizer< char_separator<char> > lin(line, sep);

    //values loop
    for(auto ilin = lin.begin(); ilin != lin.end(); ilin++) {

      //sample value
      istringstream st(*ilin);
      double val;
      st >> val;

      //sample time
      double tim = (ival++)*dt;

      //selection for time interval
      if( tim < tmin ) continue;
      if( tim > tmax ) {
        in.close();
        break;
      }

      wsig.push_back(val);
      wtim.push_back(tim);

    }//values loop

  }//input loop

  cout << "Values read: " << wsig.size() << endl;

  //beam beta
  double mp = TDatabasePDG::Instance()->GetParticle(2212)->Mass(); // GeV
  double Ep = opt_map["main.Ep"].as<double>(); // GeV
  double beta = TMath::Sqrt(Ep*Ep - mp*mp)/Ep;

  //velocity
  double cspeed = 299.792; // mm/ns
  vel = beta*cspeed; // mm/ns

  //range along z (mm) for interval from tmin to tmax
  double dz = vel*dt*wsig.size();

  cout << "vel (mm/ns): " << vel << ", dz (mm): " << dz << endl;

  //spatial distribution along z
  hz.SetBins(wsig.size(), -dz/2., dz/2.);
  hz.SetNameTitle(nam.c_str(), nam.c_str());
  hz.SetLineColor(gr.GetLineColor());
  hz.SetMarkerColor(gr.GetLineColor());

  //set the distribution from input signal
  gsig.Set(wsig.size());
  for(Int_t ibin=1; ibin<=hz.GetNbinsX(); ibin++) {
    if( wsig[ibin-1] < 0 ) continue;

    hz.SetBinContent(ibin, wsig[ibin-1]);
    hz.SetBinError(ibin, 0);

    //original signal shape
    gsig.SetPoint(ibin-1, wtim[ibin-1], wsig[ibin-1]);
  }

  //transverse width in x and y
  double epsilon = opt_map["main.epsilon"].as<double>(); // nm
  double betas = opt_map["main.betas"].as<double>(); // cm
  double sxy = TMath::Sqrt( epsilon*1e-6*betas*10 );
  cout << "sxy (mm): " << sxy << endl;

  //parametrization for transverse width
  TF1 fxy("fxy", "gaus", -4*sxy, 4*sxy);
  fxy.SetParameters(1, 0, sxy);

  //bunch particles
  for(int i=0; i<opt_map["main.np"].as<int>(); i++) {

    points.push_back( TVector3(fxy.GetRandom(), fxy.GetRandom(), hz.GetRandom()) );
    //cout << points.back().X() << " " << points.back().Y() << " " << points.back().Z() << endl;
  }

  //points representation
  gr.Set(points.size());
  gr.SetMarkerColor(kBlue);
  gr.SetMarkerStyle(kFullCircle);
  gr.SetMarkerSize(0.2);

  //range and bins
  int nx = opt_map["main.nx"].as<int>();
  double xmin = opt_map["main.xmin"].as<double>();
  double xmax = opt_map["main.xmax"].as<double>();
  int ny = opt_map["main.ny"].as<int>();
  double ymin = opt_map["main.ymin"].as<double>();
  double ymax = opt_map["main.ymax"].as<double>();
  int nz = opt_map["main.nz"].as<int>();
  double zmin = opt_map["main.zmin"].as<double>();
  double zmax = opt_map["main.zmax"].as<double>();

  hxyz.SetBins(nx, xmin, xmax, ny, ymin, ymax, nz, zmin, zmax);

  //initial particle distribution
  hxyz.Reset();

  for(auto i: points) {

    hxyz.Fill(i.x(), i.y(), i.z());
  }

}//bunch

//_____________________________________________________________________________
void bunch::move(double dt) {

  //dt in ns
  double ds = vel*dt;
  auto delt = TVector3(ds*dir.x(), ds*dir.y(), ds*dir.z());

  for(int i=0; i<points.size(); i++) {

    points[i] += delt;
  }

  //particle distribution
  hxyz.Reset();

  for(auto i: points) {

    hxyz.Fill(i.x(), i.y(), i.z());
  }

}//move

//_____________________________________________________________________________
void bunch::draw() {

  for(unsigned long i=0; i<points.size(); i++) {

    gr.SetPoint(i, points[i].z(), points[i].x());

  }

  //gr.Draw("psame");
  gr.Draw("*same");

}//draw

//_____________________________________________________________________________
void bunch::draw_input() {

  gsig.SetMarkerColor(gr.GetMarkerColor());
  gsig.SetMarkerStyle(kFullCircle);
  gsig.SetMarkerSize(0.2);

  gsig.Draw("*same");

}//draw_input

//_____________________________________________________________________________
string bunch::get_str(program_options::variables_map& opt_map, std::string par) {

  string res = opt_map[par].as<string>();
  res.erase(remove(res.begin(), res.end(), '\"'), res.end());

  return res;

}//get_str

































