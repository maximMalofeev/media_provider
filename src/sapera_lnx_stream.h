#ifndef SAPERASTREAM_H
#define SAPERASTREAM_H

#include <media_provider/media_provider.h>

namespace MediaProvider {

class SaperaStream : public Stream {
  Q_OBJECT

 public:
  SaperaStream(Resource* parent);
  ~SaperaStream();

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
