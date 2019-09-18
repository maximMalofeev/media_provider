#include "image_resource.h"
#include <QImage>
#include <QtConcurrent/QtConcurrent>
#include "image_stream.h"

namespace MediaProvider {

struct ImageResource::Implementation {
  QString resource;
  QFutureWatcher<bool> imageWatcher;
  QImage image;
  Resource::State state = Resource::Initialising;
  ImageStream* stream = {};
};

ImageResource::ImageResource(const QString& resource, QObject* parent)
    : Resource(parent) {
  impl_.reset(new Implementation);

  connect(&impl_->imageWatcher, &QFutureWatcher<QImage>::finished, [this]() {
    if (impl_->imageWatcher.result()) {
      impl_->state = Initialised;
    } else {
      impl_->state = Invalid;
    }
    emit stateChanged();
    emit sizeChanged();
    emit colorFormatChanged();
    emit availableSizesChanged();
    emit availableColorFormatsChanged();
  });

  impl_->resource = resource;
  impl_->stream = new ImageStream(&impl_->image, this);
  auto future = QtConcurrent::run([this]() {
    if (impl_->image.load(impl_->resource)) {
      return true;
    }
    return false;
  });
  impl_->imageWatcher.setFuture(future);
}

ImageResource::~ImageResource() {}

Resource::State ImageResource::state() const { return impl_->state; }

QString ImageResource::resource() const { return impl_->resource; }

QSize ImageResource::size() const {
  if (impl_->state != Initialised) {
    return {};
  }
  return impl_->image.size();
}

bool ImageResource::setSize(const QSize& size) {
  if (impl_->state != Initialised) {
    return false;
  }
  if (impl_->image.size() == size) {
    return true;
  } else {
    return false;
  }
}

QList<QVariant> ImageResource::availableSizes() const {
  if (impl_->state != Initialised) {
    return {};
  }
  return {impl_->image.size()};
}

QList<QVariant> ImageResource::availableColorFormats() const {
  if (impl_->state != Initialised) {
    return {};
  }
  return {QVariant::fromValue(impl_->image.format())};
}

QImage::Format ImageResource::colorFormat() const {
  if (impl_->state != Initialised) {
    return {};
  }
  return impl_->image.format();
}

bool ImageResource::setColorFormat(const QImage::Format format) {
  if (impl_->state != Initialised) {
    return false;
  }
  if (impl_->image.format() == format) {
    return true;
  } else {
    return false;
  }
}

Stream* ImageResource::stream() { return impl_->stream; }

}  // namespace MediaProvider
