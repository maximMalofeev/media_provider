#ifndef IMAGESTREAM_H
#define IMAGESTREAM_H

#include <media_provider/media_provider.h>

namespace MediaProvider {

class ImageStream : public Stream {
 public:
  ImageStream(QImage* image, QObject* parent = nullptr);
  ~ImageStream();

 public slots:
  void start() override;
  void stop() override;

 private:
  struct Implementation;
  QScopedPointer<Implementation> impl_;
};
}  // namespace MediaProvider

#endif  // IMAGESTREAM_H
