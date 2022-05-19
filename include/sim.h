#ifndef sim_h
#define sim_h

//simulation implementation

#include "bunch.h"

#include "TH1D.h"
#include "TH2D.h"

class sim {

  public:

    sim(const char *conf);

    void move(double dt);

    void draw();
    void draw_xy();
    void draw_z();

    void run_evolution();

    void draw_xt() { hxt.Draw(); }
    void draw_yt() { hyt.Draw(); }
    void draw_zt() { hzt.Draw(); }

    void draw_input();

    int get_izmax() { return izmax; }

  private:

    void make_pairs();

    std::vector<bunch*> bun; // bunches in simulation

    TH2D hxy; // pairs distribution in x and y
    TH1D hz; // pairs distribution in z
    TH1D hxt, hyt, hzt; // time integrals in pairs along x, y and z

    double ixymax; // initial maximum in xy
    int izmax; // initial maximum in z

    double et0; // start time for evolution
    double et1; // stop time for evolution
    int nstep; // steps for evolution

};

extern "C" {

  sim* make_sim(const char *c) { return new sim(c); }

  void sim_move(sim *s, double dt) { s->move(dt); }

  void sim_draw(sim *s) { s->draw(); }
  void sim_draw_xy(sim *s) { s->draw_xy(); }
  void sim_draw_z(sim *s) { s->draw_z(); }

  int sim_get_izmax(sim *s) { return s->get_izmax(); }

  void sim_run_evolution(sim *s) { s->run_evolution(); }

  void sim_draw_xt(sim *s) { s->draw_xt(); }
  void sim_draw_yt(sim *s) { s->draw_yt(); }
  void sim_draw_zt(sim *s) { s->draw_zt(); }

  void sim_draw_input(sim *s) { s->draw_input(); }

}

#endif

