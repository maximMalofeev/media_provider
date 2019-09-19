#include "rtsp_provider.h"
#include <QDir>
#include <QFile>
#include "rtsp_resource.h"

namespace MediaProvider {

const QString DEFAULT_ORIGIN = "rtsp_provider.ini";

struct RtspProvider::Implementation {
  State state = Initialising;
  QString origin = DEFAULT_ORIGIN;
  QStringList availableResources;
  QString lastError;

  void fetchAvailableResources() {
    availableResources.clear();
    QFile rtspIni(origin);
    if (rtspIni.open(QIODevice::ReadOnly | QIODevice::Text)) {
      while (!rtspIni.atEnd()) {
        availableResources.push_back(
            QString::fromUtf8(rtspIni.readLine()).simplified());
      }
    }
  }
};

RtspProvider::RtspProvider(QObject *parent) : Provider(parent) {
  impl_.reset(new Implementation);
  if (QFile::exists(impl_->origin)) {
    impl_->fetchAvailableResources();
  }
  impl_->state = Initialised;
}

RtspProvider::~RtspProvider() {}

Provider::State RtspProvider::state() const { return impl_->state; }

QString RtspProvider::origin() const { return impl_->origin; }

bool RtspProvider::setOrigin(const QString &orig) {
  if (orig == "") {
    impl_->origin = DEFAULT_ORIGIN;
    emit originChanged();
    impl_->fetchAvailableResources();
    emit availableResourcesChanged();
    return true;
  }

  QFileInfo fi(orig);
  if (fi.exists() && fi.isFile()) {
    impl_->origin = orig;
    emit originChanged();
    impl_->fetchAvailableResources();
    emit availableResourcesChanged();
    return true;
  }

  impl_->lastError = orig + " unexists";
  return false;
}

QStringList RtspProvider::availableResources() const {
  return impl_->availableResources;
}

Resource *RtspProvider::createResource(const QString &resource) const {
  if (impl_->availableResources.contains(resource)) {
    return new RtspResource(resource);
  }
  impl_->lastError =
      "Unable to create unexisting resource, check available resources";
  return {};
}

QString RtspProvider::errorString() const { return impl_->lastError; }

}  // namespace MediaProvider
