#ifndef SAPBUFFERPROCESSING_H
#define SAPBUFFERPROCESSING_H

#include <QImage>
#include <QThread>

namespace MediaProvider {

class DalsaBufferProcessing : public QThread {
  Q_OBJECT

 public:
  DalsaBufferProcessing(void* gevCameraHandle, QObject* parent);
  ~DalsaBufferProcessing();

  bool isInitialised() const;
  void stop();

 signals:
  void error(QString errorStr);
  void newFrame(QImage frame, qlonglong timestamp);

 protected:
  void run();

 private:
  struct Implementation;
  QScopedPointer<Implementation> impl_;
};

}  // namespace MediaProvider

#endif  // SAPBUFFERPROCESSING_H
