#include "rtsp_provider.h"
#include <QDir>
#include <QFile>
#include "rtsp_resource.h"

namespace MediaProvider {

const QString DEFAULT_ORIGIN = "rtsp_provider.ini";

const QString RtspProvider::PROVIDER_NAME = "RTSP_PROVIDER";

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
}

RtspProvider::~RtspProvider() {}

QString RtspProvider::provider() const { return PROVIDER_NAME; }

bool RtspProvider::setOrigin(const QString &orig) {
  if (orig == "") {
    Provider::setOrigin(DEFAULT_ORIGIN);
  } else {
    QFileInfo fi(orig);
    if (fi.exists() && fi.isFile()) {
      Provider::setOrigin(orig);
    } else {
      setErrorString(orig + " unexists");
      return false;
    }
  }

  if(state() == Initialised){
    initialise();
  }

  return true;
}

void RtspProvider::initialise() {
  setState(Initialising);
  setAvailableResources(impl_->fetchAvailableResources(origin()));
  setState(Initialised);
}

Resource *RtspProvider::createResource(const QString &resource) {
  if (availableResources().contains(resource)) {
    return new RtspResource{resource, this};
  }
  setErrorString(
      "Unable to create unexisting resource, check available resources");
  return {};
}

}  // namespace MediaProvider
