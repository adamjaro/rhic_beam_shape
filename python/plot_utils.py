
import ROOT as rt
from ROOT import TIter, TText, TH1, TFrame, TGraph, TLegend, TH2

#_____________________________________________________________________________
def prepare_leg(xl, yl, dxl, dyl, tsiz=0.045):

    leg = TLegend(xl, yl, xl+dxl, yl+dyl)
    leg.SetFillStyle(0)
    leg.SetBorderSize(0)
    leg.SetTextSize(tsiz)

    return leg

#prepare_leg

#_____________________________________________________________________________
def put_yx_tit(hx, ytit, xtit, yofs=1.5, xofs=1.1):

    hx.SetYTitle(ytit)
    hx.SetXTitle(xtit)

    hx.SetTitleOffset(yofs, "Y")
    hx.SetTitleOffset(xofs, "X")

#put_yx_tit

#_____________________________________________________________________________
def put_frame_yx_tit(frame, ytit, xtit, yofs=1.7, xofs=1.2):

    frame.SetTitle("")

    frame.SetYTitle(ytit)
    frame.SetXTitle(xtit)

    frame.GetYaxis().SetTitleOffset(yofs);
    frame.GetXaxis().SetTitleOffset(xofs);

    frame.GetYaxis().CenterTitle()
    frame.GetXaxis().CenterTitle()

#put_frame_yx_tit

#_____________________________________________________________________________
def set_frame_text_size(frame, siz):

    frame.SetTitleSize(siz)
    frame.SetLabelSize(siz)
    frame.SetTitleSize(siz, "Y")
    frame.SetLabelSize(siz, "Y")
    frame.SetTitleSize(siz, "Z")
    frame.SetLabelSize(siz, "Z")

#set_frame_text_size

#_____________________________________________________________________________
def set_margin_lbtr(gPad, lm, bm, tm, rm):

    gPad.SetLeftMargin(lm)
    gPad.SetBottomMargin(bm)
    gPad.SetTopMargin(tm)
    gPad.SetRightMargin(rm)

#set_margin_lbtr

#_____________________________________________________________________________
def invert_col(pad):

    bgcol=rt.kBlack
    fgcol = rt.kOrange-3

    pad.SetFillColor(bgcol)
    pad.SetFrameLineColor(fgcol)

    next = TIter( pad.GetListOfPrimitives() )
    obj = next()
    while obj != None:

        #TText
        if obj.InheritsFrom( TText.Class() ) == True:
            if obj.GetTextColor() == rt.kBlack:
                obj.SetTextColor( fgcol )

        #H1
        if obj.InheritsFrom( TH1.Class() ) == True:
            if obj.GetLineColor() == rt.kBlack:
                obj.SetLineColor(fgcol)
                obj.SetFillColor(bgcol)
            if obj.GetMarkerColor() == rt.kBlack: obj.SetMarkerColor(fgcol)
            obj.SetAxisColor(fgcol, "X")
            obj.SetAxisColor(fgcol, "Y")
            obj.SetLabelColor(fgcol, "X")
            obj.SetLabelColor(fgcol, "Y")
            obj.GetXaxis().SetTitleColor(fgcol)
            obj.GetYaxis().SetTitleColor(fgcol)

        #TFrame
        if obj.InheritsFrom( TFrame.Class() ) == True:
            if obj.GetLineColor() == rt.kBlack:
                obj.SetLineColor(fgcol)
                obj.SetFillColor(bgcol)

        #TGraph
        if obj.InheritsFrom( TGraph.Class() ) == True:
            obj.SetFillColor(bgcol)
            ax = obj.GetXaxis()
            ay = obj.GetYaxis()
            ax.SetAxisColor(fgcol)
            ay.SetAxisColor(fgcol)
            ax.SetLabelColor(fgcol)
            ay.SetLabelColor(fgcol)
            ax.SetTitleColor(fgcol)
            ay.SetTitleColor(fgcol)
            if obj.GetLineColor() == rt.kBlack:
                obj.SetLineColor(fgcol)
                #obj.SetMarkerColor(fgcol)

        #Legend
        if obj.InheritsFrom(TLegend.Class()) == True:
            obj.SetTextColor(fgcol)

        #H2
        if obj.InheritsFrom(TH2.Class()) == True:
            obj.SetAxisColor(fgcol, "Z")
            obj.SetLabelColor(fgcol, "Z")
            obj.GetZaxis().SetTitleColor(fgcol)

        #print obj.GetName(), obj.ClassName()

        #move to the next item
        obj = next()

#invert_col

#_____________________________________________________________________________
def get_obj(pad, nam):

    next = TIter(pad.GetListOfPrimitives())

    obj = next()
    while obj != None:

        if obj.GetName() == nam:
            return obj

        obj = next()

#get_obj

















