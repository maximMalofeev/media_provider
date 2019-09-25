#ifndef SAPBUFFERPROCESSING_H
#define SAPBUFFERPROCESSING_H

#include <QImage>
#include <QThread>

namespace MediaProvider {

class SapBufferProcessing : public QThread {
  Q_OBJECT

 public:
  SapBufferProcessing(void* gevCameraHandle, QObject* parent);
  ~SapBufferProcessing();

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
