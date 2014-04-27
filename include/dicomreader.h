#ifndef DICOMREADER_H
#define DICOMREADER_H

#include <QtCore/QObject>
#include <QtCore/QSharedPointer>

#include <gdcmImage.h>
#include <gdcmFile.h>

#include "ctprocessing.hpp"

class DicomReader : public QObject {
  Q_OBJECT
public:
  explicit DicomReader(QObject * parent = 0);
  ~DicomReader();

  void reset(const int & newSize = 0);
  void resetV(std::vector<cv::Mat*> & vec, const int & newSize = 0);

private:
  size_t _imageNumber;

  std::vector<cv::Mat *> _noisy;
  std::vector<cv::Mat *> _filtered;

  int _minValue;
  int _maxValue;

  void showImageWithNumber(const size_t & imageNumber);
  void readImage(gdcm::File & dFile, const gdcm::Image & dImage);

  void updateFiltered();
signals:
  void slicesProcessed(QSharedPointer<uchar> mergedData, QSharedPointer<uchar>  gradientData,
                       const std::vector<float> & scaling = std::vector<float>(),
                       const std::vector<size_t> & size = std::vector<size_t>(),
                       const int & alignment = 0, const size_t & rowLength = 0,
                       const int & alignmentGradient = 0, const size_t & rowLengthGradient = 0);

public slots:
  void readFile(QString dicomFile);
  void changeSliceNumber(int ds);

  void updateMinValue(int minValue);
  void updateMaxValue(int maxValue);
};

#endif // DICOMREADER_H
