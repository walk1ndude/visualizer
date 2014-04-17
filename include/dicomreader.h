#ifndef DICOMREADER_H
#define DICOMREADER_H

#include <QtCore/QObject>
#include <QtCore/QMutex>

#include <gdcmImage.h>
#include <gdcmFile.h>

#include <opencv2/highgui/highgui.hpp>

#include "ctprocessing.hpp"

#define OPENCL_ALL_OK 0
#define OPENCL_NOT_INITIALIZED 1

#define CANNY_LOWER 200

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
  std::vector<cv::Mat *> _smoothed;

  cv::ocl::Context * _context;

  //QMutex _mutex;

  int _minValue;
  int _maxValue;

  int initOpenCL();

  void showImageWithNumber(const size_t & imageNumber);
  void readImage(gdcm::File & dFile, const gdcm::Image & dImage);
  void mergeMatData(const std::vector<cv::Mat*> & src, const std::vector<float> & imageSpacings = std::vector<float>());

  void medianSmooth(const size_t & neighbourRadius);

  void updateFiltered();
signals:
  void slicesProcessed(uchar * mergedData, const std::vector<float> & scaling,
                       const std::vector<size_t> & size, const int & alignment, const size_t & rowLength);

public slots:
  void readFile(QString dicomFile);
  void changeSliceNumber(int ds);

  void updateMinValue(int minValue);
  void updateMaxValue(int maxValue);
};

#endif // DICOMREADER_H
