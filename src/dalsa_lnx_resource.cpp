#include "dalsa_lnx_resource.h"
#include <GenApi/GenApi.h>
#include <gevapi.h>
#include <QDebug>
#include <QtConcurrent/QtConcurrent>
#include "dalsa_lnx_stream.h"

namespace MediaProvider {

static QImage::Format dalsaFormatToQImage(int format) {
  if (format == fmtMono8) {
    return QImage::Format_Grayscale8;
  } else if (format == fmtRGB8Packed) {
    return QImage::Format_RGB888;
  }
  return QImage::Format_Invalid;
}

struct DalsaResource::Implementation {
  GEV_CAMERA_HANDLE handle{};
  QSize size;
  QImage::Format format;
  DalsaStream *stream{};
  QFutureWatcher<bool> initialisationWatcher;
};

DalsaResource::DalsaResource(const QString &res, QObject *parent)
    : Resource(parent) {
  impl_.reset(new Implementation);
  setResource(res);
  impl_->stream = new DalsaStream{this};

  connect(&impl_->initialisationWatcher, &QFutureWatcher<bool>::finished,
          [this]() {
            if (!impl_->initialisationWatcher.result()) {
              setState(Invalid);
              return;
            }
            impl_->stream->setCameraHandle(impl_->handle);
            setState(Initialised);
            emit sizeChanged();
            emit colorFormatChanged();
            emit availableSizesChanged();
            emit availableColorFormatsChanged();
          });

  auto initialisationFuture = QtConcurrent::run([this]() {
    auto status = GevOpenCameraByName(
        const_cast<char *>(resource().toStdString().c_str()), GevExclusiveMode,
        &impl_->handle);
    if (status != GEVLIB_OK) {
      setErrorString("Unable to open camera, status: " +
                     QString::number(status));
      return false;
    }

    status = GevInitCameraRegisters(impl_->handle);
    if (status != GEVLIB_OK) {
      setErrorString("Unable to init gev camera registers, status: " +
                     QString::number(status));
      return false;
    }

    DALSA_GENICAM_GIGE_REGS regs;
    status = GevGetCameraRegisters(impl_->handle, &regs,
                                   sizeof(DALSA_GENICAM_GIGE_REGS));
    if (status != GEVLIB_OK) {
      setErrorString("Unable to get camera registers, status: " +
                     QString::number(status));
      return false;
    }

    UINT32 width, height, format;
    status = GevRegisterReadInt(impl_->handle, &regs.Width, 0, &width);
    if (status != GEVLIB_OK) {
      setErrorString("Unable to get camera width, status: " +
                     QString::number(status));
      return false;
    }
    status = GevRegisterReadInt(impl_->handle, &regs.Height, 0, &height);
    if (status != GEVLIB_OK) {
      setErrorString("Unable to get camera height, status: " +
                     QString::number(status));
      return false;
    }
    status = GevRegisterReadInt(impl_->handle, &regs.PixelFormat, 0, &format);
    if (status != GEVLIB_OK) {
      setErrorString("Unable to get camera pixel format, status: " +
                     QString::number(status));
      return false;
    }

    impl_->size.setWidth(static_cast<int>(width));
    impl_->size.setHeight(static_cast<int>(height));
    impl_->format = dalsaFormatToQImage(static_cast<int>(format));

    // TODO ask why it required to properly image grabbing
    status = GevInitGenICamXMLFeatures(impl_->handle, TRUE);
    if (status != GEVLIB_OK) {
      setErrorString("Unable to init xml features, status: " +
                     QString::number(status));
      return false;
    }

    //    GEV_CAMERA_OPTIONS camOptions{};
    //    GevGetCameraInterfaceOptions(impl_->handle, &camOptions);
    //    camOptions.heartbeat_timeout_ms = 90000;
    //    GevSetCameraInterfaceOptions(impl_->handle, &camOptions);

    return true;
  });
  impl_->initialisationWatcher.setFuture(initialisationFuture);
}

DalsaResource::~DalsaResource() {
  qDebug() << "DalsaResource::~DalsaResource()";
  impl_->stream->stop();
  GevCloseCamera(&impl_->handle);
}

QSize DalsaResource::size() const { return impl_->size; }

bool DalsaResource::setSize(const QSize &size) {
  if (state() != Initialised) {
    return false;
  }
  if (size != impl_->size) {
    setErrorString("Unable to set sise");
    return false;
  }
  return true;
}

QList<QVariant> DalsaResource::availableSizes() const {
  if (state() != Initialised) {
    return {};
  }
  return {impl_->size};
}

QList<QVariant> DalsaResource::availableColorFormats() const {
  if (state() != Initialised) {
    return {};
  }
  return {QVariant::fromValue(impl_->format)};
}

QImage::Format DalsaResource::colorFormat() const {
  if (state() != Initialised) {
    return QImage::Format_Invalid;
  }
  return impl_->format;
}

bool DalsaResource::setColorFormat(const QImage::Format format) {
  if (state() != Initialised) {
    return false;
  }
  if (impl_->format != format) {
    setErrorString("Unable to set format");
    return false;
  }
  return true;
}

Stream *DalsaResource::stream() { return impl_->stream; }

}  // namespace MediaProvider
