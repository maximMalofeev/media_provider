#include "image_resource.h"
#include <QImage>
#include <QtConcurrent/QtConcurrent>
#include "image_stream.h"

namespace MediaProvider {

struct ImageResource::Implementation {
  QFutureWatcher<bool> imageWatcher;
  QImage image;
  ImageStream* stream = {};
};

ImageResource::ImageResource(const QString& res, QObject* parent)
    : Resource(parent) {
  impl_.reset(new Implementation);
  QFileInfo fi(res);
  setResource(fi.fileName());

  connect(&impl_->imageWatcher, &QFutureWatcher<QImage>::finished, [this]() {
    if (impl_->imageWatcher.result()) {
      setState(Initialised);
    } else {
      setState(Invalid);
      setErrorString("Unable to load " + resource());
    }
    emit sizeChanged();
    emit colorFormatChanged();
    emit availableSizesChanged();
    emit availableColorFormatsChanged();
  });

  impl_->stream = new ImageStream(&impl_->image, this);
  auto future = QtConcurrent::run([this, fi]() {
    if (impl_->image.load(fi.absoluteFilePath())) {
      return true;
    }
    return false;
  });
  impl_->imageWatcher.setFuture(future);
}

ImageResource::~ImageResource() {}

QSize ImageResource::size() const {
  if (state() != Initialised) {
    return {};
  }
  return impl_->image.size();
}

bool ImageResource::setSize(const QSize& size) {
  if (state() != Initialised) {
    return false;
  }
  if (impl_->image.size() == size) {
    return true;
  } else {
    return false;
  }
}

QList<QVariant> ImageResource::availableSizes() const {
  if (state() != Initialised) {
    return {};
  }
  return {impl_->image.size()};
}

QList<QVariant> ImageResource::availableColorFormats() const {
  if (state() != Initialised) {
    return {};
  }
  return {QVariant::fromValue(impl_->image.format())};
}

QImage::Format ImageResource::colorFormat() const {
  if (state() != Initialised) {
    return {};
  }
  return impl_->image.format();
}

bool ImageResource::setColorFormat(QImage::Format format) {
  if (state() != Initialised) {
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
