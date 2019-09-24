#include "sapera_lnx_resource.h"
#include <GenApi/GenApi.h>
#include <gevapi.h>
#include <QDebug>

namespace MediaProvider {

static QImage::Format dalsaFormatToQImage(int format){
  if(format == fmtMono8){
    return QImage::Format_Grayscale8;
  }else if(format == fmtRGB8Packed){
    return QImage::Format_RGB888;
  }
  return QImage::Format_Invalid;
}

struct SaperaResource::Implementation {
  GEV_CAMERA_HANDLE handle{};
  QSize size;
  QImage::Format format;
};

SaperaResource::SaperaResource(const QString &res, QObject *parent)
    : Resource(parent) {
  impl_.reset(new Implementation);
  setResource(res);

  auto status =
      GevOpenCameraByName(const_cast<char *>(resource().toStdString().c_str()),
                          GevExclusiveMode, &impl_->handle);
  if (status != GEVLIB_OK) {
    setState(Invalid);
    setErrorString("Unable to open camera");
    return;
  }

  status = GevInitGenICamXMLFeatures(impl_->handle, TRUE);
  if (status != GEVLIB_OK){
    setState(Invalid);
    setErrorString("Unable to init xml features");
    return;
  }
  // Get the name of XML file name back (example only - in case you need it somewhere).
  char xmlFileName[MAX_PATH] = {0};
  status =
      GevGetGenICamXML_FileName(impl_->handle, (int)sizeof(xmlFileName), xmlFileName);
  if (status == GEVLIB_OK) {
    qDebug() << "XML stored as" << xmlFileName;
  }

  GenApi::CNodeMapRef *camera = static_cast<GenApi::CNodeMapRef*>(GevGetFeatureNodeMap(impl_->handle));
  if(!camera){
    setState(Invalid);
    setErrorString("Unable to get camera's features");
    return;
  }
  try {
    GenApi::CIntegerPtr ptrIntNode = camera->_GetNode("Width");
    impl_->size.setWidth(static_cast<int>(ptrIntNode->GetValue()));
    ptrIntNode = camera->_GetNode("Height");
    impl_->size.setHeight(static_cast<int>(ptrIntNode->GetValue()));
    GenApi::CEnumerationPtr ptrEnumNode = camera->_GetNode("PixelFormat") ;
    impl_->format = dalsaFormatToQImage(static_cast<int>(ptrEnumNode->GetIntValue()));
  } CATCH_GENAPI_ERROR(status);

  if(status != GEVLIB_OK){
    setState(Invalid);
    setErrorString("Unable to get camera's features");
    return;
  }

  emit sizeChanged();
  emit availableSizesChanged();
  emit colorFormatChanged();
  emit availableColorFormatsChanged();
  setState(Initialised);
}

SaperaResource::~SaperaResource() {}

QSize SaperaResource::size() const { return impl_->size; }

bool SaperaResource::setSize(const QSize &size) {
  if(state() != Initialised){
    return false;
  }
  if(size != impl_->size){
    setErrorString("Unable to set sise");
    return false;
  }
  return true;
}

QList<QVariant> SaperaResource::availableSizes() const {
  if(state() != Initialised){
    return {};
  }
  return {impl_->size};
}

QList<QVariant> SaperaResource::availableColorFormats() const {
  if(state() != Initialised){
    return {};
  }
  return {QVariant::fromValue(impl_->format)};
}

QImage::Format SaperaResource::colorFormat() const {
  if(state() != Initialised){
    return QImage::Format_Invalid;
  }
  return impl_->format;
}

bool SaperaResource::setColorFormat(const QImage::Format format) {
  if(state() != Initialised){
    return false;
  }
  if(impl_->format != format){
    setErrorString("Unable to set format");
    return false;
  }
  return true;
}

Stream *SaperaResource::stream() { return {}; }

}  // namespace MediaProvider
