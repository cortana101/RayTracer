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
    static vector<Triangle*> ParseFile(string fileName);
    static vector<Triangle*> ParseFile(string fileName, double scale, Vector3D translation);
private:
    static void ParseDataLine(QString line, ElementInfo elementInfo, double scale, Vector3D translation);
//    static Triangle* ParseTriangle(QString line);
    static Vector3D ParseVertex(QString line, double scale, Vector3D translation);
    static Triangle* ParseTriangle(QString line);
    static Sphere* ParseSphere(QString line);
    static QList<Vector3D> VertexCache;
    static QList<ModelObject*> ModelCache;
    static vector<Triangle*> ConvertToVector(QList<ModelObject*> modelCache);
};

#endif /* defined(__RayTracer__PlyFileParser__) */
