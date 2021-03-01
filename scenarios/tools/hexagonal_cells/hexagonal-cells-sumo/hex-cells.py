SUMO_NET_XML = 'sumo-scenarios/highway-tunnel/tunnel.net.xml'
POI_ENB_XML = 'sumo-scenarios/highway-tunnel/tunnel_poi.add.xml'
POLY_CELL_XML = 'sumo-scenarios/highway-tunnel/cells.xml'

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


# convertion from latitude and longitude to SUMO carthesian
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


# create vertices of hexagons
import math

def createHexagonalCell(position, diagonal, rotation):
    HEXAGON = 6
    x, y = position
    vertex_step = 360 / HEXAGON
    vertices = []

    for i in range(HEXAGON):
        rad = math.radians(rotation)
        rot_x = math.cos(rad)
        rot_y = math.sin(rad)
        vertices.append(
            (rot_x * diagonal + x,
            rot_y * diagonal + y)
        )
        rotation += vertex_step
    return vertices


enb_hex_cells = [createHexagonalCell(enb, CIRCUMRADIUS, ROTATION) for enb in enb_xy_positions]

print(enb_hex_cells)

# Write Sumo polygon XML file
import xml.etree.ElementTree as ET

def writeSumoPolygonFile(
        path, polygons, root_element_name, id_prefix, type_name, 
        color='red', 
        lineWidth='1' 
    ):
    root = ET.Element(root_element_name)
    
    for idx, polygon in enumerate(polygons):
        # go full circle 
        polygon.append(polygon[0])

        cell = ET.SubElement(root, 'poly')
        cell.set('id',id_prefix + str(idx))
        cell.set('type', type_name)
        cell.set('color',color)
        cell.set('lineWidth', str(lineWidth))
        sumo_2D_points =  [','.join(map(str, vertex)) for vertex in polygon]
        # sumo_2D_points =  [tupel2string(vertex) for vertex in polygon]
        cell.set('shape', ' '.join(sumo_2D_points))

    xml = ET.tostring(root)
    file = open(path, "wb")
    file.write(xml)

writeSumoPolygonFile(POLY_CELL_XML, enb_hex_cells, 'EnbCells', 'EnbCell_', 'EnbCell')
