#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <QtCore/QDataStream>
#include <QtCore/QUrl>

#include <QtCore/QDebug>

#include <cmath>

#include "Parser/StlReader.h"
#include "Parser/Helpers.hpp"

#include <opencv2/core/core.hpp>

inline QString removeWhitespaes(QString & str) {
    while (str.length() > 0 && (str[0] == ' ' || str[0] == '\t')) {
        str.remove(0, 1);
    }

    return str;
}

class BufferData {
public:
    char * src;
    ModelInfo::VerticesVNPtr * dst;

    QScopedPointer<ModelInfo::VertexV> * minV;
    QScopedPointer<ModelInfo::VertexV> * maxV;

    quint32 vertexCount;
};

class BufferParallelReading : public cv::ParallelLoopBody {
private:
    BufferData _bufferData;

    mutable cv::Mutex _mutex;

public:
    BufferParallelReading(BufferData & bufferData) {
        _bufferData.src = bufferData.src;
        _bufferData.dst = bufferData.dst;

        _bufferData.minV = bufferData.minV;
        _bufferData.maxV = bufferData.maxV;

        (*_bufferData.dst)->resize(bufferData.vertexCount);
    }

    virtual void operator ()(const cv::Range & r) const {
        for (int i = r.start; i != r.end; ++ i) {
            char * currentPos = _bufferData.src + i * 50;

            GLfloat triangle[12];
            GLfloat normals[3];

            GLfloat x;
            GLfloat y;
            GLfloat z;

            memcpy((GLfloat *) normals, currentPos, 12);
            memcpy((GLfloat *) triangle, currentPos + 12, 36);

            for (int v = 0; v != 3; ++ v) {
                x = triangle[3 * v];
                y = triangle[3 * v + 1];
                z = triangle[3 * v + 2];

                ModelInfo::VertexVN vertex(x, y, z, normals[0], normals[1], normals[2]);

                _mutex.lock();

                if (_bufferData.minV->isNull()) {
                    (*(_bufferData.minV)).reset(new ModelInfo::VertexV(x, y, z));
                }
                else {
                    _bufferData.minV->data()->x = std::min(x, _bufferData.minV->data()->x);
                    _bufferData.minV->data()->y = std::min(y, _bufferData.minV->data()->y);
                    _bufferData.minV->data()->z = std::min(z, _bufferData.minV->data()->z);
                }

                if (_bufferData.maxV->isNull()) {
                    (*(_bufferData.maxV)).reset(new ModelInfo::VertexV(x, y, z));
                }
                else {
                    _bufferData.maxV->data()->x = std::max(x, _bufferData.maxV->data()->x);
                    _bufferData.maxV->data()->y = std::max(y, _bufferData.maxV->data()->y);
                    _bufferData.maxV->data()->z = std::max(z, _bufferData.maxV->data()->z);
                }

                _mutex.unlock();

                (**_bufferData.dst)[3 * i + v] = vertex;
            }
        }
    }
};

class NormData {
public:
    ModelInfo::VerticesVNPtr * data;

    ModelInfo::VertexV minV;
    ModelInfo::VertexV maxV;
};

class ParallelNormalizing : public cv::ParallelLoopBody {
private:
    NormData _normData;

    QVector3D _scale;

public:
    ParallelNormalizing(NormData & normData) {
        _normData.data = normData.data;

        _normData.minV = normData.minV;
        _normData.maxV = normData.maxV;

        _scale = scaleVector<float, QVector3D>(normData.maxV.x - normData.minV.x,
                                               normData.maxV.y - normData.minV.y,
                                               normData.maxV.z - normData.minV.z);
    }

    virtual void operator ()(const cv::Range & r) const {
        for (int i = r.start; i != r.end; ++ i) {
            ModelInfo::VertexVN vertex = (*_normData.data)->at(i);

            vertex.x = normalize<GLfloat>(vertex.x, _normData.minV.x, _normData.maxV.x, - _scale.x(), _scale.x());
            vertex.y = normalize<GLfloat>(vertex.y, _normData.minV.y, _normData.maxV.y, - _scale.y(), _scale.y());
            vertex.z = normalize<GLfloat>(vertex.z, _normData.minV.z, _normData.maxV.z, - _scale.z(), _scale.z());

            (**_normData.data)[i] = vertex;
        }
    }
};

namespace Parser {
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

    void StlReader::readASCII(QFile & stlFile) {
        QTextStream fileStream(&stlFile);

        ModelInfo::VertexVN vertex;
        ModelInfo::VerticesVNPtr vertices = new ModelInfo::VerticesVN;

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

        QScopedPointer<ModelInfo::VertexV> minV;
        QScopedPointer<ModelInfo::VertexV> maxV;

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

                if (minV) {
                    minV->x = std::min(minV->x, vertex.x);
                    minV->y = std::min(minV->y, vertex.y);
                    minV->z = std::min(minV->z, vertex.z);
                }
                else {
                    minV.reset(new ModelInfo::VertexV(vertex.x, vertex.y, vertex.z));
                }

                if (maxV) {
                    maxV->x = std::max(maxV->x, vertex.x);
                    maxV->y = std::max(maxV->y, vertex.y);
                    maxV->z = std::max(maxV->z, vertex.z);
                }
                else {
                    maxV.reset(new ModelInfo::VertexV(vertex.x, vertex.y, vertex.z));
                }
            }
            else {
                break;
            }

            vertices->push_back(vertex);

            vertexNumber ++;
        }

        if (outerLoopNotClosed || facetNotClosed || readStr.indexOf("endsolid") != 0) {
            emit readingErrorHappened();
            return;
        }

        NormData normData;
        normData.data = &vertices;
        normData.minV = *minV;
        normData.maxV = *maxV;

        cv::parallel_for_(cv::Range(0, vertexNumber), ParallelNormalizing(normData));

        ModelInfo::BuffersVN buffers;
        buffers.vertices = ModelInfo::VerticesVNPointer(vertices);

        emit modelRead(buffers);
    }

    void StlReader::readBinary(QFile & stlFile) {
        ModelInfo::VerticesVNPtr vertices = new ModelInfo::VerticesVN;

        QByteArray buffer = stlFile.readAll();

        if (!buffer.size()) {
            emit readingErrorHappened();
            return;
        }

        char * bufferPos = buffer.data() + 80;

        quint32 triangleCount;
        memcpy(&triangleCount, bufferPos, 4);

        bufferPos += 4;

        if (buffer.size() - 84 != (int) triangleCount * 50) {
            emit readingErrorHappened();
            return;
        }

        BufferData bufferData;

        bufferData.src = bufferPos;
        bufferData.dst = &vertices;
        bufferData.vertexCount = triangleCount * 3;

        QScopedPointer<ModelInfo::VertexV> minV;
        QScopedPointer<ModelInfo::VertexV> maxV;

        bufferData.minV = &minV;
        bufferData.maxV = &maxV;

        // no triangles -> no need to create buffers, etc
        if (!triangleCount) {
            return;
        }

        float startTime = cv::getTickCount() / cv::getTickFrequency();

        cv::parallel_for_(cv::Range(0, triangleCount), BufferParallelReading(bufferData));

        NormData normData;
        normData.data = &vertices;
        normData.minV = *minV;
        normData.maxV = *maxV;

        cv::parallel_for_(cv::Range(0, bufferData.vertexCount), ParallelNormalizing(normData));

        qDebug() << "Elapsed Time: " << cv::getTickCount() / cv::getTickFrequency() - startTime;

        ModelInfo::BuffersVN buffers;
        buffers.vertices = ModelInfo::VerticesVNPointer(vertices);

        emit modelRead(buffers);
    }
}
