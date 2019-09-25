#include "sapera_lnx_resource.h"
#include <GenApi/GenApi.h>
#include <gevapi.h>
#include <QDebug>
#include <QtConcurrent/QtConcurrent>
#include "sapera_lnx_stream.h"

namespace MediaProvider {

static QImage::Format dalsaFormatToQImage(int format) {
  if (format == fmtMono8) {
    return QImage::Format_Grayscale8;
  } else if (format == fmtRGB8Packed) {
    return QImage::Format_RGB888;
  }
  return QImage::Format_Invalid;
}

struct SaperaResource::Implementation {
  GEV_CAMERA_HANDLE handle{};
  QSize size;
  QImage::Format format;
  SaperaStream *stream{};
  QFutureWatcher<bool> initialisationWatcher;
};

SaperaResource::SaperaResource(const QString &res, QObject *parent)
    : Resource(parent) {
  impl_.reset(new Implementation);
  setResource(res);
  impl_->stream = new SaperaStream{this};

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

    status = GevInitGenICamXMLFeatures(impl_->handle, TRUE);
    if (status != GEVLIB_OK) {
      setErrorString("Unable to init xml features, status: " +
                     QString::number(status));
      return false;
    }

    GenApi::CNodeMapRef *camera =
        static_cast<GenApi::CNodeMapRef *>(GevGetFeatureNodeMap(impl_->handle));
    if (!camera) {
      setErrorString("Unable to get camera's features");
      return false;
    }

    GEV_CAMERA_OPTIONS camOptions{};
    GevGetCameraInterfaceOptions(impl_->handle, &camOptions);
    camOptions.heartbeat_timeout_ms = 90000;
//    camOptions.streamFrame_timeout_ms = 1001;
//    camOptions.streamNumFramesBuffered = 4;
//    camOptions.streamMemoryLimitMax = 64 * 1024 * 1024;
//    camOptions.streamPktSize = 9180;
//    camOptions.streamPktDelay = 10;
//    int numCpus = _GetNumCpus();
//    if (numCpus > 1) {
//      camOptions.streamThreadAffinity = numCpus - 1;
//      camOptions.serverThreadAffinity = numCpus - 2;
//    }
    GevSetCameraInterfaceOptions(impl_->handle, &camOptions);

    try {
      GenApi::CIntegerPtr ptrIntNode = camera->_GetNode("Width");
      impl_->size.setWidth(static_cast<int>(ptrIntNode->GetValue()));
      ptrIntNode = camera->_GetNode("Height");
      impl_->size.setHeight(static_cast<int>(ptrIntNode->GetValue()));
      GenApi::CEnumerationPtr ptrEnumNode = camera->_GetNode("PixelFormat");
      impl_->format =
          dalsaFormatToQImage(static_cast<int>(ptrEnumNode->GetIntValue()));
    }
    CATCH_GENAPI_ERROR(status);

    if (status != GEVLIB_OK) {
      setErrorString("Unable to get camera's features, status: " +
                     QString::number(status));
      return false;
    }

    return true;
  });
  impl_->initialisationWatcher.setFuture(initialisationFuture);
}

SaperaResource::~SaperaResource() {
  qDebug() << "SaperaResource::~SaperaResource()";
  impl_->stream->stop();
  GevCloseCamera(&impl_->handle);
}

QSize SaperaResource::size() const { return impl_->size; }

bool SaperaResource::setSize(const QSize &size) {
  if (state() != Initialised) {
    return false;
  }
  if (size != impl_->size) {
    setErrorString("Unable to set sise");
    return false;
  }
  return true;
}

QList<QVariant> SaperaResource::availableSizes() const {
  if (state() != Initialised) {
    return {};
  }
  return {impl_->size};
}

QList<QVariant> SaperaResource::availableColorFormats() const {
  if (state() != Initialised) {
    return {};
  }
  return {QVariant::fromValue(impl_->format)};
}

QImage::Format SaperaResource::colorFormat() const {
  if (state() != Initialised) {
    return QImage::Format_Invalid;
  }
  return impl_->format;
}

bool SaperaResource::setColorFormat(const QImage::Format format) {
  if (state() != Initialised) {
    return false;
  }
  if (impl_->format != format) {
    setErrorString("Unable to set format");
    return false;
  }
  return true;
}

Stream *SaperaResource::stream() { return impl_->stream; }

}  // namespace MediaProvider
