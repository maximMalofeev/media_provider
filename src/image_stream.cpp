#include "image_stream.h"

namespace MediaProvider {

struct ImageStream::Implementation {
  QImage *image = {};
};

ImageStream::ImageStream(QImage *image, Resource *parent) : Stream(parent) {
  impl_.reset(new Implementation{image});
}

ImageStream::~ImageStream() = default;

void ImageStream::start() {
  if(impl_->image->isNull()){
    setState(Invalid);
    setErrorString("Null image received");
  }
  setState(Playing);
  emit newFrame(*impl_->image, 0);
  setState(Stopped);
}

void ImageStream::stop() {}

}  // namespace MediaProvider
