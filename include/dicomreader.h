#ifndef DICOMREADER_H
#define DICOMREADER_H

#include <QtCore/QObject>

#include <gdcmImage.h>
#include <gdcmFile.h>

#include "ctprocessing.hpp"
#include "sliceinfo.h"

class DicomReader : public QObject {
  Q_OBJECT
public:
  explicit DicomReader(QObject * parent = 0);
  ~DicomReader();

  void reset(const int & newSize = 0);
  void resetV(std::vector<cv::Mat*> & vec, const int & newSize = 0);

private:
  size_t _sliceNumber;

  std::vector<cv::Mat *> _noisy;
  std::vector<cv::Mat *> _filtered;

  DicomData _dicomData;

  void showSliceWithNumber(const size_t & sliceNumber);
  void readImage(gdcm::File &dFile, const gdcm::Image & dImage);

  void fetchDicomData(DicomData & dicomData, gdcm::File & dFile, const gdcm::Image & dImage);
  void runSliceProcessing(const bool & tellAboutHURange = false);
  void updateDicomData();
signals:
  void slicesProcessed(SliceInfo::SliceSettings & sliceSettings);

public slots:
  void readFile(const QUrl & fileUrl);
  void changeSliceNumber(const int & ds);

  void updateMinHU(const int & minHU);
  void updateMaxHU(const int & maxHU);
};

#endif // DICOMREADER_H
