#ifndef PARALLELPROCESSING_HPP
#define PARALLELPROCESSING_HPP

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <QtCore/QDebug>

#define toRad(x) ((x) * CV_PI / 180.0)

#define PADDED_INCREASE 1.5f;

inline void calcSinogram(const int & row, const int & paddedSize,
                         const QVector<cv::Mat> * src, cv::Mat & dst) {
    int rowWidth = src->at(0).cols;

    cv::Mat sinogram;
    cv::Mat sinogramRow(cv::Mat::zeros(1, paddedSize, CV_32FC1));

    int startPoint = (paddedSize - rowWidth) / 2;

    for (int i = 0; i != src->size(); ++ i) {
        (src->at(i)(cv::Rect(0, row, rowWidth, 1))).copyTo(sinogramRow(cv::Rect(startPoint, 0, rowWidth, 1)));

        sinogram.push_back(sinogramRow);

        sinogramRow = cv::Scalar(0);
    }

    sinogram.copyTo(dst);
}

template <class T>
inline void dht(const cv::Mat & src, cv::Mat & dst, const QVector<QVector<float> > & cas,
                const bool inverted = false) {
    cv::Mat fft1(cv::Mat::zeros(src.rows, src.cols, CV_32FC1));

    float elem;

    qDebug() << src.rows << src.cols;

    for (int angle = 0; angle != src.rows; ++ angle) {
        for (int col = 0; col != src.cols; ++ col) {
            elem = 0.0f;
            for (int i = 0; i != src.cols; i ++) {
                elem += (src.at<T>(angle, i) * cas[col][i]);
            }
            fft1.at<float>(angle, col) = elem * (inverted ? (1 / (float) src.cols) : 1);
        }
    }

    fft1.copyTo(dst);
}

template <class T>
inline void dht2D(const cv::Mat & src, cv::Mat & dst, const QVector<QVector<float> > & cas,
                  const int & unpaddedSide,
                  const bool inverted = false) {
    cv::Mat hRow;
    cv::Mat hRowT;
    cv::Mat hCol;
    cv::Mat hColT;

    dht<T>(src, hRow, cas, inverted);

    cv::transpose(hRow, hRowT);

    dht<T>(hRowT, hCol, cas, inverted);

    cv::transpose(hCol, hColT);

    int mRow;
    int mCol;
    float A;
    float B;
    float C;
    float D;
    float E;

    float * data = (float *) hColT.data;

    for (int row = 0; row <= src.cols / 2; ++ row) { // Now calculate actual Hartley transform
        for (int col = 0; col <= src.cols / 2; ++ col) {
                if (row * row + col * col <= src.cols * src.cols / 4) {
                    mRow = (src.cols - row) % src.cols;
                    mCol = (src.cols - col)  % src.cols;
                    A = data[row * src.cols + col];	//  see Bracewell, 'Fast 2D Hartley Transf.' IEEE Procs. 9/86
                    B = data[mRow * src.cols + col];
                    C = data[row * src.cols + mCol];
                    D = data[mRow * src.cols + mCol];
                    E = ((A + D) - (B + C)) / 2;
                    data[row * src.cols + col] = A - E;
                    data[mRow * src.cols + col] = B + E;
                    data[row * src.cols + mCol] = C + E;
                    data[mRow * src.cols + mCol] = D - E;
                }
                else {
                    data[row * src.cols + col] = 0;
                    data[mRow * src.cols + col] = 0;
                    data[row * src.cols + mCol] = 0;
                    data[mRow * src.cols + mCol] = 0;
                }
        }
    }

    cv::Mat hColTUnPadded(cv::Mat::zeros(unpaddedSide, unpaddedSide, src.type()));
    //hColT(cv::Rect(src.cols / 2 - unpaddedSide, src.cols / 2 - unpaddedSide, unpaddedSide, unpaddedSide)).copyTo(hColTUnPadded);

    hColT.copyTo(dst);
}

inline void swapDht(const cv::Mat & src, cv::Mat & dst) {
    cv::Mat swapMat(cv::Mat::zeros(src.rows, src.cols, CV_32FC1));

    int pos;

    for (int angle = 0; angle != src.rows; ++ angle) {
        for (int col = 0; col != src.cols; ++ col) {
            pos = (col + src.cols / 2) % src.cols;
            swapMat.at<float>(angle, pos) = ((pos % 2) ? (-1) : 1) * src.at<float>(angle, col);
        }
    }
    swapMat.copyTo(dst);
}

inline void fourier1Dto2D(const cv::Mat & src, cv::Mat & dst,
                          const QVector<float> & sinTable, const QVector<float> & cosTable) {
    cv::Mat swapMat;

    swapDht(src, swapMat);
    cv::Mat fft2(cv::Mat::zeros(swapMat.cols, swapMat.cols, CV_32FC1));

    cv::Point2f center = cv::Point2f(swapMat.cols / 2, swapMat.cols / 2);
    cv::Point2f curPoint;

    for (int angle = 0; angle != swapMat.rows; ++ angle) {
        for (int col = 0; col != swapMat.cols; ++ col) {
            curPoint = center + (col - center.x) * cv::Point2f(cosTable[angle], - sinTable[angle]);
            fft2.at<float>(curPoint) = swapMat.at<float>(angle, col);
        }
    }

    fft2.copyTo(dst);
}

inline void swapMatParts(const cv::Mat & src, cv::Mat & dst) {
    cv::Mat swapMat(src);

    int halfRows = std::ceil(src.rows / 2);
    int halfCols = std::ceil(src.cols / 2);

    for (int y = 0; y != src.rows; ++ y) {
        for (int x = 0; x != halfCols; ++ x) {
            std::swap(swapMat.at<float>(y, x), swapMat.at<float>(y + (y > halfRows ? -1 : 1) * halfRows, x + halfCols));
        }
    }
    swapMat.copyTo(dst);
}

typedef struct _ReconstructorData {
    int sliceCount;

    QVector<cv::Mat> * src;
    QVector<cv::Mat *> * slices;
} ReconstructionData;

class ReconstructorLoop : public cv::ParallelLoopBody {
private:
    ReconstructionData * _reconstructorData;

    QVector<float>_cosTable;
    QVector<float>_sinTable;

    QVector<QVector<float> >_cas;

    int _paddedSize;

public:
    ReconstructorLoop(ReconstructionData * reconstructionData) :
        _reconstructorData(reconstructionData) {

        _reconstructorData->slices->resize(_reconstructorData->sliceCount);

        for (int angle = 0; angle != _reconstructorData->src->size(); ++ angle) {
            _cosTable.push_back(std::cos(toRad(angle)));
            _sinTable.push_back(std::sin(toRad(angle)));
        }

        //_paddedSize = std::pow(2, (std::ceil(std::log2(_reconstructorData->sliceCount)) + PADDED_INCREASE));

        _paddedSize = _reconstructorData->sliceCount * PADDED_INCREASE;

        float twoPiN = (2 * CV_PI) / _paddedSize;

        for (int i = 0; i != _paddedSize; ++ i) {
            _cas.push_back(QVector<float>());
            for (int k = 0; k != _paddedSize; ++ k) {
                _cas[i].push_back(std::cos(k * twoPiN * i) + std::sin(k * twoPiN * i));
            }
        }
    }

    virtual void operator ()(const cv::Range & r) const {
        for (int i = r.start; i != r.end; ++ i) {
            cv::Mat slice;

            calcSinogram(i, _paddedSize, _reconstructorData->src, slice);

            dht<float>(slice, slice, _cas);

            fourier1Dto2D(slice, slice, _sinTable, _cosTable);

            swapMatParts(slice, slice);

            dht2D<float>(slice, slice, _cas, _reconstructorData->src->at(0).cols, true);

            swapMatParts(slice, slice);

            cv::Mat slice8;

            double maxVal;
            double minVal;

            cv::minMaxLoc(slice, &minVal, &maxVal);

            cv::convertScaleAbs(slice, slice8,
                                32 * 256.0f / (maxVal - minVal),
                                minVal / (minVal - maxVal));

            (*_reconstructorData->slices)[i] = new cv::Mat(slice8);

            qDebug() << i << "completed";
        }
    }
};

#endif // PARALLELPROCESSING_HPP
