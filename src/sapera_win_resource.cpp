#include "sapera_win_resource.h"
#include <SapClassBasic.h>
#include <QDebug>
#include <QtConcurrent/QtConcurrent>
#include "sapera_win_shortcuts.h"
#include "sapera_win_stream.h"

namespace MediaProvider {

static QImage::Format sapFormatToQImageFormat(const char *sapFormat) {
  if (strcmp(sapFormat, "Mono8") == 0) {
    return QImage::Format_Grayscale8;
  } else if (strcmp(sapFormat, "BGR8") == 0) {
    return QImage::Format_RGB888;
  } else {
    return QImage::Format_Invalid;
  }
}

struct SaperaResource::Implementation {
  QString resource;
  State state = Initialising;
  QSize size;
  QImage::Format format = QImage::Format_Invalid;
  QList<QVariant> availableColorFormats;
  SaperaStream *stream = {};
  QFutureWatcher<bool> initWatcher;
};

SaperaResource::SaperaResource(const QString &resource, QObject *parent)
    : Resource(parent) {
  impl_.reset(new Implementation);
  impl_->resource = resource;
  impl_->stream = new SaperaStream{resource, this};

  connect(&impl_->initWatcher, &QFutureWatcher<bool>::finished, [this]() {
    if (impl_->initWatcher.result() && impl_->stream->initialise()) {
      impl_->state = Initialised;

      emit sizeChanged();
      emit availableSizesChanged();
      emit colorFormatChanged();
      emit availableColorFormatsChanged();
    } else {
      impl_->state = Invalid;
    }
    emit stateChanged();
  });

  auto future = QtConcurrent::run([this]() {
    if (impl_->stream->sapDevice()->Create()) {
      std::string error;
      if (!getFeatureValue<int>(impl_->stream->sapDevice(), WIDTH_FEATURE_NAME,
                                impl_->size.rwidth(), error)) {
        qWarning() << "Unable to get" << WIDTH_FEATURE_NAME.c_str()
                   << "feature value, reason:"
                   << impl_->stream->sapDevice()->GetLastStatus();
        return false;
      }
      if (!getFeatureValue<int>(impl_->stream->sapDevice(), HEIGHT_FEATURE_NAME,
                                impl_->size.rheight(), error)) {
        qWarning() << "Unable to get" << HEIGHT_FEATURE_NAME.c_str()
                   << "feature value, reason:"
                   << impl_->stream->sapDevice()->GetLastStatus();
        return false;
      }
      std::string currentFormat;
      if (!getFeatureValue<std::string>(impl_->stream->sapDevice(),
                                        PIXEL_FORMAT_FEATURE_NAME,
                                        currentFormat, error)) {
        qWarning() << "Unable to get" << PIXEL_FORMAT_FEATURE_NAME.c_str()
                   << "feature value, reason:"
                   << impl_->stream->sapDevice()->GetLastStatus();
        return false;
      }
      impl_->format = sapFormatToQImageFormat(currentFormat.c_str());

      SapFeature feature(impl_->resource.toStdString().c_str());
      if (feature.Create()) {
        BOOL status = false;
        BOOL isAvailable = false;
        int enumCount;
        char enumString[32];

        if (impl_->stream->sapDevice()->IsFeatureAvailable(
                PIXEL_FORMAT_FEATURE_NAME.c_str(), &isAvailable) &&
            isAvailable) {
          if (impl_->stream->sapDevice()->GetFeatureInfo(
                  PIXEL_FORMAT_FEATURE_NAME.c_str(), &feature)) {
            if (feature.GetEnumCount(&enumCount)) {
              for (int i = 0; i < enumCount; i++) {
                feature.GetEnumString(i, enumString, sizeof(enumString));
                auto format = sapFormatToQImageFormat(enumString);
                if (format != QImage::Format_Invalid) {
                  impl_->availableColorFormats.push_back(
                      QVariant::fromValue(format));
                }
              }
              return true;
            } else {
              qWarning() << "Unable to get enum count, reason:"
                         << feature.GetLastStatus();
              feature.Destroy();
              return false;
            }
          } else {
            qWarning() << "Unable to get feature info, reason:"
                       << impl_->stream->sapDevice()->GetLastStatus();
            feature.Destroy();
            return false;
          }
        } else {
          qWarning() << "Unable to get feature info, reason:"
                     << impl_->stream->sapDevice()->GetLastStatus();
          feature.Destroy();
          return false;
        }
      } else {
        return false;
      }
    } else {
      return false;
    }
  });

  impl_->initWatcher.setFuture(future);
}

SaperaResource::~SaperaResource() {}

Resource::State SaperaResource::state() const { return impl_->state; }

QString SaperaResource::resource() const { return impl_->resource; }

QSize SaperaResource::size() const {
  if (impl_->state == Initialised) {
    return impl_->size;
  }
  return {};
}

bool SaperaResource::setSize(const QSize &size) {
  if (impl_->state == Initialised && impl_->size == size) {
    return true;
  }
  return false;
}

QList<QVariant> SaperaResource::availableSizes() const {
  if (impl_->state == Initialised) {
    return {impl_->size};
  }
  return {};
}

QList<QVariant> SaperaResource::availableColorFormats() const {
  if (impl_->state == Initialised) {
    return impl_->availableColorFormats;
  }
  return {};
}

QImage::Format SaperaResource::colorFormat() const {
  if (impl_->state == Initialised) {
    return impl_->format;
  }
  return QImage::Format_Invalid;
}

// TODO set format to sap device
bool SaperaResource::setColorFormat(const QImage::Format format) {
  if (impl_->state == Initialised &&
      impl_->availableColorFormats.contains(format)) {
    impl_->format = format;
    emit colorFormatChanged();
    return true;
  }
  return false;
}

Stream *SaperaResource::stream() { return impl_->stream; }

}  // namespace MediaProvider
