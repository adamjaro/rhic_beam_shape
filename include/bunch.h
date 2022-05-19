#ifndef bunch_h
#define bunch_h

//bunch in simulation

#include "TH1D.h"
#include "TVector3.h"
#include "TH3D.h"

class bunch {

  public:

    bunch(const char *n, const char *conf);

    void move(double dt);

    void set_direction(double dx, double dy, double dz) { dir.SetXYZ(dx, dy, dz); }

    TH3D& get_hxyz() { return hxyz; }
    TGraph *get_graph() { return &gr; }

    void draw();
    void draw_input();

  private:

    std::string get_str(boost::program_options::variables_map& opt_map, std::string par);

    std::string nam; // bunch name (blue or yellow)

    TGraph gsig; // input signal vs. time
    TH1D hz; // shape along z in mm

    double vel; // beam velocity, mm/ns
    TVector3 dir; // direction unit vector

    std::vector<TVector3> points; // particles in bunch
    TGraph gr; // graphical xz representation

    TH3D hxyz; // particle distribution in x, y and z

};

extern "C" {

  //bunch* make_bunch(const char *n, const char *c) { return new bunch(n, c); }

  //void run_AnaMapsBasic(AnaMapsBasic& t, const char *c) { t.Run(c); }

}

#endif

