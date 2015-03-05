#! /usr/bin/python
import sys
import xml.etree.ElementTree as ET
import numpy as np
import matplotlib.pyplot as plt
from numpy.linalg import norm

if len(sys.argv) != 2:
    print "Usage:", sys.argv[0], "fichier.xml"


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


points = [ getPoints(f) for f in frames ]
points = np.array(points)

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

plt.subplot(211)
plt.plot(np.arange(len(area)), area)
plt.title("Area")
plt.subplot(212)
plt.plot(np.arange(len(perim)), perim)
plt.title("Perimeter")
plt.show()

# Distance des points
d1 = norm(points[1:,0] - points[:-1,0], axis=1)
d2 = norm(points[1:,1] - points[:-1,1], axis=1)
d3 = norm(points[1:,2] - points[:-1,2], axis=1)
d4 = norm(points[1:,3] - points[:-1,3], axis=1)
plt.plot(np.arange(len(d1)), d1, label="tl")
plt.plot(np.arange(len(d1)), d2, label="tr")
plt.plot(np.arange(len(d1)), d3, label="bl")
plt.plot(np.arange(len(d1)), d4, label="br")
plt.legend()
plt.title("Corner distance")
plt.show()
