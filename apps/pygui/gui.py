#! /usr/bin/env python


# On lance l'interface tkinter pour permettre le choix des plots.
import matplotlib
matplotlib.use('TkAgg')
import matplotlib.pyplot as plt

import Tkinter as tk
import ttk
import pandas as pd
import argparse

import tosGUI
import numpy as np
from skimage import io
from scipy.misc import imresize

import IPython


class Main(tk.Frame):
    def __init__(self, parent, df, gui):
        tk.Frame.__init__(self, parent)
        self.gui = gui
        self.df = df
        self.initUI(df)

    def initUI(self, df):
        vals = [ str(x) for x in df.columns ]

        tk.Label(self, text = "Plot attribute").grid(row=0, column=0)
        self.attribute_list = ttk.Combobox(self, values = vals)
        self.attribute_list.grid(row=0, column=1)
        self.abutton = tk.Button(self, text = "Add")
        self.abutton["command"] = self.addAttribute
        self.abutton.grid(row=0, column=2)

        tk.Label(self, text = "Plot spectrum").grid(row=1, column=0)
        self.specx = ttk.Combobox(self, values = vals)
        self.specx.grid(row=1, column=1)
        self.specy = ttk.Combobox(self, values = vals)
        self.specy.grid(row=1, column=2)
        self.specz = ttk.Combobox(self, values = vals)
        self.specz.grid(row=1, column=3)
        self.bbutton = tk.Button(self, text = "Add")
        self.bbutton["command"] = self.addSpectrum
        self.bbutton.grid(row=1, column=4)

        self.pack()

    def addAttribute(self):
        a = self.attribute_list.get()
        if a:
            if df[a].dtype == "O":
                self.gui.plotAttributeText(self.df[a])
            else:
                self.gui.plotAttribute(self.df[a])

    def addSpectrum(self):
        a,b,c = self.specx.get(), self.specy.get(), self.specz.get()
        if a and b and c:
            self.gui.plotSpectra(self.df, a, b, c)

def addborder(ima):
    v = ima[0,:].sum(0) + ima[-1,:].sum(0) + ima[:,0].sum(0) + ima[:,-1].sum(0)
    v = v / (2.0 * (ima.shape[0] + ima.shape[1]))
    shp = ima.shape
    shp = (shp[0]+2, shp[1]+2, 1) if ima.ndim == 2 else (shp[0]+2, shp[1]+2, shp[2])

    v = np.array(v, ndmin=1)
    new = np.empty(shp, dtype = ima.dtype)
    new[0,:,:] = np.tile(v[np.newaxis,...], (shp[1],1))
    new[-1,:,:] = np.tile(v[np.newaxis,...], (shp[1],1))
    new[:,0,:] = np.tile(v[np.newaxis,...], (shp[0],1))
    new[:,-1,:] = np.tile(v[np.newaxis,...], (shp[0],1))
    new[1:-1,1:-1,:] = ima[:,:,np.newaxis] if ima.ndim == 2 else ima

    if new.shape[2] == 1:
        new = new.reshape( new.shape[:2] )
    return new

def adjustTo(ori, ref):
    shp = np.array(ref.shape)
    shp0 = np.array(ori.shape)
    print shp, shp0
    if np.array_equal(shp0, shp):
        return ori
    ori = addborder(ori)
    return imresize(ori, shp)



parser = argparse.ArgumentParser(description='Run the gui')
parser.add_argument("tree_path")
parser.add_argument("-i", dest="images", action="append", help="Images to show")
parser.add_argument("-f", dest="attrfile", required=True, help="A CSV file containing the attributes. NxM matrices with M the number of attributes.")
parser.add_argument("-a", dest="attributes", nargs="*", help="Attributes to plot")
parser.add_argument("-s", dest="spectrum", nargs=3, action="append", help="a triple ATTR ATTR ATTR")


args = parser.parse_args()
df = pd.read_csv(args.attrfile)

root = tk.Tk()
plt.ion()
mygui = tosGUI.tosGUI(args.tree_path, df)

# Add Image
for path in args.images:
    image = io.imread(path, plugin="freeimage")
    image = adjustTo(image, mygui.nodemap)
    mygui.plotImage(image)

plt.show()

app = Main(root, df, mygui)

# Attributes
if args.attributes:
    for col in args.attributes:
        mygui.plotAttribute(df[col])

# Spectrum
if args.spectrum:
    for s in args.spectrum:
        mygui.plotSpectra(df, *s)

mygui.plotStat()
root.mainloop()

# Lance l'interpreteur
IPython.embed()



#IPython.Shell.IPShell().mainloop(sys_exit=1)



'''
mygui = tosGUI.tosGUI("../attributes/tmp.tree")
image = io.imread("../g2/maxdepth3.tiff", plugin="freeimage")
mygui.plotImage(image)

df = pd.read_csv("../attributes/pattern.csv")
mygui.plotAttribute(df["Area"])
mygui.plotSpectra(df, "Area", "Energy", "Area")
'''
