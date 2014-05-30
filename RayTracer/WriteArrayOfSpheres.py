
#Sphere array dimensions
numSpheresX = 10
numSpheresY = 10
numSpheresZ = 10

#Sphere array parameters
startingZ = 3.0
sphereSpacing = 2.0

sphereCount = numSpheresX * numSpheresY * numSpheresZ

def PrintHeader(outputFile, count):
    outputFile.write("ply\n")
    outputFile.write("format ascii 1.0\n")
    outputFile.write("element sphere {0}\n".format(count))
    outputFile.write("property float32 colorR\n")
    outputFile.write("property float32 colouG\n")
    outputFile.write("property float32 colorB\n")
    outputFile.write("property float32 gloss\n")
    outputFile.write("property float32 centerX\n")
    outputFile.write("property float32 centerY\n")
    outputFile.write("property float32 centerZ\n")
    outputFile.write("property float32 radius\n")
    outputFile.write("end_header\n")

def PrintSphere(outputFile, colourR, colourG, colourB, gloss, centerX, centerY, centerZ, radius):
    outputFile.write("{0} {1} {2} {3} {4} {5} {6} {7}\n".format(colourR, colourG, colourB, gloss, centerX, centerY, centerZ, radius))

sphereRadius = 0.5
sphereGloss = 200.0
sphereColourR = 200
sphereColourG = 200
sphereColourB = 200
    
with open("arrayOfSpheres.ply", "w") as output:
    print("writing file...")
    PrintHeader(output, sphereCount)
    for x in range(numSpheresX):
        for y in range(numSpheresY):
            for z in range(numSpheresZ):
                xOffset = -((numSpheresX * sphereSpacing)/ 2) + x * sphereSpacing
                yOffset = -((numSpheresY * sphereSpacing)/ 2) + y * sphereSpacing
                zOffset = startingZ + z * sphereSpacing
                PrintSphere(output,
                    sphereColourR,
                    sphereColourG,
                    sphereColourB,
                    sphereGloss,
                    xOffset,
                    yOffset,
                    zOffset,
                    sphereRadius)
    print("finished writing output to {0}, wrote {1} spheres".format(output.name, sphereCount))
                     
    
