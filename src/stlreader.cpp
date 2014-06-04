#include <QtCore/QFile>
#include <QtCore/QTextStream>
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

    if (stlFile.open(QIODevice::ReadOnly)) {
        if (fileName.at(fileName.length() - 4) == 's' &&
            fileName.at(fileName.length() - 3) == 't' &&
            fileName.at(fileName.length() - 2) == 'l' &&
            fileName.at(fileName.length() - 1) == 'a'
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

    while (readStr.length() != 0) {
        readStr = fileStream.readLine();
        readStr = removeWhitespaes(readStr);

        if (readStr.length() == 0) {
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
        indices.push_back(vertexNumber ++);
    }

    if (outerLoopNotClosed || facetNotClosed || readStr.indexOf("endsolid") != 0) {
        emit readingErrorHappened();
        return;
    }

    ModelInfo::ModelBuffers modelBuffers;

    //modelBuffers.vertices = QSharedPointer<QVector<ModelInfo::ModelVertex> >(&vertices);
    //modelBuffers.indices = QSharedPointer<QVector<GLuint> >(&indices);

    for (uint i = 0; i != vertexNumber; ++ i) {
        qDebug() << vertices[i].x << vertices[i].y << vertices[i].z
                 << vertices[i].nx << vertices[i].ny << vertices[i].nz << indices[i] << " ";
    }

    emit modelRead(modelBuffers);
}

void StlReader::readBinary(QFile & fileName) {

}
