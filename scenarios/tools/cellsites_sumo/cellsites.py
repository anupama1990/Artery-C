CSV_CELLSITES = '../../InTAS/scenario/cellsites/cellsites.csv'
SUMO_NET_XML = '../../InTAS/scenario/ingolstadt.net.xml'
XML_OUT = '../../InTAS/scenario/cellsites/cellsites.xml'
OMNET_OUT = '../../InTAS/scenario/cellsites/cellsites_omnet.txt'

CIRCUMRADIUS = 200
ROTATION = 0

import os, sys
if 'SUMO_HOME' in os.environ:
    tools = os.path.join(os.environ['SUMO_HOME'], 'tools')
    sys.path.append(tools)
else:   
    sys.exit("please declare environment variable 'SUMO_HOME'")

import csv

def read_csv(file):
    with open(file, mode='r') as csv_file:
        csv_reader = csv.DictReader(csv_file)
        return list(csv_reader)


flatfile = read_csv(CSV_CELLSITES)

ENB_ID = 'Enb Id'
CELLS = 'cells'

X_COORD = 'x'
Y_COORD = 'y'

X_BOUNDARY = 'x_boundary'
Y_BOUNDARY = 'y_boundary'

LATITUDE = 'Latitude'
LONGITUDE = 'Longitude'


CELL_VERTICES = 'cell_vertices'
CELL_CENTERS = 'cell_centers'
CELL_BORDERS = 'cell_borders'

enb_fields = [ENB_ID, 'District', LATITUDE, LONGITUDE, 'Provider', 'Bands']

cell_fields = ['Cell Number', 'Cell Id', 'PCI', 'RSRP [dBm]', 'Uplink Freq [MHz]', 'Downlink Freq [MHz]', 'Freq band', 'Direction']

def unify_enb(flatfile):
    enbs = {}
    for entry in flatfile:
        enb_id = entry[ENB_ID]
        if not enb_id in enbs:
            enbs[enb_id] = {field:entry[field] for field in enb_fields}
            enbs[enb_id][CELLS] = []
        enbs[enb_id][CELLS].append({field:entry[field] for field in cell_fields})
    return enbs

enbs = unify_enb(flatfile)
print(enbs)


import sumolib

# conversion from latitude and longitude to SUMO carthesian
net = sumolib.net.readNet(SUMO_NET_XML)

def geo2xy(lon, lat):
    return net.convertLonLat2XY(lon, lat)

def coordinatesInBoundary(x, y):
    xmin,_,_,ymax = net.getBoundary()
    print(xmin)
    print(ymax)
    return x - xmin, ymax - y

def convertCoordinates(enbs):
    for k in enbs.keys():
        enb = enbs[k]
        enb[X_COORD], enb[Y_COORD] = geo2xy(float(enb[LONGITUDE]), float(enb[LATITUDE]))
        enb[X_BOUNDARY], enb[Y_BOUNDARY] = coordinatesInBoundary(enb[X_COORD], enb[Y_COORD])

convertCoordinates(enbs)

import math

def regularShape(num_vertices, position, circumradius, rotation):
    x, y = position
    vertex_step = 360 / num_vertices
    vertices = []

    for i in range(num_vertices):
        rad = math.radians(rotation)
        rot_x = math.cos(rad)
        rot_y = math.sin(rad)
        vertices.append(
            (rot_x * circumradius + x,
            rot_y * circumradius + y)
        )
        rotation += vertex_step
    return vertices

overlap_radius = ((CIRCUMRADIUS / 2) * math.sin(math.radians((120))) / math.sin(math.radians(30)))

def createCellsite(enbs):
    print(enbs)
    for enb in enbs.values():
        print('enb')
        print(enb)
        pos = (enb[X_COORD], enb[Y_COORD])
        enb[CELL_CENTERS] = regularShape(3, pos, CIRCUMRADIUS / 2, ROTATION)
        enb[CELL_BORDERS] = regularShape(6, pos, overlap_radius, ROTATION + 30)
        enb[CELL_VERTICES] = [regularShape(6, cell, CIRCUMRADIUS / 2, ROTATION) for cell in enb[CELL_CENTERS]]


createCellsite(enbs)


def getLineVector(startpoint, endpoint):
    return substractVectors(endpoint, startpoint)  

def getMagnitude(vec):
    return math.sqrt(sum(vec[i]*vec[i] for i in range(len(vec))))

def addVectors(u, v):
    return [ u[i]+v[i] for i in range(len(u)) ]

def substractVectors(u, v):
    return [ u[i]-v[i] for i in range(len(u)) ]

def dotProductVectors(u, v):
    return sum(u[i]*v[i] for i in range(len(u)))

def getNormalized(vec):
    magnitude = getMagnitude(vec)
    return [ vec[i]/magnitude  for i in range(len(vec)) ]

def getOrthogonal(vec):
    return (-vec[1], vec[0])


def setSumoGenericParameter(parent, key, val):
    param = ET.SubElement(parent, 'param')
    param.set('key',key)
    param.set('value',val)


# Write Sumo polygon XML file #005a9b #3498DB #6ea1c6
import xml.etree.ElementTree as ET

def writeSumoPolygonFile(
        path,
        enbs,
        root_element_name='additional',
        color='#3498DB',  
        lineWidth='1' 
    ):
    root = ET.Element(root_element_name)
    cell_idx = 0
    for enb_idx, enb in enumerate(enbs.values()):
        enb_info = ET.SubElement(root, 'poi')
        enb_info.set('id',"Enb_" + str(cell_idx))
        enb_info.set('type', "Enb")
        enb_info.set('height', "10")
        enb_info.set('width', "10")
        enb_info.set('imgFile', '../../../images/enodeb.jpg')
        enb_info.set('x', str(enb[X_COORD]))
        enb_info.set('y', str(enb[Y_COORD]))

        [setSumoGenericParameter(enb_info, enbparam, enb[enbparam]) for enbparam in enb_fields]


        for i in range(len(enb[CELL_CENTERS])):
            polygon = enb[CELL_VERTICES][i]
            # go full circle 
            polygon.append(polygon[0])

            cell_shape = ET.SubElement(root, 'poly')
            cell_shape.set('id','Cell_Outline' + str(cell_idx))
            cell_shape.set('type', 'Cell_Outline')
            cell_shape.set('color',color)
            cell_shape.set('lineWidth', str(lineWidth))
            sumo_2D_points =  [','.join(map(str, vertex)) for vertex in polygon]

            cell_shape.set('shape', ' '.join(sumo_2D_points))
                        
            cell_info = ET.SubElement(root, 'poi')
            cell_info.set('id',"Cell_" + str(cell_idx))
            cell_info.set('type', "Cell")
            cell_info.set('height', "10")
            cell_info.set('width', "10")
            cell_info.set('imgFile', '../../../images/enodeb.jpg')
            cell_info.set('x', str(enb[CELL_CENTERS][i][0]))
            cell_info.set('y', str(enb[CELL_CENTERS][i][1]))

            [setSumoGenericParameter(cell_info, cellparam, enb[CELLS][i][cellparam]) for cellparam in cell_fields]

            cell_idx = cell_idx + 1

        
        
        # for idx, border in enumerate(borders):
        border = enb[CELL_BORDERS]
        print("border")
        print(border)
        border_idx = 0
        for start in range(0, len(border), 2):
            end = start + 2
            line = border[start:end]
            # go full circle 
            # line.append(line[0])

            border_shape = ET.SubElement(root, 'poly')
            border_shape.set('id','CellsiteBorder_' + str(enb_idx) + str(border_idx))
            border_shape.set('type', 'CellsiteBorder')
            border_shape.set('fill', 'true')
            border_shape.set('color', '#6ea1c6')

            print(line)
            print("line" + str(start))

            box = []
            orthonormal = getOrthogonal(getNormalized(getLineVector(line[0],line[1])))
            box.append(line[0])
            box.append(addVectors(line[0], orthonormal))
            box.append(addVectors(line[1], orthonormal))
            box.append(line[1])
            box.append(line[0])
            sumo_2D_points =  [','.join(map(str, vertex)) for vertex in box]
            border_shape.set('shape', ' '.join(sumo_2D_points))
            border_idx = border_idx + 1

        xml = ET.tostring(root)
        file = open(path, "wb")
        file.write(xml)



writeSumoPolygonFile(XML_OUT,enbs)



def writeOmnet(enbs):
    with open(OMNET_OUT, "w") as file:
        for idx, enb in enumerate(enbs.values()):
            file.write("*.eNodeB[{}].mobility.initialX = {}m\n".format(idx, enb[X_BOUNDARY]))
            file.write("*.eNodeB[{}].mobility.initialY = {}m\n".format(idx, enb[Y_BOUNDARY]))


writeOmnet(enbs)