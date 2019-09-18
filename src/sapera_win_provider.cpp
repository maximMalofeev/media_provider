#include "sapera_win_provider.h"
#include <SapClassBasic.h>
#include <QDebug>
#include <QTime>
#include <QtConcurrent/QtConcurrent>
#include "sapera_win_shortcuts.h"
#include "sapera_win_resource.h"

namespace MediaProvider {

const QString DEFAULT_ORIGIN = "sapera_server";

static bool isServerAppropriate(const int serverIndex) {
  if (!SapManager::IsServerAccessible(serverIndex)) {
    qWarning() << "Server not accessible";
    return false;
    ;
  }
  SapAcqDevice acqDevice(SapLocation(serverIndex), true);
  if (acqDevice.Create()) {
    std::string error;
    std::string deviceVendorName;
    bool isAppropriate = false;
    if (!getFeatureValue<std::string>(&acqDevice, VENDOR_FEATURE_NAME,
                                      deviceVendorName, error)) {
      qWarning() << error.c_str();
    } else {
      if (deviceVendorName == DALSA_VENDOR_NAME) {
        isAppropriate = true;
      }
    }
    acqDevice.Destroy();
    return isAppropriate;
  } else {
    qWarning() << "Unable to create device for server index" << serverIndex;
    return false;
  }
}

struct SaperaProvider::Implementation {
  State state = Initialising;
  QString origin = DEFAULT_ORIGIN;
  QStringList availableResources;
  QFutureWatcher<QStringList> resourcesWartcher;
};

SaperaProvider::SaperaProvider(QObject *parent) {
  impl_.reset(new Implementation);

  connect(&impl_->resourcesWartcher, &QFutureWatcher<QStringList>::finished,
          [this]() {
            impl_->state = Initialised;
            impl_->availableResources = impl_->resourcesWartcher.result();
            emit stateChanged();
            emit availableResourcesChanged();

            if (!SapManager::RegisterServerCallback(
                    SapManager::EventServerDisconnected |
                        SapManager::EventServerNew |
                        SapManager::EventServerConnected,
                    newServerEvent, this)) {
              qWarning() << "Unable to register server callback";
            }
          });

  auto resourcesFuture = QtConcurrent::run([]() {
    QStringList sources;
    for (auto server = 0; server < SapManager::GetServerCount(); server++) {
      char serverName[CORSERVER_MAX_STRLEN]{0};
      if (!SapManager::GetServerName(server, serverName)) {
        qWarning() << "Unable to get name for server" << server;
        continue;
      }

      if (isServerAppropriate(server)) {
        sources.push_back(serverName);
      }
    }

    return sources;
  });

  impl_->resourcesWartcher.setFuture(resourcesFuture);
}

SaperaProvider::~SaperaProvider() {
  if (!SapManager::UnregisterServerCallback()) {
    qWarning() << "Unable to unregister server callback";
  }
}

Provider::State SaperaProvider::state() const { return impl_->state; }

QString SaperaProvider::origin() const { return impl_->origin; }

bool SaperaProvider::setOrigin(const QString &orig) {
  if (impl_->origin == orig || orig == "") {
    return true;
  }
  return false;
}

QStringList SaperaProvider::availableResources() const {
  return impl_->availableResources;
}

Resource *SaperaProvider::createResource(const QString &resource) const {
  if (impl_->availableResources.contains(resource)) {
    return new SaperaResource{resource};
  }
  return {};
}

void SaperaProvider::newServerEvent(SapManCallbackInfo *callbackInfo) {
  const auto serverIndex = callbackInfo->GetServerIndex();
  char serverName[CORSERVER_MAX_STRLEN]{0};
  if (!SapManager::GetServerName(serverIndex, serverName)) {
    qWarning() << "Unable to get name for server" << serverIndex;
    return;
  }

  auto provider = static_cast<SaperaProvider *>(callbackInfo->GetContext());
  if (!provider) {
    qWarning() << "Unable to cast provider";
    return;
  }

  auto eventType = callbackInfo->GetEventType();

  if (eventType == SapManager::EventServerDisconnected) {
    qDebug() << serverName << "disconnected";
    auto index = provider->impl_->availableResources.indexOf(
        QRegExp(QString("%1.*").arg(serverName)));
    if (index != -1) {
      provider->impl_->availableResources.removeAt(index);
      emit provider->availableResourcesChanged();
    }
  } else if (eventType == SapManager::EventServerNew ||
             eventType == SapManager::EventServerConnected) {
    qDebug() << serverName << "connected";
    if (isServerAppropriate(serverIndex)) {
      provider->impl_->availableResources.push_back(serverName);
      emit provider->availableResourcesChanged();
    }
  }
}

}  // namespace MediaProvider
