#include "image_resource.h"
#include <QImage>
#include <QtConcurrent/QtConcurrent>
#include "image_stream.h"

namespace MediaProvider {

struct ImageResource::Implementation {
  QFutureWatcher<bool> imageWatcher;
  QFileInfo fi;
  QImage image;
  ImageStream* stream = {};
};

ImageResource::ImageResource(const QString& res, QObject* parent)
    : Resource(parent) {
  impl_.reset(new Implementation);
  impl_->fi = QFileInfo(res);
  setResource(impl_->fi.fileName());
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

void ImageResource::initialise() {
  setState(Initialising);
  connect(&impl_->imageWatcher, &QFutureWatcher<QImage>::finished, this, [this]() {
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
  }, Qt::QueuedConnection);

  impl_->stream = new ImageStream(&impl_->image, this);
  auto future = QtConcurrent::run([this]() {
    if (impl_->image.load(impl_->fi.absoluteFilePath())) {
      return true;
    }
    return false;
  });
  impl_->imageWatcher.setFuture(future);
}

}  // namespace MediaProvider
