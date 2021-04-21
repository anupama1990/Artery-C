CSV_CELLSITES   = '../../InTAS/scenario/cellsites/cellsites.csv'
SUMO_NET_XML    = '../../InTAS/scenario/ingolstadt.net.xml'
IN_ROUTE_DIR    = '../../InTAS/scenario/routes/'
OUT_ROUTE_DIR   = '../../InTAS/scenario/cellsites/routes/'

COVERAGE_RADIUS =  1000


import os, sys
if 'SUMO_HOME' in os.environ:
    tools = os.path.join(os.environ['SUMO_HOME'], 'tools')
    sys.path.append(tools)
else:   
    sys.exit("please declare environment variable 'SUMO_HOME'")

import sumolib

# read cellsites csv
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

convertCoordinates(enbs)


covered_edges = set()

for enb in enbs.values():
    edges = net.getNeighboringEdges(enb[X_COORD], enb[Y_COORD], COVERAGE_RADIUS)
    covered_edges = covered_edges.union({edge[0].getID() for edge in edges})

valid_routes = []

import os

FILE_SUFFIX = '.rou.xml'

for filename in sorted(os.listdir(IN_ROUTE_DIR)):
    if filename.endswith(FILE_SUFFIX): 
        path = os.path.join(IN_ROUTE_DIR, filename)
        routes_root = [routes for routes in sumolib.xml.parse(path, 'routes')][0]
        print(len(routes_root.vehicle))
        
        routes_root.vehicle = [v for v in routes_root.vehicle if v.routeDistribution[0].route[0].edges.split()[0] in covered_edges]
        
        print(len(routes_root.vehicle))
        with open(os.path.join(OUT_ROUTE_DIR, filename), 'w') as file:
            file.write(routes_root.toXML())