#! /usr/bin/python 
import sys
import xml.etree.ElementTree as ET
import numpy as np
import matplotlib.pyplot as plt
import matplotlib as mpl
import datetime
from numpy.linalg import norm
from scipy.interpolate import Rbf, InterpolatedUnivariateSpline
from scipy.ndimage.filters import convolve

if len(sys.argv) < 3:
    print "Usage:", sys.argv[0], "fichier.xml out.png [out.xml]"


tree = ET.parse(sys.argv[1])
frames = tree.getroot().findall("*/frame")

def getPoint(point):
    x = float(point.get("x"))
    y = float(point.get("y"))
    return (x,y)

def getPoints(frame):
    tl = getPoint(frame.find("./point[@name='tl']"))
    tr = getPoint(frame.find("./point[@name='tr']"))
    bl = getPoint(frame.find("./point[@name='bl']"))
    br = getPoint(frame.find("./point[@name='br']"))
    return [tl, tr, bl, br]

def getDist(frame):
    if frame.get("rejected") == "True":
        return [0, 0, 0, 0]


def export_xml(y, filename):
    f = open(filename, "w")
    f.write("<?xml version='1.0' encoding='utf-8'?>'\n")
    f.write('<seg_result version="0.2" generated="%s">\n' % datetime.datetime.now().isoformat())
    f.write('<software_used name="MILENA" version="2.0"/>\n')
    idx = filename.index("background")
    f.write('<source_sample_file>%s.avi</source_sample_file>\n' % filename[idx:])
    f.write(' <segmentation_results>\n')
    for i in xrange(len(y)):
        f.write('  <frame index="%i" rejected="false">\n' % (i+1))
        f.write('    <point name="tl" x="%i" y="%i" />\n' % (y[i,0,0], y[i,0,1]))
        f.write('    <point name="tr" x="%i" y="%i" />\n' % (y[i,1,0], y[i,1,1]))
        f.write('    <point name="bl" x="%i" y="%i" />\n' % (y[i,2,0], y[i,2,1]))
        f.write('    <point name="br" x="%i" y="%i" />\n' % (y[i,3,0], y[i,3,1]))
        f.write('  </frame>')
    f.write(' </segmentation_results>\n')
    f.write('</seg_result>\n')
    pass

points = [ getPoints(f) for f in frames ]
points = np.array(points)

x = np.arange(len(points))


a = norm((points[:,1] - points[:,0]), axis = 1)
b = norm((points[:,3] - points[:,1]), axis = 1)
c = norm((points[:,3] - points[:,0]), axis = 1)
d = norm((points[:,3] - points[:,2]), axis = 1)
e = norm((points[:,2] - points[:,0]), axis = 1)

s = (a+b+c)/2.0
a1 = np.sqrt(s*(s-a)*(s-b)*(s-c))
s = (c+d+e)/2.0
a2 = np.sqrt(s*(s-c)*(s-d)*(s-e))
area = a1 + a2
perim = a + b + d + e


# Detect bad points

# last = 2
# firsterr = True
# area[last] = area[:last+1].mean()
# for i in xrange(last+1, len(points)):
#     maxdist = np.abs(points[i] - points[last]).max()
#     if maxdist > (80 + 5 * (i-last)):
#     #maxdist = abs(area[i] - area[last]) / area[last]
#     #if maxdist > 0.20: #if 10% d'ecart
#         if firsterr:
#             print sys.argv[1]
#             firsterr = False
#         print "Frame #%i dismissed (%f)" % (i, maxdist)
#         mask[i] = False
#         # if (i - last > 5) and (np.std(area[last-5:last]) / area.mean()) < 0.05:
#         #     last = i
#         #     mask[last-5:last] = True
#         # else:
#         #     mask[i] = False
#     else:
#         last = i

# mask[-1] = True

# interpolate missing values
y = np.empty(points.shape)
mask = np.ones( (len(points)), dtype=bool)
firsterr = True
for k in xrange(4):
    mymask = np.ones( (len(points)), dtype=bool)
    last = 2
    for i in xrange(last+1, len(points)):
        maxdist = np.abs(points[i,k] - points[last,k]).max()
        if maxdist > (100 + 5 * (i-last)):
            if firsterr:
                print sys.argv[1]
                firsterr = False
            print "Frame #%i dismissed (%f)" % (i, maxdist)
            mymask[i] = False
        else:
            last = i

    f = InterpolatedUnivariateSpline(x[mymask], points[mymask,k,0], k=1)
    y[:,k,0] = f(x)
    f = InterpolatedUnivariateSpline(x[mymask], points[mymask,k,1], k=1)
    y[:,k,1] = f(x)
    mask = np.logical_and(mask,mymask)

avg = np.empty(points.shape)
for i in xrange(4):
    avg[:,i,0] = convolve(points[:,i,0], [1,1,1,1,1]) / 5.0
    avg[:,i,1] = convolve(points[:,i,1], [1,1,1,1,1]) / 5.0

plt.clf()

for i in range(4):
    plt.subplot(5,2,i*2+1)
    plt.plot(x, points[:,i,0], '+')
    plt.plot(x, y[:,i,0], 'r')
    plt.plot(x, avg[:,i,0] + 50, 'g')
    plt.plot(x, avg[:,i,0] - 50, 'b')
    plt.subplot(5,2,i*2+2)
    plt.plot(x, points[:,i,1], '+')
    plt.plot(x, y[:,i,1], 'r')
    plt.plot(x, avg[:,i,1] + 50, 'g')
    plt.plot(x, avg[:,i,1] - 50, 'b')

plt.subplot("529")
plt.plot(x, area)
plt.subplot(5,2,10)
plt.fill(x, np.logical_not(mask))


# plt.scatter(points[:,0,0], points[:,0,1], c = np.arange(len(points)) / float(len(points)),
#             cmap=mpl.cm.Blues)
# plt.scatter(points[:,1,0], points[:,1,1], c = np.arange(len(points)) / float(len(points)),
#             cmap=mpl.cm.Reds)
# plt.scatter(points[:,2,0], points[:,2,1], c = np.arange(len(points)) / float(len(points)),
#             cmap=mpl.cm.spring)
# plt.scatter(points[:,3,0], points[:,3,1], c = np.arange(len(points)) / float(len(points)),
#             cmap=mpl.cm.winter)
#plt.show()
plt.savefig(sys.argv[2], dpi=300)
if len(sys.argv) > 3:
    export_xml(y, sys.argv[3])
