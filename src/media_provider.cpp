#include <media_provider/media_provider.h>
#include <QAbstractVideoSurface>
#include <QDebug>
#include "image_provider.h"
#include "rtsp_provider.h"
#include "video_provider.h"
#ifdef WIN32
#include "dalsa_win_provider.h"
#elif unix
#include "dalsa_lnx_provider.h"
#endif

namespace MediaProvider {

// ==================Stream==================
struct Stream::Implementation {
  State state = Stopped;
  QString lastError;
  Resource *resource;
};

Stream::Stream(Resource *parent) : QObject(parent) {
  impl_.reset(new Implementation);
  impl_->resource = parent;
  connect(parent, &Resource::stateChanged, [this]() {
    auto res = dynamic_cast<Resource *>(this->parent());
    if (res && res->state() == Resource::Invalid) {
      setState(Invalid);
      setErrorString("Resource switched to Invalid state");
    }
  });
}

void Stream::setState(const Stream::State state) {
  if (impl_->state != state) {
    impl_->state = state;
    emit stateChanged();
  }
}

void Stream::setErrorString(const QString &errorStr) {
  if (impl_->lastError != errorStr) {
    impl_->lastError = errorStr;
    emit errorStringChanged();
  }
}

Resource *Stream::resource() const { return impl_->resource; }

Stream::~Stream() {}

Stream::State Stream::state() const { return impl_->state; }

QString Stream::errorString() const { return impl_->lastError; }

// ==================Resource==================
struct Resource::Implementation {
  State state = Initialising;
  QString resource;
  QString lastError;
};

Resource::Resource(QObject *parent) : QObject(parent) {
  impl_.reset(new Implementation);
}

void Resource::setState(const Resource::State state) {
  if (impl_->state != state) {
    impl_->state = state;
    emit stateChanged();
  }
}

void Resource::setResource(const QString &resource) {
  impl_->resource = resource;
}

void Resource::setErrorString(const QString &errorStr) {
  if (impl_->lastError != errorStr) {
    impl_->lastError = errorStr;
    emit errorStringChanged();
  }
}

Resource::~Resource() {}

Resource::State Resource::state() const { return impl_->state; }

QString Resource::resource() const { return impl_->resource; }

QString Resource::errorString() const { return impl_->lastError; }

// ==================Provider==================
struct Provider::Implementation {
  State state = Initialising;
  QString origin;
  QStringList availableResources;
  QString lastError;
};

Provider::Provider(QObject *parent) : QObject(parent) {
  impl_.reset(new Implementation);
}

Provider::~Provider() {}

Provider::State Provider::state() const { return impl_->state; }

QString Provider::origin() const { return impl_->origin; }

void Provider::setState(const Provider::State state) {
  if (impl_->state != state) {
    impl_->state = state;
    emit stateChanged();
  }
}

void Provider::setAvailableResources(const QStringList &availableResources) {
  if (impl_->availableResources != availableResources) {
    impl_->availableResources = availableResources;
    emit availableResourcesChanged();
  }
}

void Provider::setErrorString(const QString &errorStr) {
  if (impl_->lastError != errorStr) {
    impl_->lastError = errorStr;
    emit errorStringChanged();
  }
}

bool Provider::setOrigin(const QString &orig) {
  if (impl_->origin != orig) {
    impl_->origin = orig;
    emit originChanged();
  }
  return true;
}

QStringList Provider::availableResources() const {
  return impl_->availableResources;
}

QString Provider::errorString() const { return impl_->lastError; }

Provider *Provider::createProvider(const QString &providerName,
                                   QObject *parent) {
  if (providerName == "ImageProvider") {
    return new ImageProvider(parent);
  } else if (providerName == "VideoProvider") {
    return new VideoProvider(parent);
  } else if (providerName == "RtspProvider") {
    return new RtspProvider(parent);
  } else if (providerName == "DalsaProvider") {
    return new DalsaProvider(parent);
  } else {
    return {};
  }
}

QStringList Provider::availableProviders() {
  return QStringList{"ImageProvider", "VideoProvider", "RtspProvider",
                     "DalsaProvider"};
}

}  // namespace MediaProvider
