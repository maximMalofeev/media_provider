#ifndef SAPERASTREAM_H
#define SAPERASTREAM_H

#include <media_provider/media_provider.h>

namespace MediaProvider {

class DalsaStream : public Stream {
  Q_OBJECT

 public:
  DalsaStream(Resource* parent);
  ~DalsaStream();

  void setCameraHandle(void* handle);

 public slots:
  void start();
  void stop();
  void onBufferProcessingError(QString errorStr);

 private:
  struct Implementation;
  QScopedPointer<Implementation> impl_;
};

}  // namespace MediaProvider

#endif  // SAPERASTREAM_H
