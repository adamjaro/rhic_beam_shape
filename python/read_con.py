
#read parameters from config parser

import configparser
from ctypes import c_double

#_____________________________________________________________________________
class read_con:
    #_____________________________________________________________________________
    def __init__(self, config):

        self.con = configparser.RawConfigParser(inline_comment_prefixes=(";","#"))
        self.con.read(config)

    #_____________________________________________________________________________
    def __call__(self, par):

        return self.con.getfloat("main", par)

    #_____________________________________________________________________________
    def cdbl(self, par):

        return c_double( self.con.getfloat("main", par) )

    #_____________________________________________________________________________
    def int(self, par, sec="main"):

        return self.con.getint(sec, par)

    #_____________________________________________________________________________
    def flt(self, par, sec="main"):

        return self.con.getfloat(sec, par)

    #_____________________________________________________________________________
    def str(self, par, sec="main"):

        return self.con.get(sec, par).strip("\"'")

    #_____________________________________________________________________________
    def has_option(self, par):

        return self.con.has_option("main", par)


