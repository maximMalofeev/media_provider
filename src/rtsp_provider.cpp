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
    if (QFile::exists(impl_->origin)) {
      impl_->fetchAvailableResources();
      emit availableResourcesChanged();
    }
    return true;
  }
  if (QFile::exists(orig)) {
    impl_->origin = orig;
    impl_->fetchAvailableResources();
    return true;
  }
  return false;
}

QStringList RtspProvider::availableResources() const {
  return impl_->availableResources;
}

Resource *RtspProvider::createResource(const QString &resource) const {
  if (impl_->availableResources.contains(resource)) {
    return new RtspResource(resource);
  }
  return {};
}

}  // namespace MediaProvider
