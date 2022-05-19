
//_____________________________________________________________________________
//
// Simulation implementation
//
//_____________________________________________________________________________

//C++
#include <iostream>
#include <fstream>

//Boost
#include <boost/tokenizer.hpp>
#include <boost/program_options.hpp>

//ROOT
#include "TGraph.h"
#include "TCanvas.h"
#include "TFile.h"

//local classes
#include "sim.h"

using namespace std;
using namespace boost;

//_____________________________________________________________________________
sim::sim(const char *conf) {

  cout << "Hi from sim" << endl;

  //blue beam
  bun.push_back( new bunch("blue", conf) );
  bun.back()->get_graph()->SetMarkerColor(kBlue);
  bun.back()->set_direction(0, 0, -1);

  //yellow beam
  bun.push_back( new bunch("yellow", conf) );
  bun.back()->get_graph()->SetMarkerColor(kYellow);
  bun.back()->set_direction(0, 0, 1);

  //configuration file
  program_options::options_description opt("opt");
  opt.add_options()
    ("main.nx", program_options::value<int>(), "Bins in x")
    ("main.xmin", program_options::value<double>(), "Minimum in x, mm")
    ("main.xmax", program_options::value<double>(), "Maximum in x, mm")
    ("main.ny", program_options::value<int>(), "Bins in y")
    ("main.ymin", program_options::value<double>(), "Minimum in y")
    ("main.ymax", program_options::value<double>(), "Maximum in y")
    ("main.nz", program_options::value<int>(), "Bins in z")
    ("main.zmin", program_options::value<double>(), "Minimum in z")
    ("main.zmax", program_options::value<double>(), "Maximum in z")
    ("main.et0", program_options::value<double>(), "Start time, ns")
    ("main.et1", program_options::value<double>(), "Stop time, ns")
    ("main.nstep", program_options::value<int>(), "Number of steps")
  ;

  //load the configuration file
  ifstream config(conf);
  program_options::variables_map opt_map;
  program_options::store(program_options::parse_config_file(config, opt, true), opt_map);

  //range and bins for pairs
  int nx = opt_map["main.nx"].as<int>();
  double xmin = opt_map["main.xmin"].as<double>();
  double xmax = opt_map["main.xmax"].as<double>();
  int ny = opt_map["main.ny"].as<int>();
  double ymin = opt_map["main.ymin"].as<double>();
  double ymax = opt_map["main.ymax"].as<double>();
  int nz = opt_map["main.nz"].as<int>();
  double zmin = opt_map["main.zmin"].as<double>();
  double zmax = opt_map["main.zmax"].as<double>();

  hxy.SetBins(nx, xmin, xmax, ny, ymin, ymax);
  hz.SetBins(nz, zmin, zmax);

  make_pairs();

  ixymax = hxy.GetMaximum();
  izmax = hz.GetMaximum();

  hxt.SetBins(nx, xmin, xmax);
  hyt.SetBins(ny, ymin, ymax);
  hzt.SetBins(nz, zmin, zmax);

  hxt.SetNameTitle("hxt", "hxt");
  hyt.SetNameTitle("hyt", "hyt");
  hzt.SetNameTitle("hzt", "hzt");

  //time range and steps for evolution
  et0 = opt_map["main.et0"].as<double>();
  et1 = opt_map["main.et1"].as<double>();
  nstep = opt_map["main.nstep"].as<int>();

}//sim

//_____________________________________________________________________________
void sim::move(double dt) {

  for(auto i: bun) {

    i->move(dt);
  }

  make_pairs();

}//move

//_____________________________________________________________________________
void sim::make_pairs() {

  hxy.Reset();
  hz.Reset();

  bunch *b0 = bun[0];
  bunch *b1 = bun[1];

  //pairs in bunches
  for(int ix=0; ix<hxy.GetNbinsX()+1; ix++) {
    for(int iy=0; iy<hxy.GetNbinsY()+1; iy++) {
      for(int iz=0; iz<hz.GetNbinsX()+1; iz++) {

        double nb0 = b0->get_hxyz().GetBinContent(ix, iy, iz);
        double nb1 = b1->get_hxyz().GetBinContent(ix, iy, iz);
 
        double npair = min(nb0, nb1);

        hxy.SetBinContent(ix, iy, npair+hxy.GetBinContent(ix, iy));
        hz.SetBinContent(iz, npair+hz.GetBinContent(iz));

      }//z
    }//y
  }//x

}//make_pairs

//_____________________________________________________________________________
void sim::run_evolution() {

  //bunches evolution over time interval in steps

  double dt = (et1-et0)/nstep;

  move(et0-dt);

  for(int i=0; i<nstep; i++) {

    cout << i << endl;

    move(dt);

    for(int ix=0; ix<hxy.GetNbinsX()+1; ix++) {
      for(int iy=0; iy<hxy.GetNbinsY()+1; iy++) {

        hxt.AddBinContent(ix, hxy.GetBinContent(ix, iy));
        hyt.AddBinContent(iy, hxy.GetBinContent(ix, iy));

      }//y
    }//x

    for(int iz=0; iz<hz.GetNbinsX()+1; iz++) {

        hzt.AddBinContent(iz, hz.GetBinContent(iz));

    }//z

  }//dt

  TFile out("sim.root", "recreate");

  hxt.Write("hxt");
  hyt.Write("hyt");
  hzt.Write("hzt");

  out.Close();

}//run_evolution

//_____________________________________________________________________________
void sim::draw() {

  for_each(bun.begin(), bun.end(), mem_fun( &bunch::draw ));

}//draw

//_____________________________________________________________________________
void sim::draw_xy() {

  hxy.SetMinimum(0.98);
  hxy.SetMaximum(ixymax);
  hxy.SetContour(300);

  hxy.Draw("colz same");

}//draw_xy

//_____________________________________________________________________________
void sim::draw_z() {

  hz.SetFillColor(kBlue);

  hz.Draw("same");

}//draw_z

//_____________________________________________________________________________
void sim::draw_input() {

  for(auto i: bun) {

    i->draw_input();
  }

}//draw_input






















