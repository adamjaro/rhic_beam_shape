#!/usr/bin/python3

import sys
from ctypes import CDLL, c_char_p, c_double, c_void_p
import os

import ROOT as rt
from ROOT import gPad, gROOT, gStyle, gSystem, TCanvas, TH1D
from ROOT import TIter

sys.path.append("./python")
import plot_utils as ut
from read_con import read_con

#_____________________________________________________________________________
def main():

    #configuration from command line
    config = get_config()
    con = read_con(config)

    #analysis library
    lib = CDLL("build/librhic_beam_shape.so")

    #simulation instance
    lib.make_sim.restype = c_void_p
    sim = c_void_p( lib.make_sim( c_char_p(bytes(config, "utf-8")) ) )

    print(con.str("mode"))

    #functions
    func = {}
    func["evolution"] = evolution
    func["video"] = video
    func["draw_xz"] = draw_xz
    func["draw_input"] = draw_input

    #call the function selected in the configuration
    func[con.str("mode")](lib, sim, con)

#main

#_____________________________________________________________________________
def draw_xz(lib, sim, con):

    c1 = TCanvas("c1","c1",1200,900)
    frame = gPad.DrawFrame(-2e4, -1, 2e4, 1) # xmin, ymin, xmax, ymax in ROOT

    lib.sim_move(sim, c_double(-20)) # ns

    lib.sim_draw(sim)

    gPad.SetGrid()

    ut.invert_col(gPad)
    c1.SaveAs("01fig.pdf")

#draw_xz

#_____________________________________________________________________________
def draw_input(lib, sim, con):

    c1 = TCanvas("c1","c1",800,800)
    frame = gPad.DrawFrame(con("tmin"), -1, con("tmax"), 4) # xmin, ymin, xmax, ymax in ROOT

    lib.sim_draw_input(sim)

    gPad.SetGrid()

    ut.invert_col(gPad)
    c1.SaveAs("01fig.pdf")

#draw_input

#_____________________________________________________________________________
def evolution(lib, sim, con):

    lib.sim_run_evolution(sim)

    can = TCanvas("c1", "c1", 800, 800)

    lib.sim_draw_zt(sim)
    hzt = ut.get_obj(gPad, "hzt")

    hzt.SetLineWidth(2)
    hzt.SetLineColor(rt.kBlue)

    hzt.SetXTitle("#it{z} (mm)")
    hzt.SetYTitle("Counts")

    #ut.put_yx_tit(hzt, "Counts", "#it{z} (mm)", 1.7, 1.3)

    ut.set_margin_lbtr(gPad, 0.15, 0.1, 0.015, 0.05)

    gPad.SetGrid()

    ut.invert_col(gPad)

    can.SaveAs("01fig.pdf")

#evolution

#_____________________________________________________________________________
def video(lib, sim, con):

    #ns
    tmin = con.flt("tmin", "video")
    tmax = con.flt("tmax", "video")
    nstep = con.int("nstep", "video")

    dt = float(tmax-tmin)/nstep
    time = tmin-dt
    lib.sim_move(sim, c_double(time))

    can = TCanvas("c1","c1",950,950)

    #temporary directory
    #os.system("rm -f "+out)
    os.system("rm -rf tmp")
    os.system("mkdir tmp")

    for i in range(nstep):

        lib.sim_move(sim, c_double(dt))

        time += dt
        can.Clear()

        nam = "tmp/fig_"+"{0:04d}".format(i)+".png"
        create_plot_pairs(lib, sim, can, nam, time)#, cross_angle, can, zpmax, time, nam)

    #command to make the video
    os.system(con.str("cmd", "video"))

    #remove the temporary directory
    os.system("rm -rf tmp")

#video

#_____________________________________________________________________________
def create_plot_pairs(lib, sim, can, nam, time):

    #lib.sim_move(sim, c_double(5)) # ns

    zmin = -1.4e4
    zmax = 1.4e4

    xmin = -1
    xmax = 1

    tsiz = 0.045
    tsiz2 = 0.04

    invert = True

    #electron and proton beams
    #beam_el = beam_lin(zmin, zmax)
    #beam_p = beam_lin(zmin, zmax, -cross_angle)
    #beam_p.col = rt.kRed

    #can = TCanvas("c1","c1",950,950)

    can.SetMargin(0, 0, 0, 0)
    #can = TCanvas("c1","c1",1086, 543)
    can.Divide(1, 2, 0, 0)

    can.cd(1)

    frame = gPad.DrawFrame(zmin, xmin, zmax, xmax) # xmin, ymin, xmax, ymax in ROOT
    ut.put_frame_yx_tit(frame, "#it{x} (mm)", "#it{z} (mm)", 0.6, 1.2)
    ut.set_frame_text_size(frame, tsiz)
    ut.set_margin_lbtr(gPad, 0.06, 0.11, 0.03, 0.01)
    gPad.SetGrid()
    #frame.Draw()

    #beam_el.draw()
    #beam_p.draw()

    leg = ut.prepare_leg(0.75, 0.73, 0.15, 0.22, tsiz)
    hle = TH1D()
    hle.SetMarkerColor(rt.kBlue)
    hle.SetMarkerStyle(rt.kFullCircle)
    hlp = TH1D()
    hlp.SetMarkerColor(rt.kYellow)
    hlp.SetMarkerStyle(rt.kFullCircle)
    leg.AddEntry("", "#it{t} = "+"{0:.2f}".format(time)+" ns", "")
    leg.AddEntry(hle, "Blue beam", "p")
    leg.AddEntry(hlp, "Yellow beam", "p")
    leg.Draw("same")

    lib.sim_draw(sim)

    if invert: ut.invert_col(gPad)

    can.cd(2)
    gPad.Divide(2, 1, 0, 0)

    can.cd(2)
    gPad.cd(1)

    frame2 = gPad.DrawFrame(-1, -1, 1, 1) # xmin, ymin, xmax, ymax in ROOT
    ut.put_frame_yx_tit(frame2, "#it{y} (mm)", "#it{x} (mm)", 1.5, 1.2)
    ut.set_frame_text_size(frame2, tsiz2)
    ut.set_margin_lbtr(gPad, 0.12, 0.14, 0.02, 0.1)
    gPad.SetGrid()
    #frame2.Draw()

    lib.sim_draw_xy(sim)

    if invert: ut.invert_col(gPad)

    can.cd(2)
    gPad.cd(2)

    zpmax = lib.sim_get_izmax(sim)
    frame3 = gPad.DrawFrame(-4000, 0, 4000, zpmax*1.2)
    ut.put_frame_yx_tit(frame3, "Bunch overlap (a.u.)", "#it{z} (mm)", 1.5, 1.2)
    frame3.GetYaxis().CenterTitle(rt.kFALSE)
    ut.set_margin_lbtr(gPad, 0.14, 0.14, 0.02, 0.04)
    ut.set_frame_text_size(frame3, tsiz2)
    gPad.SetGrid()
    frame3.Draw()

    lib.sim_draw_z(sim)

    if invert: ut.invert_col(gPad)

    can.SaveAs(nam)
    #can.SaveAs("01fig.pdf")

#create_plot_pairs

#_____________________________________________________________________________
def get_config():

    #command line options
    args = sys.argv
    if len(args) < 2:
        print("No configuration specified.")
        quit()
    args.pop(0)

    return args.pop(0)

#get_config

#_____________________________________________________________________________
if __name__ == "__main__":

    gROOT.SetBatch()
    gStyle.SetPadTickX(1)
    gStyle.SetPadTickY(1)
    gStyle.SetFrameLineWidth(2)
    gStyle.SetPalette(1)
    gStyle.SetOptStat("")

    main()



