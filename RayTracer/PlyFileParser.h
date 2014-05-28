//
//  PlyFileParser.h
//  RayTracer
//
//  Created by Daniel Shih on 5/25/14.
//  Copyright (c) 2014 Daniel Shih. All rights reserved.
//

#ifndef __RayTracer__PlyFileParser__
#define __RayTracer__PlyFileParser__

#include <iostream>
#include <queue>
#include <QtCore/QString>
#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <QtCore/QRegularExpression>
#include "ModelObject.h"
#include "Triangle.h"
#include "Sphere.h"

using namespace std;

struct PropertyInfo
{
    string type;
    string name;
};

struct ElementInfo
{
    string elementName;
    int elementCount;
    queue<PropertyInfo> properties;
};

class PlyFileParser
{
public:
    static ModelObject** ParseFile(string fileName, int* outModelLength);
private:
    static void ParseDataLine(QString line, ElementInfo elementInfo);
//    static Triangle* ParseTriangle(QString line);
    static Vector3D ParseVertex(QString line);
    static Triangle* ParseTriangle(QString line);
    static Sphere* ParseSphere(QString line);
    static QList<Vector3D> VertexCache;
    static QList<ModelObject*> ModelCache;
    static ModelObject** ConvertToArray(QList<ModelObject*> modelCache, int* outModelLength);
};

#endif /* defined(__RayTracer__PlyFileParser__) */
