#! /usr/bin/python
import sys
import xml.etree.ElementTree as ET

if len(sys.argv) != 3:
    print "Usage:", sys.argv[0], "fichier1.xml fichier2.xml"

tol = 4
tree1 = ET.parse(sys.argv[1])
tree2 = ET.parse(sys.argv[2])

frames1 = tree1.getroot().findall("*/frame")
frames2 = tree2.getroot().findall("*/frame")

def getPoint(point):
    x = float(point.get("x"))
    y = float(point.get("y"))
    return (x,y)

def getPoints(frame):
    tl = getPoint(frame.find("./point[@name='tl']"))
    tr = getPoint(frame.find("./point[@name='tr']"))
    bl = getPoint(frame.find("./point[@name='bl']"))
    br = getPoint(frame.find("./point[@name='br']"))
    return (tl, tr, bl, br)

def checkCoord(f1, f2, idx):
    pts1 = getPoints(f1)
    pts2 = getPoints(f2)

    s = 0
    for p1, p2 in zip(pts1, pts2):
        s += ((p1[0] - p2[0])**2 + (p1[1] - p2[1])**2) ** .5
    print "Frame", idx, "error:", s

for f1, f2 in zip(frames1, frames2):
    assert f1.get("index") == f2.get("index")
    idx = f1.get("index")
    if f1.get("rejected") != f2.get("rejected"):
        print "Frame", idx, ": rejection error"
    else:
        checkCoord(f1, f2, idx)


