CSV_CELLSITES = '../../InTAS/scenario/cellsites/cellsites.csv'
SUMO_NET_XML = '../../InTAS/scenario/ingolstadt.net.xml'
XML_OUT = '../../InTAS/scenario/cellsites/cellsites.xml'
OMNET_OUT = '../../InTAS/scenario/cellsites/cellsites_omnet.txt'

CIRCUMRADIUS = 200
ROTATION = 0

MY_PROVIDER = "Telekom"

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

PROVIDER = 'Provider'
ORIENTATION = 'Orientation'
ACTIVE = "Active"

CELL_VERTICES = 'cell_vertices'
CELL_CENTERS = 'cell_centers'
CELL_BORDERS = 'cell_borders'



enb_fields = [ENB_ID, 'District', LATITUDE, LONGITUDE, PROVIDER, 'Bands', ORIENTATION, ACTIVE]

cell_fields = ['Cell Number', 'Cell Id', 'PCI', 'RSRP [dBm]', 'Uplink Freq [MHz]', 'Downlink Freq [MHz]', 'Freq band']

def unify_enb(flatfile):
    enbs = {}
    for entry in flatfile:
        if entry[ACTIVE] == 'yes':
            enb_id = entry[ENB_ID]
            if not enb_id in enbs:
                enbs[enb_id] = {field:entry[field] for field in enb_fields}
                enbs[enb_id][CELLS] = []
            enbs[enb_id][CELLS].append({field:entry[field] for field in cell_fields})
    return enbs

enbs = unify_enb(flatfile)


import sumolib

# conversion from latitude and longitude to SUMO carthesian
net = sumolib.net.readNet(SUMO_NET_XML)

def geo2xy(lon, lat):
    return net.convertLonLat2XY(lon, lat)

def coordinatesInBoundary(x, y):
    xmin,_,_,ymax = net.getBoundary()
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

    # make 0° point to North
    rotation = rotation 

    for i in range(num_vertices):
        rad = math.radians(rotation)
        rot_x = math.sin(rad)
        rot_y = math.cos(rad)
        vertices.append(
            (rot_x * circumradius + x,
            rot_y * circumradius + y)
        )
        rotation += vertex_step
    return vertices

overlap_radius = ((CIRCUMRADIUS / 2) * math.sin(math.radians((120))) / math.sin(math.radians(30)))

def createCellsite(enbs):
    for enb in enbs.values():
        pos = (enb[X_COORD], enb[Y_COORD])
        rotation = float(enb[ORIENTATION])
        enb[CELL_CENTERS] = regularShape(3, pos, CIRCUMRADIUS / 2, rotation)
        enb[CELL_BORDERS] = regularShape(24, pos, overlap_radius, rotation + 30)
        enb[CELL_VERTICES] = [regularShape(6, cell, CIRCUMRADIUS / 2, rotation) for cell in enb[CELL_CENTERS]]

createCellsite(enbs)





def get_vector(startpoint, endpoint):
    return substract_vectors(endpoint, startpoint)  

def get_magnitude(vec):
    return math.sqrt(sum(vec[i]*vec[i] for i in range(len(vec))))

def add_vectors(u, v):
    return [ u[i]+v[i] for i in range(len(u)) ]

def substract_vectors(u, v):
    return [ u[i]-v[i] for i in range(len(u)) ]

def multiply_scalar(vec, s):
    return [v * s for v in vec]

def dot_product_vectors(u, v):
    return sum(u[i]*v[i] for i in range(len(u)))

def get_normalized(vec):
    magnitude = get_magnitude(vec)
    return [ vec[i]/magnitude  for i in range(len(vec)) ]

def get_orthogonal(vec):
    return (-vec[1], vec[0])

def get_orthonormal(startpoint, endpoint):
    return get_orthogonal(get_normalized(get_vector(startpoint, endpoint)))


def set_sumo_generic_parameter(parent, key, val):
    param = ET.SubElement(parent, 'param')
    param.set('key',key)
    param.set('value',val)


def get_covered_lane_ids(net, cellcenter, coverage_radius):
    lanes = net.getNeighboringLanes(cellcenter[0], cellcenter[1], coverage_radius)
    lane_ids = tuple(lane[0].getID() for lane in lanes)
    return lane_ids

# Write Sumo polygon XML file #005a9b #3498DB #6ea1c6
import xml.etree.ElementTree as ET

def write_sumo_xml(
        path,
        enbs,
        root_element_name='additional',
        # color='#3498DB', 
        color = 'blue',
        # color='#FF4500',  
        lineWidth='1.5' 
    ):
    root = ET.Element(root_element_name)
    cell_idx = 0

    for enb_idx, enb in enumerate(enbs.values()):
        if enb[PROVIDER] == MY_PROVIDER:
            cell_color = color
        else:
            cell_color = 'grey'
        enb_info = ET.SubElement(root, 'poi')
        enb_info.set('id',"eNB_" + str(cell_idx))
        enb_info.set('type', "eNB")
        enb_info.set('height', "10")
        enb_info.set('width', "10")
        enb_info.set('imgFile', '../../../images/enodeb.jpg')
        enb_info.set('color', 'white')
        enb_info.set('x', str(enb[X_COORD]))
        enb_info.set('y', str(enb[Y_COORD]))

        [set_sumo_generic_parameter(enb_info, enbparam, enb[enbparam]) for enbparam in enb_fields]


        for i in range(len(enb[CELL_CENTERS])):
            polygon = enb[CELL_VERTICES][i]
            # go full circle 
            polygon.append(polygon[0])

            cell_poly = ET.SubElement(root, 'poly')
            cell_poly.set('id','Cell_Hexagon_' + str(cell_idx))
            cell_poly.set('type', 'Cell_Hexagon')
            cell_poly.set('color', cell_color)
            cell_poly.set('lineWidth', str(lineWidth))
            sumo_2D_points =  [','.join(map(str, vertex)) for vertex in polygon]
            cell_poly.set('shape', ' '.join(sumo_2D_points))
            set_sumo_generic_parameter(
                cell_poly, 'laneIdList', 
                ','.join(get_covered_lane_ids(net, enb[CELL_CENTERS][i], CIRCUMRADIUS/2))
            )
            
                        
            cell_info = ET.SubElement(root, 'poi')
            cell_info.set('id',"Cell_" + str(cell_idx))
            cell_info.set('type', "Cell")
            cell_info.set('height', "10")
            cell_info.set('width', "10")
            cell_info.set('imgFile', '../../../images/enodeb.jpg')
            cell_info.set('color', cell_color)
            cell_info.set('x', str(enb[CELL_CENTERS][i][0]))
            cell_info.set('y', str(enb[CELL_CENTERS][i][1]))

            [set_sumo_generic_parameter(cell_info, cellparam, enb[CELLS][i][cellparam]) for cellparam in cell_fields]

            cell_idx = cell_idx + 1

        
        
        # for idx, border in enumerate(borders):
        border = enb[CELL_BORDERS]
        border_idx = 0
        num_vertices = int(len(border) / 6)
        skip = False
        for start in range(0, len(border), num_vertices):
            if not skip:
                end = start + num_vertices + 1
                lines = border[start:end]
                
                # go full circle 
                # line.append(line[0])

                border_shape = ET.SubElement(root, 'poly')
                border_shape.set('id','CellsiteBorder_' + str(enb_idx) + str(border_idx))
                border_shape.set('type', 'CellsiteBorder')
                border_shape.set('fill', 'true')
                if enb[PROVIDER] == MY_PROVIDER:
                    # border_shape.set('color', '#3DAEE9')
                    border_shape.set('color', '#FFA500')
                else:
                    border_shape.set('color', '#D3D3D3')

                line_shape = lines

                for idx in range(len(lines)-1, -1, -1):
                    if idx != 0:
                        orthonormal = get_orthonormal(lines[idx],lines[idx - 1])
                    else:
                        orthonormal = get_orthonormal(lines[idx + 1], lines[idx])
                    line_shape.append(add_vectors(lines[idx],multiply_scalar(orthonormal, 2.75)))
                    
                
                line_shape.append(line_shape[0])

                sumo_2D_points =  [','.join(map(str, vertex)) for vertex in line_shape]
                border_shape.set('shape', ' '.join(sumo_2D_points))
                border_idx = border_idx + 1
                skip = True
            else:
                skip = False

        xml = ET.tostring(root)
        file = open(path, "wb")
        file.write(xml)



write_sumo_xml(XML_OUT,enbs)




def split_external_enbs(enbs):
    my_enbs = list()
    external_enbs = list()
    for enb in enbs.values():
        if enb[PROVIDER] == MY_PROVIDER:
            my_enbs.append(enb)
        else:
            external_enbs.append(enb)
    return my_enbs, external_enbs
 

def write_omnet(enbs):
    my_enbs, external_enbs = split_external_enbs(enbs)
    with open(OMNET_OUT, "w") as file:
        file.write('# active simulated LTE Basestations\n*.numLteBaseStations = {}'.format(len(my_enbs)))
        for idx, enb in enumerate(my_enbs):
            file.write("\n*.eNodeB[{}].mobility.initialX = {}m".format(idx, enb[X_BOUNDARY]))
            file.write("\n*.eNodeB[{}].mobility.initialY = {}m".format(idx, enb[Y_BOUNDARY]))
        
        file.write('\n')
        file.write('\n# external enbs for interference simulation\n*.numExtCells = {}'.format(len(external_enbs)))
        file.write('\n*.extCell[*].txPower = 20')
        file.write('\n*.extCell[*].txDirection = "ISOTROPIC"')
        file.write('\n*.extCell[*].bandAllocation = "RANDOM_ALLOC"')
        file.write('\n*.extCell[*].bandUtilization = 0.5')
        for idx, enb in enumerate(external_enbs):
            file.write("\n*.extCell[{}].mobility.initialX = {}m".format(idx, enb[X_BOUNDARY]))
            file.write("\n*.extCell[{}].mobility.initialY = {}m".format(idx, enb[Y_BOUNDARY]))
        

write_omnet(enbs)
