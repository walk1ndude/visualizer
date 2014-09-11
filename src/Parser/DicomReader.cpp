#include <gdcmReader.h>
#include <gdcmImageReader.h>
#include <gdcmAttribute.h>
#include <gdcmStringFilter.h>
#include <gdcmException.h>

#include <QtCore/QUrl>

#include <opencv2/highgui/highgui.hpp>

#include "Parser/DicomReader.h"
#include "Parser/Helpers.hpp"

#define WINDOW_NOISY "noisy"

#define MIN_HU 700
#define MAX_HU 3600

namespace Parser {
    DicomReader::DicomReader() :
        _sliceNumber(0) {

    }

    DicomReader::~DicomReader() {
        reset();
    }

    void DicomReader::resetV(std::vector<cv::Mat *> & vec, const int & newSize) {
        for (std::vector<cv::Mat *>::iterator it = vec.begin(); it != vec.end(); ++ it) {
            delete *it;
        }
        vec.clear();
        vec.resize(newSize);
    }

    void DicomReader::reset(const int & newSize) {
        resetV(_noisy, newSize);
        resetV(_filtered, newSize);
    }

    void DicomReader::fetchDicomData(DicomData & dicomData, gdcm::File & dFile, const gdcm::Image & dImage) {
        gdcm::StringFilter dStringFilter;
        dStringFilter.SetFile(dFile);

        gdcm::DataSet & dDataSet = dFile.GetDataSet();

        dicomData.imageSpacings.setX(dImage.GetSpacing(0));
        dicomData.imageSpacings.setY(dImage.GetSpacing(1));

        gdcm::Tag tagFind(0x0018, 0x0050);
        if (dDataSet.FindDataElement(tagFind)) {
            dicomData.imageSpacings.setZ(std::stof(dStringFilter.ToString(tagFind)));
        }
        else {
            //pure assumtion for now
            dicomData.imageSpacings.setZ(1.0);
        }

        tagFind.SetElementTag(0x0018, 0x0088);
        if (dDataSet.FindDataElement(tagFind)) {
            dicomData.imageSpacings[2] += std::stof(dStringFilter.ToString(tagFind));
        }
        else {
            //pure assumtion for now
            dicomData.imageSpacings[2] += 0.1;
        }

        tagFind.SetElementTag(0x0028, 0x1050);
        if (dDataSet.FindDataElement(tagFind)) {
            dicomData.windowCenter = std::stoi(dStringFilter.ToString(tagFind));
        }
        else {
            dicomData.windowCenter = 0;
        }

        tagFind.SetElementTag(0x0028, 0x1051);
        if (dDataSet.FindDataElement(tagFind)) {
            dicomData.windowWidth = std::stoi(dStringFilter.ToString(tagFind));
        }
        else {
            dicomData.windowWidth = 0;
        }

        dicomData.width = dImage.GetDimension(0);
        dicomData.height = dImage.GetDimension(1);
        dicomData.depth = dImage.GetDimension(2);

        dicomData.vbuffer.resize(dImage.GetBufferLength());
        dicomData.buffer = &(dicomData.vbuffer[0]);
        dImage.GetBuffer(dicomData.buffer);

        //MONOCHROME2

        gdcm::PhotometricInterpretation photometricInterpretation = dImage.GetPhotometricInterpretation();

        dicomData.inverseNeeded = (photometricInterpretation != gdcm::PhotometricInterpretation::MONOCHROME2);

        gdcm::PixelFormat pixelFormat = dImage.GetPixelFormat();
        if (pixelFormat.GetScalarType() == gdcm::PixelFormat::UINT16) {
            dicomData.type = CV_16UC1;
        }
        else {
            dicomData.type = CV_8UC1; // for now
        }

        dicomData.bytesAllocated = pixelFormat.GetBitsAllocated() / 8;

        try {
            dicomData.slope = dImage.GetSlope();
            dicomData.intercept = dImage.GetIntercept();
        }
        catch (gdcm::Exception &) {
            dicomData.slope = 1.0f;
            dicomData.intercept = 0.0f;
        }

        dicomData.minValue = (int) pixelFormat.GetMin();
        dicomData.maxValue = (int) pixelFormat.GetMax();

        dicomData.minHUPossible = dicomData.slope * dicomData.minValue + dicomData.intercept;
        dicomData.maxHUPossible = dicomData.slope * dicomData.maxValue + dicomData.intercept;

        dicomData.isLittleEndian = (pixelFormat.GetBitsAllocated() - pixelFormat.GetHighBit() == 1);

        dicomData.neighbourRadius = 0;

        dicomData.noisy = &_noisy;
        dicomData.filtered = &_filtered;

        dicomData.minHU = std::max(dicomData.minHUPossible, MIN_HU);
        dicomData.maxHU = std::min(dicomData.maxHUPossible, MAX_HU);
    }

    void DicomReader::readImage(gdcm::File & dFile, const gdcm::Image & dImage) {

        fetchDicomData(_dicomData, dFile, dImage);

        //cv::namedWindow(WINDOW_NOISY, CV_WINDOW_AUTOSIZE);

        runSliceProcessing(true);
    }

    void DicomReader::runSliceProcessing(const bool & tellAboutHURange) {
        TextureInfo::MergedDataPtr mergedData = nullptr;

        QOpenGLPixelTransferOptions pixelTransferOptions;

        _dicomData.pixelTransferOptions = &pixelTransferOptions;
        _dicomData.mergeLocation = &mergedData;

        float startTime = cv::getTickCount() / cv::getTickFrequency();

        cv::parallel_for_(cv::Range(0, (int) _dicomData.depth), SliceProcessing(&_dicomData));

        qDebug() << "Elapsed Time: " << cv::getTickCount() / cv::getTickFrequency() - startTime;

        size_t depth = _dicomData.depth - _dicomData.neighbourRadius * 2;

        SliceInfo::Slices slices;

        slices.texture.mergedData = TextureInfo::MergedDataPointer(mergedData);
        slices.texture.scaling = scaleVector<float, QVector3D>(
                _dicomData.width * _dicomData.imageSpacings.x(),
                _dicomData.height * _dicomData.imageSpacings.y(),
                depth * _dicomData.imageSpacings.z()
                ) *
                QVector3D(
                    1.0f / _dicomData.imageSpacings.x(),
                    1.0f / _dicomData.imageSpacings.y(),
                    1.0f / _dicomData.imageSpacings.z()
                    );

        slices.imageSpacings = ModelInfo::ImageSpacings(_dicomData.width * _dicomData.imageSpacings.x() / (0.5f * slices.texture.scaling.x()),
                                                        _dicomData.height * _dicomData.imageSpacings.y() / (0.5f * slices.texture.scaling.y()),
                                                        depth * _dicomData.imageSpacings.z() / (0.5f * slices.texture.scaling.z())
                                                        );

        slices.texture.size.setX(_dicomData.width);
        slices.texture.size.setY(_dicomData.height);
        slices.texture.size.setZ(depth);

        slices.texture.pixelTransferOptions = pixelTransferOptions;

        slices.huRange.setX(_dicomData.minHUPossible);
        slices.huRange.setY(_dicomData.maxHUPossible);

        slices.texture.pixelType = QOpenGLTexture::UInt16;
        slices.texture.textureFormat = QOpenGLTexture::R16_UNorm;
        slices.texture.pixelFormat = QOpenGLTexture::Red;
        slices.texture.target = QOpenGLTexture::Target3D;

        emit slicesProcessed(QVariant::fromValue(slices));
    }

    QUrl DicomReader::dicomFile() const {
        return _dicomFile;
    }

    void DicomReader::setDicomFile(const QUrl & dicomFile) {
        if (dicomFile.isEmpty()) {
            return;
        }

        gdcm::ImageReader dIReader;

        dIReader.SetFileName(dicomFile.toLocalFile().toStdString().c_str());

        if (dIReader.Read()) {
            readImage(dIReader.GetFile(), dIReader.GetImage());
            _dicomFile = dicomFile;
        }
        else {
            qDebug() << "can't read file";
        }

        emit dicomFileChanged();
    }

    void DicomReader::nextSlice(const int & ds) {
        if (_noisy.size()) {
            _sliceNumber += ds;
            _sliceNumber %= _noisy.size();
            showSliceWithNumber(_sliceNumber);
        }
    }

    void DicomReader::showSliceWithNumber(const size_t & sliceNumber) {
        cv::imshow(WINDOW_NOISY, *(_noisy.at(sliceNumber)));
        cv::waitKey(1);
    }

    void DicomReader::updateMinHU(const int & minHU) {
        _dicomData.minHU = std::min(minHU, _dicomData.maxHU);
        _dicomData.maxHU = std::max(minHU, _dicomData.maxHU);

        resetV(_noisy);
        resetV(_filtered);

        runSliceProcessing();
    }

    void DicomReader::updateMaxHU(const int & maxHU) {
        _dicomData.minHU = std::max(maxHU, _dicomData.minHU);
        _dicomData.maxHU = std::min(maxHU, _dicomData.minHU);

        resetV(_noisy);
        resetV(_filtered);

        runSliceProcessing();
    }
}
