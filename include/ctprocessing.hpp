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

static int count = 0;

inline void calcGradientSlice(const int & position, const std::vector<cv::Mat> & smoothed, uchar * gradientStartPosition) {
    int height = smoothed.at(0).cols;
    int width = smoothed.at(0).rows;

    qDebug() << count ++;

    int startSlice = std::max(0, position - 1);
    int endSlice = std::min((int) smoothed.size() - 1, position + 1);

    cv::Vec3f p;
    cv::Vec3f n;
    cv::Vec3f normalized;

    cv::Mat dummy(height, width, CV_8UC3);

    for (int x = 0; x != width; ++ x) {
        for (int y = 0; y != height; ++ y) {
            p[0] = smoothed.at(position).at<uchar>(y, std::max(x - 1, 0));
            n[0] = smoothed.at(position).at<uchar>(y, std::min(x + 1, width));

            p[1] = smoothed.at(position).at<uchar>(std::max(y, y - 1), x);
            n[1] = smoothed.at(position).at<uchar>(std::max(y, y + 1), x);

            p[2] = smoothed.at(startSlice).at<uchar>(y, x);
            n[2] = smoothed.at(endSlice).at<uchar>(y, x);

            cv::normalize(n - p, normalized, 1.0);
/*
            *(currentPos ++) = normalized[0];
            *(currentPos ++) = normalized[1];
            *(currentPos ++) = normalized[2];
            */

            dummy.at<cv::Vec3b>(y, x)[0] = 255;
            dummy.at<cv::Vec3b>(y, x)[1] = 255;
            dummy.at<cv::Vec3b>(y, x)[2] = 255;
        }
    }

    memcpy(gradientStartPosition, dummy.data, dummy.elemSize() * dummy.total());
}

inline void filterSlice(const cv::Mat & src, cv::Mat & dst, const int & minValue, const int & maxValue,
                        const cv::Mat & dilateMat, const cv::Size & gaussSize) {
    cv::Mat filtered(cv::Mat::zeros(src.rows, src.cols, CV_8UC1));

    cv::Mat maskRange(cv::Mat::zeros(src.rows, src.cols, CV_8UC1));
    cv::Mat masked(cv::Mat::zeros(src.rows, src.cols, CV_8UC1));
    cv::Mat maskedRange(cv::Mat::zeros(src.rows, src.cols, CV_8UC1));

    cv::inRange(src, cv::Scalar(minValue), cv::Scalar(maxValue), maskRange);

    cv::bitwise_and(src, src, masked, maskRange);

    cv::adaptiveThreshold(masked, maskedRange, 255, cv::ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY_INV, 3, 3);
    cv::dilate(maskedRange, maskedRange, dilateMat);
    cv::GaussianBlur(maskedRange, maskedRange, gaussSize, 2);
/*
    std::vector<cv::Vec3f> circles;

    cv::HoughCircles(maskedRange, circles, CV_HOUGH_GRADIENT, 1, src.cols / 100, 200, 100, 0, 0);

    for (size_t i = 0; i != circles.size(); ++ i) {
        cv::circle(maskedRange, cv::Point(circles[i][0], circles[i][1]), cvRound(circles[i][2]), cv::Scalar(255), 3, 8, 0);
    }
*/
    cv::Mat maskContours(cv::Mat::zeros(src.rows, src.cols, CV_8UC1));

    std::vector<std::vector<cv::Point> >contours;

    cv::findContours(maskedRange, contours, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE, cv::Point());

    for (size_t k = 0; k < contours.size(); ++ k) {
        if (contours.at(k).size() > 10) {
            cv::drawContours(maskContours, contours, k, cv::Scalar(255), CV_FILLED);
        }
    }

    cv::bitwise_and(masked, masked, filtered, maskContours);
    cv::equalizeHist(filtered, dst);
}

inline void smoothSlices(const int startSlice, const int endSlice, std::vector<cv::Mat> & slices, cv::Mat & smoothed) {
    int cols = slices.at(startSlice).cols;
    int rows = slices.at(startSlice).rows;

    cv::Mat mergeMat(cv::Mat::zeros(rows, cols, CV_16UC1));
    cv::Mat conv16;

    for (int i = startSlice; i != endSlice; ++ i) {
        slices.at(i).convertTo(conv16, CV_16UC1, FROM_8_TO_16);
        mergeMat += conv16;
    }

    mergeMat /= (endSlice - startSlice);

    mergeMat.convertTo(smoothed, CV_8UC1, FROM_16_TO_8);
}

inline void mergeSlice(const int & startSlice, const int & endSlice, const int & sliceSize,
                       std::vector<cv::Mat> & slices, std::vector<cv::Mat> & smoothed,
                       uchar * mergeStartPoint) {
    int position = startSlice;

    smoothSlices(startSlice, endSlice, slices, smoothed[position]);
    memcpy(mergeStartPoint, smoothed[position].data, sliceSize);
}

inline void checkNeighbours(const int & position, const int & neighbourDiameter,
                            const int & minNeighbour, const int & maxNeighbour,
                            std::vector<cv::Mat> & filtered, std::vector<cv::Mat> & smoothed,
                            std::vector<bool> & gradientCalculated,
                            const int & sliceSize, uchar * mergeStartPoint, uchar * gradientStartPoint) {
    int leftestMergeNeighbour = std::max(minNeighbour, position - neighbourDiameter);
    int rightestMergeNeighbour = std::min(maxNeighbour - neighbourDiameter, position + 1);

    int rightCheckNeighbour;
    int currentCheckNeighbour;

    bool canMerge;

    //leftestMergeNeighbour = minNeighbour;
    //rightestMergeNeighbour = maxNeighbour - neighbourDiameter;

    int leftGradient;
    int rightGradient;

    for (int leftCheckNeighbour = leftestMergeNeighbour; leftCheckNeighbour != rightestMergeNeighbour; ++ leftCheckNeighbour) {

        leftGradient = std::max(minNeighbour, leftCheckNeighbour - 1);
        rightGradient = std::min((int) smoothed.size() - 1, leftCheckNeighbour + 1);

        if (!smoothed.at(leftCheckNeighbour).empty()) {

            if (!gradientCalculated.at(leftCheckNeighbour)
                    && !smoothed.at(leftGradient).empty() && !smoothed.at(rightGradient).empty()) {
                gradientCalculated[leftCheckNeighbour] = true;
                calcGradientSlice(leftCheckNeighbour, smoothed, gradientStartPoint + sliceSize * 3 * leftCheckNeighbour);
            }

            continue;
        }

        rightCheckNeighbour = leftCheckNeighbour + neighbourDiameter + 1;
        currentCheckNeighbour = leftCheckNeighbour;

        canMerge = true;

        while (canMerge && currentCheckNeighbour != rightCheckNeighbour) {
            canMerge &= !filtered[currentCheckNeighbour ++].empty();
        }

        if (canMerge) {
            mergeSlice(leftCheckNeighbour, rightCheckNeighbour, sliceSize, filtered, smoothed,
                       mergeStartPoint + sliceSize * leftCheckNeighbour);

            if (!gradientCalculated.at(leftCheckNeighbour) && !smoothed.at(leftGradient).empty() && !smoothed.at(rightGradient).empty()) {
                gradientCalculated[leftCheckNeighbour] = true;
                calcGradientSlice(leftCheckNeighbour, smoothed, gradientStartPoint + sliceSize * 3 * leftCheckNeighbour);
            }
        }
    }
}

typedef struct _FilterData {
    std::vector<cv::Mat *> * src;
    std::vector<cv::Mat *> * filtered;

    uchar ** mergeLocation;

    uchar ** gradientLocation;

    size_t * rowLenght;
    int * alignment;

    size_t * rowLenghtGradient;
    int * alignmentGradient;

    int neighbourRadius;

    int minValue;
    int maxValue;
} FilterData;

inline void processSlice(const int & i, std::vector<cv::Mat> & filtered, std::vector<cv::Mat> & smoothed,
                         std::vector<bool> & gradientCalculated,
                         const FilterData & filterData,
                         const int & sliceSize, const cv::Mat & dilateMat,
                         const cv::Size & gaussSize, const int & neighbourDiameter) {
    filterSlice(*(filterData.src->at(i)), filtered[i], filterData.minValue, filterData.maxValue, dilateMat, gaussSize);

    checkNeighbours(i, neighbourDiameter, 0, filtered.size(), filtered, smoothed, gradientCalculated,
                    sliceSize, *(filterData.mergeLocation), *(filterData.gradientLocation));

}

class SliceFilter : public cv::ParallelLoopBody {
protected:
    FilterData _filterData;

    cv::Mat _dilateMat;

    cv::Size _gaussSize;

    int _neighbourDiameter;

    mutable std::vector<cv::Mat>_filtered;
    mutable std::vector<cv::Mat>_smoothed;

    mutable std::vector<bool>_gradientCalculated;

    int _sliceSize;

    inline void fetchCvMats(const cv::Size & newSize, const int & newType = CV_8UC1) {
        int slicesTotalCount = _filterData.src->size();
        int slicesMergeCount = slicesTotalCount - _neighbourDiameter;

        _filtered.resize(slicesTotalCount);
        _smoothed.resize(slicesMergeCount);

        _gradientCalculated.resize(slicesMergeCount, false);

        cv::Mat dummyMat(cv::Mat::zeros(newSize, newType));
        cv::Mat dummyMatGrad(cv::Mat::zeros(newSize, CV_8UC3));

        _sliceSize = dummyMat.elemSize() * dummyMat.total();

        *(_filterData.mergeLocation) = new uchar[_sliceSize * slicesMergeCount];
        *(_filterData.gradientLocation) = new uchar[_sliceSize * slicesMergeCount * 3]; // gradient image has 3 channels

        *(_filterData.alignment) = (dummyMat.step & 3) ? 1 : 4;
        *(_filterData.rowLenght) = dummyMat.step1();

        *(_filterData.alignmentGradient) = (dummyMatGrad.step & 3) ? 1 : 4;
        *(_filterData.rowLenghtGradient) = dummyMatGrad.step1();
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
            processSlice(i, _filtered, _smoothed, _gradientCalculated,
                         _filterData, _sliceSize, _dilateMat, _gaussSize, _neighbourDiameter);
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

            /* see http://www.dabsoft.ch/dicom/3/C.11.2.1.2/*/
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

    cv::resize(dcmToMat, dcmToMat, scaledSize, 0, 0, cv::INTER_LINEAR);

    if (dicomData.type == CV_16UC1) {
        cv::Mat dcmToMat8(scaledSize, CV_8UC1);
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

        float ratioX = 1.0;
        float ratioY = 1.0;

        _dicomData.imageSpacing->at(0) *= (1 / ratioX);
        _dicomData.imageSpacing->at(1) *= (1 / ratioY);

        _scaledSize = cv::Size(_dicomData.width * ratioX, _dicomData.height * ratioY);

        _filterData.src->resize(_dicomData.depth);
        _filterData.filtered->resize(_dicomData.depth);

        fetchCvMats(_scaledSize);

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

    }

    virtual void operator ()(const cv::Range & r) const {
        for (int i = r.start; i != r.end; ++ i) {
            decodeSlice<T>(i, _filterData.src, _scaledSize, _dicomData);

            processSlice(i, _filtered, _smoothed, _gradientCalculated,
                         _filterData, _sliceSize,
                         _dilateMat, _gaussSize, _neighbourDiameter);

            _filterData.filtered->at(i) = new cv::Mat(_filtered.at(i));
        }
    }
};

#endif // CTPROCESSING_HPP
