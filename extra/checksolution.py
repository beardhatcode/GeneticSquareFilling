'''
Python scripts that plots solution
INPUT is two arguments : filename of polygon information and solution
for example: python3 checksolution.py vierkant.txt solution.txt
'''


import sys
import matplotlib.pyplot as plt
from matplotlib.path import Path
import matplotlib.patches as patches

polygonfile = sys.argv[1]
solutionfile = sys.argv[2]

'''
GET POLYGON INFO
'''

polygon = open(polygonfile, 'r')
lines = polygon.readlines()
ngon = int(lines[0].rstrip('\n'));
coordinates = []

for i in range(1,1+ngon):
	co = tuple(lines[i].rstrip('\n').split(' '))
	co = tuple(map(lambda x: float(x), co));
	coordinates.append(co)
coordinates.append(coordinates[0])


'''
GET SOLUTION INFO
'''
solution = open(solutionfile, 'r')
lines = solution.readlines()
xco = []
yco = []
for i in range(1,len(lines)):
	co = tuple(lines[i].rstrip('\n').split(' '))
	co = tuple(map(lambda x: float(x), co))
	xco.append(co[0])
	yco.append(co[1])

#Find minimum and maximum coordinates
minx = min(map(lambda co : co[0], coordinates));
miny = min(map(lambda co : co[1], coordinates));
maxx = max(map(lambda co : co[0], coordinates));
maxy = max(map(lambda co : co[1], coordinates));


'''
PLOT POINTS AND POLYGON
'''

directions = [Path.MOVETO]
for _ in range(ngon-1):
	directions.append(Path.LINETO)
directions.append(Path.CLOSEPOLY)


path = Path(coordinates, directions)
fig = plt.figure()
ax = fig.add_subplot(111)
patch = patches.PathPatch(path, facecolor='white', lw=2)
ax.add_patch(patch)
ax.plot(xco, yco, 'or')
ax.set_xlim(minx-1, maxx+1)
ax.set_ylim(miny-1, maxy+1)
plt.show()
