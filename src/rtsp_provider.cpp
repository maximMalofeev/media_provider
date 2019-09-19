#include "rtsp_provider.h"
#include <QDir>
#include <QFile>
#include "rtsp_resource.h"

namespace MediaProvider {

const QString DEFAULT_ORIGIN = "rtsp_provider.ini";

struct RtspProvider::Implementation {
  QStringList fetchAvailableResources(const QString &origin) {
    QStringList availableResources;
    QFile rtspIni(origin);
    if (rtspIni.open(QIODevice::ReadOnly | QIODevice::Text)) {
      while (!rtspIni.atEnd()) {
        availableResources.push_back(
            QString::fromUtf8(rtspIni.readLine()).simplified());
      }
    }
    return availableResources;
  }
};

RtspProvider::RtspProvider(QObject *parent) : Provider(parent) {
  impl_.reset(new Implementation);
  Provider::setOrigin(DEFAULT_ORIGIN);
  if (QFile::exists(origin())) {
    setAvailableResources(impl_->fetchAvailableResources(origin()));
  }
  setState(Initialised);
}

RtspProvider::~RtspProvider() {}

bool RtspProvider::setOrigin(const QString &orig) {
  if (orig == "") {
    Provider::setOrigin(DEFAULT_ORIGIN);
    setAvailableResources(impl_->fetchAvailableResources(origin()));
    return true;
  }

  QFileInfo fi(orig);
  if (fi.exists() && fi.isFile()) {
    Provider::setOrigin(orig);
    setAvailableResources(impl_->fetchAvailableResources(origin()));
    return true;
  }

  setErrorString(orig + " unexists");
  return false;
}

Resource *RtspProvider::createResource(const QString &resource) {
  if (availableResources().contains(resource)) {
    return new RtspResource(resource);
  }
  setErrorString(
      "Unable to create unexisting resource, check available resources");
  return {};
}

}  // namespace MediaProvider
