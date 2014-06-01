#Creates a set of 4-sided cones in a 3D array

triangleHeight = 1.0
triangleWidth = 1.0

triangleFaceFormat = "3 {0} {1} {2}\n"

numTriX = 10
numTriY = 10
numTriZ = 10
startingZ = 4.0
triSpacing = 2.0

class vertex:
    x = 0.0
    y = 0.0
    z = 0.0
    def getVertexString(self):
        return "{0} {1} {2}\n".format(self.x, self.y, self.z)

class triangle:
    def __init__(self, topx, topy, topz):
        self.top = vertex()
        self.top.x = topx
        self.top.y = topy
        self.top.z = topz
    def getVertices(self):
        vertices = [self.top]
        v1,v2,v3,v4 = vertex(), vertex(), vertex(), vertex()
        v1.y = self.top.y - triangleHeight
        v2.y = self.top.y - triangleHeight
        v3.y = self.top.y - triangleHeight
        v4.y = self.top.y - triangleHeight
        v1.x = self.top.x - triangleWidth / 2
        v2.x = self.top.x - triangleWidth / 2
        v3.x = self.top.x + triangleWidth / 2
        v4.x = self.top.x + triangleWidth / 2
        v1.z = self.top.z - triangleWidth / 2
        v2.z = self.top.z + triangleWidth / 2
        v3.z = self.top.z + triangleWidth / 2
        v4.z = self.top.z - triangleWidth / 2
        vertices.append(v1)
        vertices.append(v2)
        vertices.append(v3)
        vertices.append(v4)
        return vertices
    def getVertexIndiciesList(self, triangleProperties, triangleCount):
        startingIndex = triangleCount * 5
        faces = triangleProperties + triangleFaceFormat.format(startingIndex, startingIndex + 1, startingIndex + 2)
        faces += triangleProperties + triangleFaceFormat.format(startingIndex, startingIndex + 2, startingIndex + 3)
        faces += triangleProperties + triangleFaceFormat.format(startingIndex, startingIndex + 3, startingIndex + 4)
        faces += triangleProperties + triangleFaceFormat.format(startingIndex, startingIndex + 1, startingIndex + 4)
        faces += triangleProperties + triangleFaceFormat.format(startingIndex + 1, startingIndex + 2, startingIndex + 3)
        faces += triangleProperties + triangleFaceFormat.format(startingIndex + 1, startingIndex + 3, startingIndex + 4)
        return faces

def PrintHeader(outputFile, triangleList):
    outputFile.write("ply\n")
    outputFile.write("format ascii 1.0\n")
    #each triangle cone has 5 vertices
    outputFile.write("element vertex {0}\n".format(len(triangleList) * 5))
    outputFile.write("property float32 x\n")
    outputFile.write("property float32 y\n")
    outputFile.write("property float32 z\n")
    #each triangle cone has 6 faces 
    outputFile.write("element triangle {0}\n".format(len(triangleList) * 6))
    outputFile.write("property float32 colorR\n")
    outputFile.write("property float32 colorG\n")
    outputFile.write("property float32 colorB\n")
    outputFile.write("property float32 gloss\n")
    outputFile.write("property list uchar int vertex_indices\n")
    outputFile.write("end_header\n")

triangleList = []

for x in range(numTriX):
    for y in range(numTriY):
        for z in range(numTriZ):
            xOffset = -((numTriX * triSpacing) / 2) + x * triSpacing
            yOffset = -((numTriY * triSpacing) / 2) + y * triSpacing
            zOffset = startingZ + z * triSpacing
            newTriangle = triangle(xOffset, yOffset, zOffset)
            triangleList.append(newTriangle)

print("writing to file")

with open("arrayOfTriangles.ply", "w") as triFile:
    PrintHeader(triFile, triangleList)
    for tri in triangleList:
        for v in tri.getVertices():
            triFile.write(v.getVertexString())
    for t in triangleList:
        triangleProperties = "200 100 200 500.0 "
        triFile.write(t.getVertexIndiciesList(triangleProperties, triangleList.index(t)))

print("finished writing, wrote {0} triangles".format(len(triangleList)))
