SUMO_NET_XML = '../../InTAS/scenario/ingolstadt.net.xml'
POI_ENB_XML = '../../InTAS/scenario/InTAS_poi.add.xml'
POLY_CELL_XML = '../../InTAS/scenario/InTAS_cellsites.xml'

CIRCUMRADIUS = 200
ROTATION = 0


import os, sys
if 'SUMO_HOME' in os.environ:
    tools = os.path.join(os.environ['SUMO_HOME'], 'tools')
    sys.path.append(tools)
else:   
    sys.exit("please declare environment variable 'SUMO_HOME'")


import sumolib

# read in ENBs from POI additionals XML files
from xml.dom import minidom

def mDomAttrVal(element, attribute_name):
    return element.attributes[attribute_name].value


def loadEnbPositions(poi_path):
    dom = minidom.parse(poi_path)

    return [
        (mDomAttrVal(enb, 'lon'), mDomAttrVal(enb, 'lat'))
        for enb in dom.getElementsByTagName('poi') 
        if mDomAttrVal(enb, 'type') == 'eNodeB'
    ]


enb_geo_positions = loadEnbPositions(POI_ENB_XML)
print(enb_geo_positions)


# conversion from latitude and longitude to SUMO carthesian
net = sumolib.net.readNet(SUMO_NET_XML)

def geo2xy(lon, lat):
    return net.convertLonLat2XY(lon, lat)


def convertGeo2Xy(geo_positions):
    return [
        (geo2xy(pos[0], pos[1]))
        for pos in geo_positions
    ]


enb_xy_positions = convertGeo2Xy(enb_geo_positions)
print(enb_xy_positions)


# create vertices of regular shapes
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



def createCellsite(enb_xy_positions):
    enb_hex_cells = []
    center_points = []
    cell_borders = []
    for pos in enb_xy_positions:
        cellcenters = regularShape(3, pos, CIRCUMRADIUS / 2, ROTATION)
        cell_borders.append(regularShape(6, pos, overlap_radius, ROTATION + 30))
        center_points.append(cellcenters)
        for cell in cellcenters:
            enb_hex_cells.append(regularShape(6, cell, CIRCUMRADIUS / 2, ROTATION))
            # center_points.append(regularShape(6, cell, 10, 0))
            
        # hex_cells = [regularShape(6, cell, CIRCUMRADIUS / 4, ROTATION) for cell in cellcenters]
        # enb_hex_cells.append(hex_cells)
    return enb_hex_cells, center_points, cell_borders


enb_hex_cells, center_points, cell_borders = createCellsite(enb_xy_positions)

print(enb_hex_cells)

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
# 110,161,198
# 0,90,155
import xml.etree.ElementTree as ET

def writeSumoPolygonFile(
        path, polygons, centers, borders, root_element_name, id_prefix, type_name, 
        # color='0.43137254902,0.631372549019,0.776470588235',  
        color='#3498DB',  
        # color='red', 
        lineWidth='1' 
    ):
    root = ET.Element(root_element_name)
    last_idx = 0
    for idx, polygon in enumerate(polygons):
        # go full circle 
        polygon.append(polygon[0])

        cell = ET.SubElement(root, 'poly')
        cell.set('id',id_prefix + str(idx))
        cell.set('type', type_name)
        cell.set('color',color)
        # cell.set('fill', 'true')
        # cell.set('layer', '0')

        cell.set('lineWidth', str(lineWidth))
        sumo_2D_points =  [','.join(map(str, vertex)) for vertex in polygon]

        cell.set('shape', ' '.join(sumo_2D_points))
        last_idx = idx

    last_idx = last_idx + 1
    
    for site in centers:
        print('site')
        print(site)

        for cell in site:
            print(cell)
            cellel = ET.SubElement(root, 'poi')
            cellel.set('id',"Cell" + str(last_idx))
            cellel.set('type', "Cell")
            cellel.set('height', "10")
            cellel.set('width', "10")
            cellel.set('imgFile', '../../images/enodeb.jpg')
            cellel.set('x', str(cell[0]))
            cellel.set('y', str(cell[1]))

            setSumoGenericParameter(cellel, 'cellID', 'Cell_' + str(last_idx))
            setSumoGenericParameter(cellel, 'PCI', '202 (67/1)')
            setSumoGenericParameter(cellel, 'Maximum Signal (RSRP)',	'-84 dBm')
            setSumoGenericParameter(cellel, 'Uplink Freq',	'909.9 MHz')
            setSumoGenericParameter(cellel, 'Downlink Freq',	'954.9 MHz')
            setSumoGenericParameter(cellel, 'Freq Band',	'E-GSM (B8 FDD)')
            last_idx = last_idx + 1

    
    last_idx =last_idx + 1
    for idx, border in enumerate(borders):
        print("border")
        print(border)
        for start in range(0, len(border), 2):
            end = start + 2
            line = border[start:end]
            # go full circle 
            # line.append(line[0])

            cell = ET.SubElement(root, 'poly')
            cell.set('id','CellBorder' + str(last_idx))
            cell.set('type', type_name)
            cell.set('fill', 'true')
            cell.set('color', '#6ea1c6')

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
            cell.set('shape', ' '.join(sumo_2D_points))
            last_idx = last_idx + 1

    xml = ET.tostring(root)
    file = open(path, "wb")
    file.write(xml)



writeSumoPolygonFile(POLY_CELL_XML, enb_hex_cells, center_points, cell_borders, 'EnbCells', 'EnbCell_', 'EnbCell')
