//
//  PlyFileParser.cpp
//  RayTracer
//
//  Created by Daniel Shih on 5/25/14.
//  Copyright (c) 2014 Daniel Shih. All rights reserved.
//

#include "PlyFileParser.h"
#include <fstream>
#include <regex>

// The expected types for this parser is
// Vertex { float x, float y, float z },
// Triangle/Face { float colorR, float colorG, float colorB, float gloss, list[Vertex indicies]},
// Sphere { float colorR, float colorG, float colorB, float gloss, float centerX, float centerY, float centerZ, float radius }
// We wont parse any other types of things

QList<Vector3D> PlyFileParser::VertexCache;
QList<ModelObject*> PlyFileParser::ModelCache;

vector<Triangle*> PlyFileParser::ParseFile(string fileName)
{
    return ParseFile(fileName, 1.0, Vector3D(0.0, 0.0, 0.0));
}

vector<Triangle*> PlyFileParser::ParseFile(string fileName, double scale, Vector3D translation)
{
    VertexCache.clear();
    ModelCache.clear();
    QString qfileName = QString::fromStdString(fileName);
    QFile qFile (qfileName);
    qFile.open(QIODevice::ReadOnly);

    QRegularExpression elementTag = QRegularExpression("(element)\\s(.*?)\\s([0-9]*)");
    QRegularExpression propertyTag = QRegularExpression("(property)\\s(.*?)\\s(.*)");
    QRegularExpression dataLine = QRegularExpression("[0-9\\.\\-]*[\\s\\n]");
    QRegularExpression endHeader = QRegularExpression("end.*");
    
    queue<ElementInfo> elementTypes;
    
    bool finishedParsingHeader = false;
    
    //QTextStream qCout (stdout);
    
    while (!qFile.atEnd())
    {
        QString line = qFile.readLine();
        
        QRegularExpressionMatch elementMatches = elementTag.match(line);

        if(!finishedParsingHeader)
        {
            if (elementMatches.hasMatch())
            {
                ElementInfo elementInfo;
                
                elementInfo.elementName = elementMatches.captured(2).toStdString();
                elementInfo.elementCount = elementMatches.captured(3).toInt();
                
                elementTypes.push(elementInfo);
            }
            
            QRegularExpressionMatch propertyMatch = propertyTag.match(line);
            
            if (propertyMatch.hasMatch())
            {
                PropertyInfo propertyInfo;
                propertyInfo.type = propertyMatch.captured(2).toStdString();
                propertyInfo.name = propertyMatch.captured(3).toStdString();
                
                elementTypes.front().properties.push(propertyInfo);
            }
        }
        else
        {
            ParseDataLine(line, elementTypes.front(), scale, translation);

            elementTypes.front().elementCount--;
            
            if (elementTypes.front().elementCount == 0)
            {
                elementTypes.pop();
            }
        }
        
        if(line.contains(QRegularExpression("end.*")))
        {
            finishedParsingHeader = true;
        }
        
        //qCout << line;
        //qCout.flush();
    }
    
    qFile.close();
    
    return ConvertToVector(ModelCache);
}

void PlyFileParser::ParseDataLine(QString line, ElementInfo elementInfo, double scale, Vector3D translation)
{
    QString qElementName = QString::fromStdString(elementInfo.elementName);
    
    if (QString::compare(qElementName, "vertex", Qt::CaseInsensitive) == 0)
    {
        Vector3D vertex = ParseVertex(line, scale, translation);
        VertexCache.append(vertex);
    }
    else if (QString::compare(qElementName, "triangle", Qt::CaseInsensitive) == 0 || QString::compare(qElementName, "face", Qt::CaseInsensitive) == 0)
    {
        Triangle* triangle = ParseTriangle(line);
        ModelCache.append(triangle);
    }
    else if (elementInfo.elementName == "sphere")
    {
        Sphere* sphere = ParseSphere(line);
        ModelCache.append(sphere);
    }
}

Vector3D PlyFileParser::ParseVertex(QString line, double scale, Vector3D translation)
{
    QStringList dataValues = line.split(" ");
    
    if (dataValues.length() < 3)
    {
        throw "Incorrect number of fields specified for vertex data";
    }
    
    double x = dataValues.at(0).toDouble();
    double y = dataValues.at(1).toDouble();
    double z = dataValues.at(2).toDouble();
    
    return Vector3D(x, y, z).Scale(scale).Add(translation);
}

Triangle* PlyFileParser::ParseTriangle(QString line)
{
    QStringList dataValues = line.split(" ");
    string lineString = line.toStdString();
    
    if (dataValues.length() < 8)
    {
        throw "Incorrect number of fields specified for vertex data";
    }
    
    double colourR = dataValues.at(0).toDouble();
    double colourG = dataValues.at(1).toDouble();
    double colourB = dataValues.at(2).toDouble();
    double gloss = dataValues.at(3).toDouble();
    int v1Index = dataValues.at(5).toInt();
    int v2Index = dataValues.at(6).toInt();
    int v3Index = dataValues.at(7).toInt();
    
    Triangle* output = new Triangle();
    output->p1 = VertexCache.at(v1Index);
    output->p2 = VertexCache.at(v2Index);
    output->p3 = VertexCache.at(v3Index);
    output->colour = Colour(colourR, colourG, colourB);
    output->gloss = gloss;
    
    return output;
}

Sphere* PlyFileParser::ParseSphere(QString line)
{
    QStringList dataValues = line.split(" ");
    
    if (dataValues.length() != 8)
    {
        throw "Incorrect number of fields specified for vertex data";
    }
    
    double colourR = dataValues.at(0).toDouble();
    double colourG = dataValues.at(1).toDouble();
    double colourB = dataValues.at(2).toDouble();
    double gloss = dataValues.at(3).toDouble();
    double centerX = dataValues.at(4).toDouble();
    double centerY = dataValues.at(5).toDouble();
    double centerZ = dataValues.at(6).toDouble();
    double radius = dataValues.at(7).toDouble();

    Vector3D center (centerX, centerY, centerZ);
    Sphere* output = new Sphere(center, radius);
    output->colour = Colour(colourR, colourG, colourB);
    output->gloss = gloss;
    return output;
}

vector<Triangle*> PlyFileParser::ConvertToVector(QList<ModelObject*> modelCache)
{
    int modelLength = modelCache.count();
    
    // Only support triangles at this point, spheres may be added in later
    vector<Triangle*> modelVector;
    
    for (int i = 0; i < modelLength; i++)
    {
        Triangle* triangleModel = dynamic_cast<Triangle*>(modelCache.at(i));
        modelVector.push_back(triangleModel);
    }
    
    return modelVector;
}