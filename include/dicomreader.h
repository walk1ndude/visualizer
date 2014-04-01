#ifndef DICOMREADER_H
#define DICOMREADER_H

#include <QtCore/QObject>

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

  void reset(Images & images, const int & newSize = 0);
  void resetV(std::vector<cv::Mat*> & vec, const int & newSize = 0);

private:
  size_t _imageNumber;

  std::vector<float>_imageSpacings;

  Images _images;

  cv::ocl::Context * _context;

  int initOpenCL();

  void showImageWithNumber(const size_t & imageNumber);
  void readImage(gdcm::File & dFile, const gdcm::Image & dImage);

signals:
  void slicesProcessed(const std::vector<cv::Mat *> & ctImages, const std::vector<float> & imageSpacings);

public slots:
  void readFile(QString dicomFile);
  void changeSliceNumber(int ds);
};

#endif // DICOMREADER_H
