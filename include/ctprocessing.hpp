#ifndef CTPROCESSING_HPP
#define CTPROCESSING_HPP

#if defined(_WIN32)

#include <stdint.h>

typedef uint8_t u_int8_t;
typedef uint16_t u_int16_t;
typedef uint32_t u_int32_t;

#endif

#include <QtCore/QDebug>

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/ocl/ocl.hpp>

#define RADON_DEGREE_RANGE 180
#define PI_TIMES_2 (2 * CV_PI)
#define toRad(x) ((x) * CV_PI / 180.0)

#define FROM_16_TO_8 0.00390625
#define FROM_8_TO_16 256.0

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

inline void filterSlice(const cv::Mat & src, cv::Mat & dst, const int & minValue, const int & maxValue,
                        const cv::Mat & dilateMat, const cv::Size & gaussSize) {
    dst = cv::Mat::zeros(src.rows, src.rows, CV_8UC1);

    cv::Mat maskRange(cv::Mat::zeros(src.rows, src.cols, CV_8UC1));
    cv::Mat maskedRange(cv::Mat::zeros(src.rows, src.cols, CV_8UC1));

    cv::inRange(src, cv::Scalar(minValue), cv::Scalar(maxValue), maskRange);

    cv::bitwise_and(src, src, maskedRange, maskRange);

    cv::adaptiveThreshold(maskedRange, maskedRange, 255, cv::ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY_INV, 3, 5);
    cv::dilate(maskedRange, maskedRange, dilateMat);
    cv::GaussianBlur(maskedRange, maskedRange, gaussSize, 0.5);

    cv::Mat maskContours(cv::Mat::zeros(src.rows, src.cols, CV_8UC1));

    std::vector<std::vector<cv::Point> >contours;

    cv::findContours(maskedRange, contours, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE, cv::Point());

    for (size_t k = 0; k < contours.size(); ++ k) {
        if (contours.at(k).size() > 10) {
            cv::drawContours(maskContours, contours, k, cv::Scalar(255), CV_FILLED);
        }
    }

    cv::bitwise_and(maskedRange, maskedRange, dst, maskContours);
    cv::equalizeHist(dst, dst);
}

inline void smoothSlices(const int startSlice, const int endSlice, std::vector<cv::Mat> & slices, cv::Mat & smoothed) {
    int cols = slices.at(startSlice).cols;
    int rows = slices.at(startSlice).rows;

    cv::Mat mergeMat(cv::Mat::zeros(rows, cols, CV_16UC1));
    cv::Mat conv16(cols, rows, CV_16UC1);

    for (int i = startSlice; i != endSlice; ++ i) {
        slices.at(i).convertTo(conv16, CV_16UC1, FROM_8_TO_16);
        mergeMat += conv16;
    }

    mergeMat /= (endSlice - startSlice);

    mergeMat.convertTo(smoothed, CV_8UC1, FROM_16_TO_8);
}

inline void mergeSlice(const int & startSlice, const int & endSlice, const int & sliceSize,
                       std::vector<cv::Mat> & slices, uchar * mergeStartPoint) {

    cv::Mat smoothed;

    smoothSlices(startSlice, endSlice, slices, smoothed);
    memcpy(mergeStartPoint, smoothed.data, sliceSize);
}

inline void checkNeighbours(const int & position, const int & neighbourDiameter,
                            const int & minNeighbour, const int & maxNeighbour,
                            const std::vector<bool> & hasProcessed, std::vector<bool> & hasMerged,
                            std::vector<cv::Mat> & slices, const int & sliceSize,
                            uchar * mergeStartPoint) {
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
            canMerge &= hasProcessed[currentCheckNeighbour ++];
        }

        if (canMerge) {
            hasMerged[leftCheckNeighbour] = true;
            mergeSlice(leftCheckNeighbour, rightCheckNeighbour, sliceSize, slices, mergeStartPoint + sliceSize * leftCheckNeighbour);
        }
    }
}

typedef struct _FilterData {
    std::vector<cv::Mat *> * src;

    uchar ** mergeLocation;

    size_t * rowLenght;

    int * alignment;

    int neighbourRadius;

    int minValue;
    int maxValue;
} FilterData;

inline void processSlice(const int & i, std::vector<cv::Mat> & filtered, const FilterData & filterData,
                         std::vector<bool> & hasProcessed, std::vector<bool> & hasMerged,
                         const int & sliceSize, const cv::Mat & dilateMat,
                         const cv::Size & gaussSize, const int & neighbourDiameter) {
    filterSlice(*(filterData.src->at(i)), filtered[i], filterData.minValue, filterData.maxValue, dilateMat, gaussSize);

    hasProcessed[i] = true;

    checkNeighbours(i, neighbourDiameter, 0, filtered.size(), hasProcessed, hasMerged, filtered,
                    sliceSize, *(filterData.mergeLocation));
}

class SliceFilter : public cv::ParallelLoopBody {
protected:
    FilterData _filterData;

    cv::Mat _dilateMat;

    cv::Size _gaussSize;

    int _neighbourDiameter;

    mutable std::vector<bool>_hasProcessed; // because flags need to be changeds in const method
    mutable std::vector<bool>_hasMerged;

    mutable std::vector<cv::Mat>_filtered;

    int _sliceSize;

    inline void fetchCvMats(const cv::Size & newSize, const int & newType = CV_8UC1) {
        _hasProcessed.resize(_filterData.src->size(), false);
        _hasMerged.resize(_filterData.src->size() - _neighbourDiameter, false);

        _filtered.resize(_filterData.src->size());

        cv::Mat dummyMat(cv::Mat::zeros(newSize, newType));

        _sliceSize = dummyMat.elemSize() * dummyMat.total();

        *(_filterData.mergeLocation) = new uchar[_sliceSize * (_filterData.src->size() - _neighbourDiameter)];

        *(_filterData.alignment) = (dummyMat.step & 3) ? 1 : 4;
        *(_filterData.rowLenght) = dummyMat.step1();
    }

public:
    SliceFilter(FilterData & filterData) :
        _filterData(filterData) {

        _neighbourDiameter = 2 * _filterData.neighbourRadius;

        _dilateMat = cv::Mat(3, 3, CV_8UC1);
        _gaussSize = cv::Size(3, 3);

        if (_filterData.src->size()) { // it could be uninitialized
            fetchCvMats(_filterData.src->at(0)->size());
        }
    }

    virtual void operator ()(const cv::Range & r) const {
        for (int i = r.start; i != r.end; ++ i) {
            processSlice(i, _filtered, _filterData, _hasProcessed, _hasMerged, _sliceSize,
                         _dilateMat, _gaussSize, _neighbourDiameter);
        }
    }
};

typedef struct _DicomData {
    std::vector<float> * imageSpacing;

    int bytesAllocated;

    int width;
    int height;
    int depth;

    int sliceSize;
    char * buffer;

    int type;

    int64_t minValue;
    int64_t maxValue;
    int64_t windowCenter;
    int64_t windowWidth;

    bool isLittleEndian;
    bool inverseNeeded;

    float slope;
    float intercept;

    bool isRadonNeeded;
} DicomData;

template <typename T> inline void decodeSlice(const int & position, std::vector<cv::Mat *> * dst,
                                              const cv::Size scaledSize, const DicomData & dicomData) {

    cv::Mat dcmToMat(cv::Mat::zeros(dicomData.width, dicomData.height, dicomData.type));

    T pixel;

    char * posInBuffer = dicomData.buffer + dicomData.sliceSize * position;

    for (int y = 0; y < dicomData.height; ++ y) {
        for (int x = 0; x < dicomData.width; ++ x) {
            pixel = 0;

            if (dicomData.isLittleEndian) {
                for (int k = 0; k < dicomData.bytesAllocated; ++ k) {
                    pixel |= (T)*(posInBuffer + k) << (8 * k);
                }
            }
            else {
                for (int k = dicomData.bytesAllocated - 1; k > 0 ; -- k) {
                    pixel |= (T)*posInBuffer << (8 * (dicomData.bytesAllocated - k + 1));
                }
            }

            posInBuffer += dicomData.bytesAllocated;

            /* similar as http://code.google.com/p/pydicom/source/browse/source/dicom/contrib/pydicom_PIL.py*/
            pixel = dicomData.slope * pixel + dicomData.intercept;

            if (pixel <= (dicomData.windowCenter - 0.5 - (dicomData.windowWidth - 1) / 2.0)) {
                pixel = (T)dicomData.minValue;
            }
            else if (pixel > (dicomData.windowCenter - 0.5 + (dicomData.windowWidth - 1) / 2.0)) {
                pixel = (T)dicomData.maxValue;
            }
            else {
                pixel = ((pixel - dicomData.windowCenter + 0.5) / (dicomData.windowWidth - 1) + 0.5) *
                        (dicomData.maxValue - dicomData.minValue);
            }

            //MONOCHROME2 - high value -> brighter, -1 high -> blacker
            if (dicomData.inverseNeeded) {
                dcmToMat.at<T>(y, x) = ~pixel;
            }
            else {
                dcmToMat.at<T>(y, x) = pixel;
            }
        }
    }

    cv::resize(dcmToMat, dcmToMat, scaledSize);

    if (dicomData.type == CV_16UC1) {
        cv::Mat dcmToMat8(dicomData.width, dicomData.height, CV_8UC1);
        dcmToMat.convertTo(dcmToMat8, CV_8UC1, FROM_16_TO_8);
        dst->at(position) = new cv::Mat(dcmToMat8);
    }
    else {
        dst->at(position) = new cv::Mat(dcmToMat);
    }
}

typedef struct _LoaderData {
    DicomData dicomData;
    FilterData filterData;
} LoaderData;

template <typename T>
class SliceLoader : public SliceFilter {
private:
    DicomData _dicomData;

    int _width;
    int _height;

    cv::Size _scaledSize;

    int _pad;

    int _widthPad;
    int _heightPad;

    int _wPad;
    int _hPad;

    std::vector<cv::Mat>_rotationMatrix;

    std::vector<float>_cosTable;
    std::vector<float>_sinTable;

    std::vector<float>_dhtCoeffs;

public:
    SliceLoader(LoaderData & loaderData) : SliceFilter(loaderData.filterData) {
        _dicomData = loaderData.dicomData;
        _filterData = loaderData.filterData;

        _dicomData.sliceSize = _dicomData.bytesAllocated * _dicomData.width * _dicomData.height;

        _dicomData.imageSpacing->at(0) *= (float) 2.0;
        _dicomData.imageSpacing->at(1) *= (float) 2.0;

        _width = _dicomData.width / 2;
        _height = _dicomData.height / 2;

        _scaledSize = cv::Size(_width, _height);

        _filterData.src->resize(_dicomData.depth);

        fetchCvMats(_scaledSize);

        if (_dicomData.isRadonNeeded) {

            //useful image is ellipsoid with ra rb as width and height,
            //all black near corners - we loss this "garbage" during rotations - good riddance
            _pad = std::max(_width, _height);

            _widthPad = std::ceil((_pad - _width) / 2.0);
            _heightPad = std::ceil((_pad - _height) / 2.0);

            _wPad = _width + _widthPad;
            _hPad = _height + _heightPad;

            float twoPiN = PI_TIMES_2 / _wPad;

            for (int angle = 0; angle < RADON_DEGREE_RANGE; ++ angle) {
                _rotationMatrix.push_back(cv::getRotationMatrix2D(cv::Point2i(
                                                                      (_width + _widthPad) / 2,
                                                                      (_height + _heightPad) / 2
                                                                      ),
                                                                  angle, 1.0));
                _cosTable.push_back(std::cos(toRad(angle)));
                _sinTable.push_back(std::sin(toRad(angle)));

            }

            for (int i = 0; i != _wPad; ++ i) {
                _dhtCoeffs.push_back(std::cos(twoPiN * i) + std::sin(twoPiN * i));
            }
        }

    }

    virtual void operator ()(const cv::Range & r) const {
        for (int i = r.start; i != r.end; ++ i) {
            decodeSlice<T>(i, _filterData.src, _scaledSize, _dicomData);

            processSlice(i, _filtered, _filterData, _hasProcessed, _hasMerged, _sliceSize,
                         _dilateMat, _gaussSize, _neighbourDiameter);
        }
    }
};

#endif // CTPROCESSING_HPP
