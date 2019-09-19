#include "image_stream.h"

namespace MediaProvider {

struct ImageStream::Implementation {
  QImage *image = {};
};

ImageStream::ImageStream(QImage *image, QObject *parent) : Stream(parent) {
  impl_.reset(new Implementation{image});
  if(image->isNull()){
    setState(Invalid);
    setErrorString("Null image received");
  }
}

ImageStream::~ImageStream() = default;

void ImageStream::start() {
  if(state() == Invalid){
    return;
  }
  setState(Playing);
  emit newFrame(*impl_->image, 0);
  setState(Stopped);
}

void ImageStream::stop() {}

}  // namespace MediaProvider
