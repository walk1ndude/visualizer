#ifndef CTPROCESSING_HPP
#define CTPROCESSING_HPP

#if defined(_WIN32)

#include <stdint.h>

typedef uint8_t u_int8_t;
typedef uint16_t u_int16_t;
typedef uint32_t u_int32_t;

#endif

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>

#include <QtCore/QDebug>

#define RADON_DEGREE_RANGE 180
#define PI_TIMES_2 (2 * CV_PI)
#define toRad(x) ((x) * CV_PI / 180.0)

#define FROM_16_TO_8 0.00390625
#define FROM_8_TO_16 256.0

#define GRAD_WIDTH 256
#define GRAD_HEIGHT 256

inline void radon(const cv::Mat & src, cv::Mat & dst, const std::vector<cv::Mat> & rotationMatrix,
                  const int & theta,
                  const int & width, const int & height,
                  const int & wPad, const int & hPad) {
    assert(src.type() == CV_16UC1);

    dst = cv::Mat::zeros(theta, src.cols, CV_32FC1);

    cv::Mat imgPad(cv::Mat::zeros(hPad, wPad, CV_16UC1));
    cv::Mat imgPadRotated(cv::Mat::zeros(hPad, wPad, CV_16UC1));

    src.copyTo(imgPad(cv::Rect(ceil((wPad - width) / 2.0), ceil((hPad - height) / 2.0), width, height)));

    cv::Size size(imgPad.rows, imgPad.cols);
    cv::Mat sinogramCol;

    for (int angle = 0; angle != theta; ++ angle) {
        cv::warpAffine(imgPad, imgPadRotated, rotationMatrix[angle],
                       size, cv::INTER_LINEAR | cv::WARP_INVERSE_MAP, cv::BORDER_TRANSPARENT);

        sinogramCol = dst(cv::Rect(0, angle, wPad, 1));
        cv::reduce(imgPadRotated, sinogramCol, 0, CV_REDUCE_SUM, CV_32FC1);
    }
}

inline void fourier1D(const cv::Mat & src, cv::Mat dst, const std::vector<float> & dhtCoeffs) {
    assert(src.type() == CV_32FC1);

    dst = cv::Mat::zeros(src.rows, src.cols, CV_32FC1);

    int pos;
    double elem;

    for (int angle = 0; angle != src.rows; ++ angle) {

        for (int col = 0; col != src.cols; ++ col) {
            elem = 0;
            pos = (col + src.cols - src.cols / 2) % src.cols;

            for (int i = 0; i != src.cols; i ++) {
                elem += (src.at<float>(angle, i) * dhtCoeffs[i]);
            }

            dst.at<float>(angle, pos) = ((pos % 2) ? (-1) : 1) * elem / src.cols;
        }
    }
}

inline void fourier1Dto2D(const cv::Mat & src, cv::Mat & dst,
                          const std::vector<float> & sinTable, const std::vector<float> & cosTable) {
    assert(src.type() == CV_32FC1);

    dst = cv::Mat::zeros(src.rows, src.cols, CV_32FC1);

    int orgx = src.rows / 2;
    int orgy = orgx;
    int radius;
    int x;
    int y;

    for (int angle = 0; angle != src.rows; ++ angle) {
        for (int col = 0; col != src.cols; ++ col) {
            radius = col - src.rows / 2;

            x = (int) (orgx + radius * cosTable[angle]);
            y = (int) (orgy - radius * sinTable[angle]);

            dst.at<float>(y, x) = src.at<float>(angle, col);
        }
    }
}

inline void backproject(const cv::Mat & src, cv::Mat & dst,
                        const std::vector<float> & cosTable, const std::vector<float> & sinTable) {
    assert(src.type() == CV_32FC1);

    int paralProj = src.cols;
    int theta = src.rows;

    dst = cv::Mat::zeros(paralProj, paralProj, CV_32FC1);

    int midIndex = std::floor(paralProj / 2.0) + 1;

    int rotX;
    int xMin = - std::ceil(paralProj / 2.0);
    int yMin = xMin;
    int xMax = std::ceil(paralProj / 2.0 - 1);
    int yMax = xMax;

    for (int angle = 0; angle != theta; ++ angle) {
        for (int y = yMin; y != yMax; ++ y) {
            for (int x = xMin; x != xMax; ++ x) {
                rotX = (int)(midIndex - y * sinTable[angle] - x * cosTable[angle]);
                if (rotX >= 0 && rotX < paralProj) {
                    dst.at<float>(y - yMin, x - xMin) += (src.at<float>(angle, rotX) / paralProj);
                }
            }
        }
    }
}

inline void filterSlice(const cv::Mat & src, cv::Mat & dst,
                        const cv::Mat & dilateMat, const cv::Size & gaussSize) {
    cv::Mat filtered(cv::Mat::zeros(src.rows, src.cols, CV_16UC1));
/*
    cv::Mat maskRange(cv::Mat::zeros(src.rows, src.cols, CV_16UC1));
    cv::Mat masked(cv::Mat::zeros(src.rows, src.cols, CV_16UC1));
    cv::Mat maskedRange(cv::Mat::zeros(src.rows, src.cols, CV_16UC1));

    cv::inRange(src, cv::Scalar(minValue), cv::Scalar(maxValue), maskRange);

    cv::bitwise_and(src, src, masked, maskRange);
*/
    //cv::adaptiveThreshold(maskedRange, masked, 255, cv::ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY_INV, 3, 3);
    cv::dilate(src, filtered, dilateMat);
    cv::GaussianBlur(filtered, dst, gaussSize, 2.4);

    /*
    cv::Mat maskContours(cv::Mat::zeros(src.rows, src.cols, CV_16UC1));

    std::vector<std::vector<cv::Point> >contours;

    cv::findContours(maskedRange, contours, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE, cv::Point());

    for (size_t k = 0; k < contours.size(); ++ k) {
        if (contours.at(k).size() > 10) {
            cv::drawContours(maskContours, contours, k, cv::Scalar(65535), CV_FILLED);
        }
    }

    cv::bitwise_and(masked, masked, dst, maskContours);*/
    //cv::equalizeHist(filtered, dst);
}

inline void smoothSlices(const int startSlice, const int endSlice, std::vector<cv::Mat> & slices, cv::Mat & smoothed) {
    int cols = slices.at(startSlice).cols;
    int rows = slices.at(startSlice).rows;

    cv::Mat mergeMat(cv::Mat::zeros(rows, cols, CV_32FC1));
    cv::Mat conv16;

    for (int i = startSlice; i != endSlice; ++ i) {
        slices.at(i).convertTo(conv16, CV_32FC1, FROM_8_TO_16);
        mergeMat += conv16;
    }

    mergeMat /= (endSlice - startSlice);

    mergeMat.convertTo(smoothed, CV_16UC1, FROM_16_TO_8);
}

inline void mergeSlice(const int & startSlice, const int & endSlice, const int & sliceSize,
                       std::vector<cv::Mat> & slices, uchar * mergeStartPoint) {
    cv::Mat smoothed;

    smoothSlices(startSlice, endSlice, slices, smoothed);
    memcpy(mergeStartPoint, smoothed.data, sliceSize);
}

inline void checkNeighbours(const int & position, const int & neighbourDiameter,
                            const int & minNeighbour, const int & maxNeighbour,
                            std::vector<cv::Mat> & filtered, std::vector<bool> & hasMerged,
                            const int & sliceSize, uchar * mergeStartPoint) {
    int leftestMergeNeighbour = std::max(minNeighbour, position - neighbourDiameter);
    int rightestMergeNeighbour = std::min(maxNeighbour - neighbourDiameter, position + 1);

    int rightCheckNeighbour;
    int currentCheckNeighbour;

    bool canMerge;

    for (int leftCheckNeighbour = leftestMergeNeighbour; leftCheckNeighbour != rightestMergeNeighbour; ++ leftCheckNeighbour) {

        if (hasMerged[leftCheckNeighbour]) {
            continue;
        }

        rightCheckNeighbour = leftCheckNeighbour + neighbourDiameter + 1;
        currentCheckNeighbour = leftCheckNeighbour;

        canMerge = true;

        while (canMerge && currentCheckNeighbour != rightCheckNeighbour) {
            canMerge &= !filtered[currentCheckNeighbour ++].empty();
        }

        if (canMerge) {
            hasMerged[leftCheckNeighbour] = true;
            mergeSlice(leftCheckNeighbour, rightCheckNeighbour, sliceSize, filtered, mergeStartPoint + sliceSize * leftCheckNeighbour);
            }
    }
}

inline void processSlice(const int & i, std::vector<cv::Mat> & noisy, std::vector<cv::Mat> & filtered,
                         std::vector<bool> & hasMerged,
                         const int & sliceSize,
                         const cv::Mat & dilateMat, const cv::Size & gaussSize,
                         const int & neighbourDiameter, uchar * mergeLocation) {
    filterSlice(noisy[i], filtered[i], dilateMat, gaussSize);

    checkNeighbours(i, neighbourDiameter, 0, filtered.size(), filtered, hasMerged, sliceSize, mergeLocation);
}

typedef struct _DicomData {
    std::vector<float> imageSpacings;

    std::vector<char>vbuffer;

    int bytesAllocated;

    int width;
    int height;
    int depth;

    int sliceSize;

    char * buffer;

    int type;

    int minValue;
    int maxValue;

    int minHUPossible;
    int maxHUPossible;

    int minHU;
    int maxHU;

    int windowCenter;
    int windowWidth;

    bool isLittleEndian;
    bool inverseNeeded;

    float slope;
    float intercept;

    std::vector<cv::Mat *> * noisy;
    std::vector<cv::Mat *> * filtered;

    uchar ** mergeLocation;

    size_t * rowLenght;
    int * alignment;

    int neighbourRadius;

    //bool isRadonNeeded;
} DicomData;

inline void decodeSlice(const int & position, std::vector<cv::Mat> & dst, const DicomData * dicomData) {
    cv::Mat dcmToMat(cv::Mat::zeros(dicomData->width, dicomData->height, dicomData->type));

    u_int16_t pixelU;
    int16_t pixel;

    char * posInBuffer = dicomData->buffer + dicomData->sliceSize * position;

    for (int y = 0; y < dicomData->height; ++ y) {
        for (int x = 0; x < dicomData->width; ++ x) {
            pixel = 0;
            pixelU = 0;

            if (dicomData->isLittleEndian) {
                for (int k = 0; k < dicomData->bytesAllocated; ++ k) {
                    pixelU |= (u_int16_t)*(posInBuffer + k) << (8 * k);
                }
            }
            else {
                for (int k = dicomData->bytesAllocated - 1; k > 0 ; -- k) {
                    pixelU |= (u_int16_t)*posInBuffer << (8 * (dicomData->bytesAllocated - k + 1));
                }
            }

            posInBuffer += dicomData->bytesAllocated;

            /* see http://www.dabsoft.ch/dicom/3/C.11.2.1.2/*/
            pixel = dicomData->slope * pixelU + dicomData->intercept;

            if (pixel < dicomData->minHU || pixel > dicomData->maxHU) {
                dcmToMat.at<u_int16_t>(y, x) = 0;
                continue;
            }

            if (pixel <= (dicomData->windowCenter - 0.5 - (dicomData->windowWidth - 1) / 2.0)) {
                pixelU = (u_int16_t)dicomData->minValue;
            }
            else if (pixel > (dicomData->windowCenter - 0.5 + (dicomData->windowWidth - 1) / 2.0)) {
                pixelU = (u_int16_t)dicomData->maxValue;
            }
            else {
                pixelU = ((pixel - dicomData->windowCenter + 0.5) / (dicomData->windowWidth - 1) + 0.5) *
                        (dicomData->maxValue - dicomData->minValue);
            }

            dcmToMat.at<u_int16_t>(y, x) = pixelU;
        }
    }

    dst.at(position) = dcmToMat;
}

class SliceProcessing : public cv::ParallelLoopBody {
private:
    DicomData * _dicomData;

    cv::Mat _dilateMat;

    cv::Size _gaussSize;

    int _neighbourDiameter;

    mutable std::vector<bool>_hasMerged;

    mutable std::vector<cv::Mat> _noisy;
    mutable std::vector<cv::Mat> _filtered;

    int _sliceSize;

    inline void fetchCvMats(const cv::Size & newSize, const int & depth, const int & newType = CV_16UC1) {
        int slicesTotalCount = depth;
        int slicesMergeCount = slicesTotalCount - _neighbourDiameter;

        _noisy.resize(slicesTotalCount);
        _filtered.resize(slicesTotalCount);

        _dicomData->noisy->resize(slicesTotalCount);
        _dicomData->filtered->resize(slicesTotalCount);

        _hasMerged.resize(slicesMergeCount, false);

        cv::Mat dummyMat(cv::Mat::zeros(newSize, newType));

        _sliceSize = dummyMat.elemSize() * dummyMat.total();

        *(_dicomData->mergeLocation) = new uchar[_sliceSize * slicesMergeCount];

        *(_dicomData->alignment) = (dummyMat.step & 3) ? 1 : 4;
        *(_dicomData->rowLenght) = dummyMat.step1();
    }

/*
    int _pad;

    int _widthPad;
    int _heightPad;

    int _wPad;
    int _hPad;

    std::vector<cv::Mat>_rotationMatrix;

    std::vector<float>_cosTable;
    std::vector<float>_sinTable;

    std::vector<float>_dhtCoeffs;
*/
public:
    SliceProcessing(DicomData * dicomData) :
        _dicomData(dicomData) {

        _dicomData->sliceSize = _dicomData->bytesAllocated * _dicomData->width * _dicomData->height;

        _neighbourDiameter = 2 * _dicomData->neighbourRadius;

        _dilateMat = cv::Mat(3, 3, CV_8UC1);
        _gaussSize = cv::Size(3, 3);

        fetchCvMats(cv::Size(_dicomData->width, _dicomData->height), _dicomData->depth);

        /*

        if (_dicomData.isRadonNeeded) {

            //useful image is ellipsoid with ra rb as width and height,
            //all black near corners - we loss this "garbage" during rotations - good riddance
            _pad = std::max(_scaledSize.width, _scaledSize.height);

            _widthPad = std::ceil((_pad - _scaledSize.width) / 2.0);
            _heightPad = std::ceil((_pad - _scaledSize.height) / 2.0);

            _wPad = _scaledSize.width + _widthPad;
            _hPad = _scaledSize.height + _heightPad;

            float twoPiN = PI_TIMES_2 / _wPad;

            for (int angle = 0; angle < RADON_DEGREE_RANGE; ++ angle) {
                _rotationMatrix.push_back(cv::getRotationMatrix2D(cv::Point2i(
                                                                      (_scaledSize.width + _widthPad) / 2,
                                                                      (_scaledSize.height + _heightPad) / 2
                                                                      ),
                                                                  angle, 1.0));
                _cosTable.push_back(std::cos(toRad(angle)));
                _sinTable.push_back(std::sin(toRad(angle)));

            }

            for (int i = 0; i != _wPad; ++ i) {
                _dhtCoeffs.push_back(std::cos(twoPiN * i) + std::sin(twoPiN * i));
            }
        }
*/
    }

    virtual void operator ()(const cv::Range & r) const {
        for (int i = r.start; i != r.end; ++ i) {
            decodeSlice(i, _noisy, _dicomData);
            processSlice(i, _noisy, _filtered, _hasMerged, _dicomData->sliceSize,
                         _dilateMat, _gaussSize, _neighbourDiameter, *(_dicomData->mergeLocation));

            _dicomData->noisy->at(i) = new cv::Mat(_noisy.at(i));
            _dicomData->filtered->at(i) = new cv::Mat(_filtered.at(i));
        }
    }
};

#endif // CTPROCESSING_HPP
