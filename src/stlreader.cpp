#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <QtCore/QDataStream>
#include <QtCore/QUrl>

#include <QtCore/QDebug>

#include "stlreader.h"

StlReader::StlReader(QObject * parent) :
    QObject(parent) {

}

StlReader::~StlReader() {

}

void StlReader::readFile(const QUrl & fileUrl) {
    QString fileName = fileUrl.toLocalFile();

    QFile stlFile(fileName);

    QString fileNameLower = fileName.toLower();

    if (stlFile.open(QIODevice::ReadOnly)) {
        if (fileNameLower.at(fileName.length() - 4) == 's' &&
            fileNameLower.at(fileName.length() - 3) == 't' &&
            fileNameLower.at(fileName.length() - 2) == 'l' &&
            fileNameLower.at(fileName.length() - 1) == 'a'
        ) {
            readASCII(stlFile);
        }
        else {
            readBinary(stlFile);
        }

        stlFile.close();
    }
}

QString StlReader::removeWhitespaes(QString & str) {
    while (str.length() > 0 && (str[0] == ' ' || str[0] == '\t')) {
        str.remove(0, 1);
    }

    return str;
}

void StlReader::readASCII(QFile & stlFile) {
    QTextStream fileStream(&stlFile);

    ModelInfo::ModelVertex vertex;

    QVector<ModelInfo::ModelVertex> vertices;
    QVector<GLuint> indices;

    QString readStr = fileStream.readLine();
    readStr = removeWhitespaes(readStr);

    QString floatStr;

    float x;
    float y;
    float z;

    if (readStr.indexOf("solid ") != 0) {
        emit readingErrorHappened();
        return;
    }

    uint vertexNumber = 0;

    bool outerLoopNotClosed = false;
    bool facetNotClosed = false;

    while (readStr.length()) {
        readStr = fileStream.readLine();
        readStr = removeWhitespaes(readStr);

        if (!readStr.length()) {
            continue;
        }

        if (vertexNumber % 3 == 0) {
            if ((!outerLoopNotClosed && readStr.indexOf("outer loop") == 0) ||
                (outerLoopNotClosed && readStr.indexOf("endloop") == 0)) {
                outerLoopNotClosed = !outerLoopNotClosed;
                continue;
            }
            else if (!facetNotClosed && readStr.indexOf("facet") == 0) {
                facetNotClosed = !facetNotClosed;

                if (readStr.indexOf("normal") == 6) {
                    floatStr = readStr.mid(13);
                    QTextStream normalStream(&floatStr);

                    normalStream >> x >> y >> z;

                    vertex.nx = (GLfloat) x;
                    vertex.ny = (GLfloat) y;
                    vertex.nz = (GLfloat) z;

                    continue;
                }
            }
            else if (facetNotClosed && readStr.indexOf("endfacet") == 0) {
                facetNotClosed = !facetNotClosed;
                continue;
            }
            else if (readStr.indexOf("vertex") != 0) {
                break;
            }
        }

        if (vertexNumber % 3 == 0 || readStr.indexOf("vertex") == 0) {
            floatStr = readStr.mid(7);
            QTextStream vertexStream(&floatStr);

            vertexStream >> x >> y >> z;

            vertex.x = (GLfloat) x;
            vertex.y = (GLfloat) y;
            vertex.z = (GLfloat) z;
        }
        else {
            break;
        }

        vertices.push_back(vertex);

        vertexNumber ++;
    }

    if (outerLoopNotClosed || facetNotClosed || readStr.indexOf("endsolid") != 0) {
        emit readingErrorHappened();
        return;
    }

    ModelInfo::ModelBuffers modelBuffers;

    //modelBuffers.vertices = QSharedPointer<QVector<ModelInfo::ModelVertex> >(&vertices);

    for (quint32 i = 0; i != vertexNumber; ++ i) {
        qDebug() << vertices[i].x << vertices[i].y << vertices[i].z
                 << vertices[i].nx << vertices[i].ny << vertices[i].nz
                 << " ";
    }

    emit modelRead(modelBuffers);
}

void StlReader::readBinary(QFile & stlFile) {
    ModelInfo::ModelVertex vertex;

    QVector<ModelInfo::ModelVertex> vertices;

    float triangle[12];
    float normals[3];

    QByteArray buffer = stlFile.readAll();
    stlFile.close();

    if (!buffer.size()) {
        emit readingErrorHappened();
        return;
    }

    char * bufferPos = buffer.data() + 80;

    quint32 indexCount;
    memcpy(&indexCount, bufferPos, 4);

    bufferPos += 4;

    if (buffer.size() - 84 != (int) indexCount * 50) {
        emit readingErrorHappened();
        return;
    }

    for (quint32 t = 0; t != indexCount; ++ t) {
        memcpy(normals, bufferPos, 12);
        bufferPos += 12;

        memcpy(triangle, bufferPos, 36);
        bufferPos += 36;

        for (int i = 0; i != 3; ++ i) {
            vertex.x = (GLfloat) triangle[3 * i];
            vertex.y = (GLfloat) triangle[3 * i + 1];
            vertex.z = (GLfloat) triangle[3 * i + 2];

            vertex.nx = (GLfloat) normals[0];
            vertex.ny = (GLfloat) normals[1];
            vertex.nz = (GLfloat) normals[2];

            vertices.push_back(vertex);
        }

        bufferPos += 2;
    }

    for (quint32 i = 0; i != indexCount; ++ i) {
        qDebug() << vertices[i].x << vertices[i].y << vertices[i].z
                 << vertices[i].nx << vertices[i].ny << vertices[i].nz
                 << " ";
    }
    qDebug() << indexCount;
}

