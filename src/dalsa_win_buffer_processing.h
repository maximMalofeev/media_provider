#ifndef SAPBUFFERPROCESSING_H
#define SAPBUFFERPROCESSING_H

#include <SapClassBasic.h>
#include <QImage>
#include <QObject>

namespace MediaProvider {

class SapBufferProcessing : public QObject, public SapProcessing {
  Q_OBJECT

 public:
  SapBufferProcessing(SapBuffer* sapBuffer, QObject* parent = nullptr);

 signals:
  void newFrame(QImage frame, qlonglong timestamp);

 protected:
  BOOL Run() override;

 private:
  int frameSize{};
};

}  // namespace MediaProvider

#endif  // SAPBUFFERPROCESSING_H
