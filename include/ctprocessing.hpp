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
#include <opencv2/ocl/ocl.hpp>

#define RADON_DEGREE_RANGE 180
#define PI_TIMES_2 (2 * CV_PI)
#define toRad(x) ((x) * CV_PI / 180.0)

static inline void radon(const cv::Mat & src, cv::Mat & dst, const std::vector<cv::Mat> & rotationMatrix,
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

static inline void fourier1D(const cv::Mat & src, cv::Mat dst, const std::vector<float> & dhtCoeffs) {
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

static inline cv::Mat fourier1Dto2D(const cv::Mat & src, cv::Mat & dst,
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

static inline cv::Mat backproject(const cv::Mat & src, cv::Mat & dst,
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

void inline filterOneSlice(const cv::Mat & src, cv::Mat & dst, const int & minVal, const int & maxVal) {
    cv::Mat maskRange(cv::Mat(cv::Mat::zeros(src.cols, src.rows, CV_8UC1)));
    cv::Mat maskedRange(cv::Mat(cv::Mat::zeros(src.cols, src.rows, CV_8UC1)));

    cv::inRange(src, cv::Scalar(minVal), cv::Scalar(maxVal), maskRange);

    cv::bitwise_and(src, src, maskedRange, maskRange);

    cv::adaptiveThreshold(maskedRange, maskedRange, 255, cv::ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY_INV, 3, 5);
    cv::dilate(maskedRange, maskedRange, cv::Mat(3, 3, CV_8UC1));
    cv::GaussianBlur(maskedRange, maskedRange, cv::Size(3, 3), 0.5);

    cv::Mat maskContours(cv::Mat(cv::Mat::zeros(src.cols, src.rows, CV_8UC1)));

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

typedef struct _CtData {
    std::vector<cv::Mat *> * noisy;
    std::vector<cv::Mat *> * filtered;

    std::vector<float> * imageSpacing;

    int bytesAllocated;
    int width;
    int height;
    int offset;
    int type;

    int64_t minValue;
    int64_t maxValue;
    int64_t windowCenter;
    int64_t windowWidth;

    bool isLittleEndian;
    bool inverseNeeded;
    bool isRadonNeeded;

    float slope;
    float intercept;

    char * buffer;
}CtData;

template <typename T>
class CtProcessing : public cv::ParallelLoopBody {
private:
    CtData _ctData;

    int width;
    int height;

    int pad;

    int widthPad;
    int heightPad;

    int wPad;
    int hPad;

    std::vector<cv::Mat>rotationMatrix;

    std::vector<float>cosTable;
    std::vector<float>sinTable;

    std::vector<float>dhtCoeffs;

public:
    CtProcessing(CtData & ctData) : _ctData(ctData) {
        _ctData.offset = _ctData.bytesAllocated * _ctData.width * _ctData.height;

        _ctData.imageSpacing->at(0) *= (float) 2.0;
        _ctData.imageSpacing->at(1) *= (float) 2.0;

        width = _ctData.width / 2;
        height = _ctData.height / 2;

        if (_ctData.isRadonNeeded) {

            //useful image is ellipsoid with ra rb as width and height,
            //all black near corners - we loss this "garbage" during rotations - good riddance
            pad = std::max(width, height);

            widthPad = std::ceil((pad - width) / 2.0);
            heightPad = std::ceil((pad - height) / 2.0);

            wPad = width + widthPad;
            hPad = height + heightPad;

            float twoPiN = PI_TIMES_2 / wPad;

            for (int angle = 0; angle < RADON_DEGREE_RANGE; ++ angle) {
                rotationMatrix.push_back(cv::getRotationMatrix2D(cv::Point2i((width + widthPad) / 2, (height + heightPad) / 2),
                                                             angle, 1.0));
                cosTable.push_back(std::cos(toRad(angle)));
                sinTable.push_back(std::sin(toRad(angle)));

            }

            for (int i = 0; i != wPad; ++ i) {
                dhtCoeffs.push_back(std::cos(twoPiN * i) + std::sin(twoPiN * i));
            }
        }
    }

    virtual void operator ()(const cv::Range & r) const {

        for (register int i = r.start; i != r.end; ++ i) {

            cv::Mat * data = new cv::Mat(_ctData.width, _ctData.height, _ctData.type);
            T pixel;

            char * bufferImageI = _ctData.buffer + _ctData.offset * i;

            for (int y = 0; y < _ctData.height; y ++) {
                for (int x = 0; x < _ctData.width; x ++) {
                    pixel = 0;

                    if (_ctData.isLittleEndian) {
                        for (int k = 0; k < _ctData.bytesAllocated; k ++) {
                            pixel |= (T)*(bufferImageI + k) << (8 * k);
                        }
                    }
                    else {
                        for (int k = _ctData.bytesAllocated - 1; k > 0 ; k --) {
                            pixel |= (T)*(bufferImageI + k) << (8 * (_ctData.bytesAllocated - k + 1));
                        }
                    }

                    bufferImageI += _ctData.bytesAllocated;

                    /* similar as http://code.google.com/p/pydicom/source/browse/source/dicom/contrib/pydicom_PIL.py*/
                    pixel = _ctData.slope * pixel + _ctData.intercept;

                    if (pixel <= (_ctData.windowCenter - 0.5 - (_ctData.windowWidth - 1) / 2.0)) {
                        pixel = (T)_ctData.minValue;
                    }
                    else if (pixel > (_ctData.windowCenter - 0.5 + (_ctData.windowWidth - 1) / 2.0)) {
                        pixel = (T)_ctData.maxValue;
                    }
                    else {
                        pixel = ((pixel - _ctData.windowCenter + 0.5) / (_ctData.windowWidth - 1) + 0.5) *
                                (_ctData.maxValue - _ctData.minValue);
                    }

                    //MONOCHROME2 - high value -> brighter, -1 high -> blacker
                    if (_ctData.inverseNeeded) {
                        data->at<T>(y, x) = ~pixel;
                    }
                    else {
                        data->at<T>(y, x) = pixel;
                    }
                }
            }

            cv::resize(*data, *data, cv::Size(width, height));
            (*data).convertTo(*data, CV_8UC1, 1 / 256.0);

            cv::Mat * filtered = new cv::Mat(cv::Mat::zeros(data->cols, data->rows, CV_8UC1));

            filterOneSlice(*data, *filtered, 40, 55);

            _ctData.noisy->at(i) = data;
            _ctData.filtered->at(i) = filtered;
        }
    }
};

typedef struct _FilterData {
    std::vector<cv::Mat *> src;
    std::vector<cv::Mat *> dst;

    int minVal;
    int maxVal;
} FilterData;

class SliceFilter : public cv::ParallelLoopBody {
private:
    FilterData _filterData;

public:
    SliceFilter(FilterData & filterData) :
        _filterData(filterData) {

    }

    virtual void operator ()(const cv::Range & r) const {
        for (register int i = r.start; i != r.end; ++ i) {
            filterOneSlice(*(_filterData.src[i]), *(_filterData.dst[i]), _filterData.minVal, _filterData.maxVal);
        }
    }
};

typedef struct _SmoothData {
    std::vector<cv::Mat*> * data;
    std::vector<cv::Mat*> * smoothData;

    size_t neighbourRadius;
}SmoothData;

class VolumeSmoothing : public cv::ParallelLoopBody {
private:
    SmoothData _smoothData;

    size_t _cols;
    size_t _rows;

    size_t _neighbourCount;

public:
    VolumeSmoothing(SmoothData & smoothData) :
        _smoothData(smoothData),
        _cols(smoothData.data->at(0)->cols),
        _rows(smoothData.data->at(0)->rows) {

        _neighbourCount = 2 * _smoothData.neighbourRadius + 1;
    }

    virtual void operator ()(const cv::Range & r) const {
        for (register int i = r.start; i != r.end; ++ i) {
            cv::Mat mergeMat(cv::Mat::zeros(_rows, _cols, CV_16UC1));
            cv::Mat mergeMat8(_cols, _rows, CV_8UC1);
            cv::Mat conv16(_cols, _rows, CV_16UC1);

            for (size_t j = i + _smoothData.neighbourRadius; j != i + _neighbourCount; ++ j) {
                _smoothData.data->at(j)->convertTo(conv16, CV_16UC1, 256.0);
                mergeMat += conv16;
            }

            mergeMat /= _neighbourCount;

            mergeMat.convertTo(mergeMat8, CV_8UC1, 1 / 256.0);
            _smoothData.smoothData->at(i) = new cv::Mat(mergeMat8);
        }
    }
};

#endif // CTPROCESSING_HPP
