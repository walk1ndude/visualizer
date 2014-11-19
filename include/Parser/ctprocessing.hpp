#ifndef CTPROCESSING_HPP
#define CTPROCESSING_HPP

#include "Parser/Helpers.hpp"

#include "Info/VolumeInfo.h"
#include "Info/TextureInfo.h"

namespace Parser {
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

    inline void mergeSlice(const int & startSlice, const int & endSlice, const size_t & sliceSize,
                           std::vector<cv::Mat> & slices, uchar * mergeStartPoint) {
        cv::Mat smoothed;

        smoothSlices(startSlice, endSlice, slices, smoothed);
/*
        if (startSlice < slices.size() / 2) {
            cv::circle(smoothed, cv::Point(smoothed.cols / 2, smoothed.rows / 2), 5, 65535, CV_FILLED);
        }

        if (startSlice > slices.size() / 2 - 2 && startSlice < slices.size() / 2 + 2) {
            cv::line(smoothed, cv::Point(40, smoothed.rows / 2), cv::Point(smoothed.cols / 2, smoothed.rows / 2), 64300, 3);
            cv::line(smoothed, cv::Point(smoothed.cols / 2, 40), cv::Point(smoothed.cols / 2, smoothed.rows / 2), 64000, 3);
        }
*/
        memcpy(mergeStartPoint, smoothed.data, sliceSize);
    }

    static cv::Mutex sliceMutex;

    inline void checkNeighbours(const int & position, const int & neighbourDiameter,
                                const int & minNeighbour, const int & maxNeighbour,
                                std::vector<cv::Mat> & filtered, std::vector<bool> & hasMerged,
                                const size_t & sliceSize, uchar * mergeStartPoint) {
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

            sliceMutex.lock();

            canMerge = true;

            while (canMerge && currentCheckNeighbour != rightCheckNeighbour) {
                canMerge &= !filtered[currentCheckNeighbour ++].empty();
            }

            if (canMerge) {
                hasMerged[leftCheckNeighbour] = true;
                sliceMutex.unlock();

                mergeSlice(leftCheckNeighbour, rightCheckNeighbour, sliceSize, filtered, mergeStartPoint + sliceSize * leftCheckNeighbour);
            }
            else {
                sliceMutex.unlock();
            }
        }
    }

    inline void processSlice(const int & i, std::vector<cv::Mat> & noisy, std::vector<cv::Mat> & filtered,
                             std::vector<bool> & hasMerged,
                             const size_t & sliceSize,
                             const cv::Mat & dilateMat, const cv::Size & gaussSize,
                             const int & neighbourDiameter, uchar * mergeLocation) {
        //filterSlice(noisy[i], filtered[i], dilateMat, gaussSize);

        checkNeighbours(i, neighbourDiameter, 0, (int) filtered.size(), noisy, hasMerged, sliceSize, mergeLocation);
    }

    class DicomData {
    public:
        QVector3D imageSpacings;

        std::vector<char> vbuffer;

        int bytesAllocated;

        size_t width;
        size_t height;
        size_t depth;

        size_t sliceSize;

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

        TextureInfo::MergedDataPtr * mergeLocation;

        QOpenGLPixelTransferOptions * pixelTransferOptions;

        int neighbourRadius;
    };

    inline void decodeSlice(const int & position, std::vector<cv::Mat> & dst, const DicomData * dicomData) {
        cv::Mat dcmToMat(cv::Mat::zeros((int) dicomData->width, (int) dicomData->height, dicomData->type));

        quint16 pixelU;
        qint32 pixel;

        char * posInBuffer = dicomData->buffer + dicomData->sliceSize * position;

        for (int y = 0; y != (int) dicomData->height; ++ y) {
            for (int x = 0; x != (int) dicomData->width; ++ x) {
                pixel = 0;
                pixelU = 0;

                if (dicomData->isLittleEndian) {
                    for (int k = 0; k < dicomData->bytesAllocated; ++ k) {
                        pixelU |= (quint16)*(posInBuffer + k) << (8 * k);
                    }
                }
                else {
                    for (int k = dicomData->bytesAllocated - 1; k > 0 ; -- k) {
                        pixelU |= (quint16)*posInBuffer << (8 * (dicomData->bytesAllocated - k + 1));
                    }
                }

                posInBuffer += dicomData->bytesAllocated;

                /* see http://www.dabsoft.ch/dicom/3/C.11.2.1.2/*/ /*
                pixel = dicomData->slope * pixelU + dicomData->intercept;

                if (pixel < dicomData->minHU || pixel > dicomData->maxHU) {
                    dcmToMat.at<quint16>(y, x) = 0;
                    continue;
                }

                if (pixel <= (dicomData->windowCenter - 0.5 - (dicomData->windowWidth - 1) / 2.0)) {
                    pixelU = (quint16)dicomData->minValue;
                }
                else if (pixel > (dicomData->windowCenter - 0.5 + (dicomData->windowWidth - 1) / 2.0)) {
                    pixelU = (quint16)dicomData->maxValue;
                }
                else {
                    pixelU = ((pixel - dicomData->windowCenter + 0.5) / (dicomData->windowWidth - 1) + 0.5) *
                            (dicomData->maxValue - dicomData->minValue);
                }
*/

                dcmToMat.at<quint16>(y, x) = pixelU;
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

        size_t _sliceSize;

        inline void fetchCvMats(const cv::Size & newSize, const int & depth, const int & newType = CV_16UC1) {
            int slicesTotalCount = depth;
            int slicesMergeCount = slicesTotalCount - _neighbourDiameter;

            _noisy.resize(slicesTotalCount);
            _filtered.resize(slicesTotalCount);

            _dicomData->noisy->resize(slicesTotalCount);
            _dicomData->filtered->resize(slicesTotalCount);

            _hasMerged.resize(slicesMergeCount, false);

            cv::Mat dummyMat(cv::Mat::zeros(newSize, newType));

            _sliceSize = (size_t) (dummyMat.elemSize() * dummyMat.total());

            *(_dicomData->mergeLocation) = new TextureInfo::MergedData[_sliceSize * slicesMergeCount];

            _dicomData->pixelTransferOptions->setAlignment((dummyMat.step & 3) ? 1 : 4);
            _dicomData->pixelTransferOptions->setRowLength((int) dummyMat.step1());
        }

    public:
        SliceProcessing(DicomData * dicomData) :
            _dicomData(dicomData) {

            _dicomData->sliceSize = _dicomData->bytesAllocated * _dicomData->width * _dicomData->height;

            _neighbourDiameter = 2 * _dicomData->neighbourRadius;

            _dilateMat = cv::Mat(3, 3, CV_8UC1);
            _gaussSize = cv::Size(3, 3);

            fetchCvMats(cv::Size((int) _dicomData->width, (int) _dicomData->height), (int) _dicomData->depth);
        }

        virtual void operator ()(const cv::Range & r) const {
            for (int i = r.start; i != r.end; ++ i) {
                decodeSlice(i, _noisy, _dicomData);
                processSlice(i, _noisy, _filtered, _hasMerged, _dicomData->sliceSize,
                             _dilateMat, _gaussSize, _neighbourDiameter, *(_dicomData->mergeLocation));

                //_dicomData->noisy->at(i) = new cv::Mat(_noisy.at(i));
                //_dicomData->filtered->at(i) = new cv::Mat(_filtered.at(i));
            }
        }
    };
}
#endif // CTPROCESSING_HPP
