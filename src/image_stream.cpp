#include "image_stream.h"

namespace MediaProvider {

struct ImageStream::Implementation {
  QImage *image = {};
  State state = State::Stopped;
};

ImageStream::ImageStream(QImage *image, QObject *parent) : Stream(parent) {
  impl_.reset(new Implementation{image});
}

ImageStream::~ImageStream() = default;

Stream::State MediaProvider::ImageStream::state() const { return impl_->state; }

void ImageStream::start() {
  impl_->state = Playing;
  emit stateChanged();

  emit newFrame(*impl_->image, 0);

  impl_->state = Stopped;
  emit stateChanged();
}

void ImageStream::stop() {}

}  // namespace MediaProvider
