#include "dalsa_win_provider.h"
#include <SapClassBasic.h>
#include <QDebug>
#include <QTime>
#include <QtConcurrent/QtConcurrent>
#include "dalsa_win_resource.h"
#include "dalsa_win_shortcuts.h"

namespace MediaProvider {

const QString DEFAULT_ORIGIN = "dalsa_server";

// Check if able to create server and vendor is Dalsa
static bool isServerAppropriate(const int serverIndex) {
  if(SapManager::IsSystemLocation(serverIndex)){
    return false;
  }
  if (!SapManager::IsServerAccessible(serverIndex)) {
    qDebug() << "Server not accessible";
    return false;
  }
  SapAcqDevice acqDevice(SapLocation(serverIndex), true);
  if (acqDevice.Create()) {
    std::string error;
    std::string deviceVendorName;
    bool isAppropriate = false;
    if (!getFeatureValue<std::string>(&acqDevice, VENDOR_FEATURE_NAME,
                                      deviceVendorName, error)) {
      qDebug() << error.c_str();
    } else {
      if (deviceVendorName == DALSA_VENDOR_NAME) {
        isAppropriate = true;
      }
    }
    acqDevice.Destroy();
    return isAppropriate;
  } else {
    qDebug() << "Unable to create device for server index" << serverIndex;
    return false;
  }
}

const QString DalsaProvider::PROVIDER_NAME = "DALSA_PROVIDER";

struct DalsaProvider::Implementation {
  QFutureWatcher<QStringList> resourcesWartcher;
};

DalsaProvider::DalsaProvider(QObject *parent) {
  impl_.reset(new Implementation);
  Provider::setOrigin(DEFAULT_ORIGIN);

  SapManager::SetDisplayStatusMode(SapManager::StatusLog);

  connect(&impl_->resourcesWartcher, &QFutureWatcher<QStringList>::finished,
          [this]() {
            setAvailableResources(impl_->resourcesWartcher.result());
            setState(Initialised);

            if (!SapManager::RegisterServerCallback(
                    SapManager::EventServerDisconnected |
                        SapManager::EventServerNew |
                        SapManager::EventServerConnected,
                    onServerEvent, this)) {
              qCritical() << "Unable to register server callback";
              setState(Invalid);
              setErrorString(
                  QString("Unable to register server callback, reason: ") +
                  SapManager::GetLastStatus());
            }
          });

  auto resourcesFuture = QtConcurrent::run([]() {
    QStringList sources;
    for (auto server = 0; server < SapManager::GetServerCount(); server++) {
      char serverName[CORSERVER_MAX_STRLEN]{0};
      if (!SapManager::GetServerName(server, serverName)) {
        qDebug() << "Unable to get name for server" << server;
        continue;
      }

      qDebug() << "Checking" << serverName;
      if (isServerAppropriate(server)) {
        sources.push_back(serverName);
      }
    }

    return sources;
  });

  impl_->resourcesWartcher.setFuture(resourcesFuture);
}

DalsaProvider::~DalsaProvider() {
  if (!SapManager::UnregisterServerCallback()) {
    qWarning() << "Unable to unregister server callback, reson:" << SapManager::GetLastStatus();
  }
  if(!SapManager::Close()){
    qWarning() << "Unable to close sap, reson:" << SapManager::GetLastStatus();
  }
}

QString DalsaProvider::provider() const { return PROVIDER_NAME; }

bool DalsaProvider::setOrigin(const QString &orig) {
  if (origin() == orig || orig == "") {
    return true;
  }
  setErrorString(
      "Unable to set origin, SpaeraProvider supports only default origin");
  return false;
}

Resource *DalsaProvider::createResource(const QString &resource) {
  if (availableResources().contains(resource)) {
    auto res = new DalsaResource{resource};
    connect(this, &DalsaProvider::serverDisconnected, res, &DalsaResource::onServerDisconnected);
    return res;
  }
  setErrorString(
      "Unable to create unexisting resource, check available resources");
  return {};
}

void DalsaProvider::onServerEvent(SapManCallbackInfo *callbackInfo) {
  qDebug() << "onServerEvent";
  const auto serverIndex = callbackInfo->GetServerIndex();
  char serverName[CORSERVER_MAX_STRLEN]{0};
  if (!SapManager::GetServerName(serverIndex, serverName)) {
    qWarning() << "Unable to get name for server" << serverIndex;
    return;
  }

  auto provider = static_cast<DalsaProvider *>(callbackInfo->GetContext());
  if (!provider) {
    qWarning() << "Unable to cast provider";
    return;
  }

  auto eventType = callbackInfo->GetEventType();

  if (eventType == SapManager::EventServerDisconnected) {
    qDebug() << serverName << "disconnected";
    emit provider->serverDisconnected(QString{serverName});
    auto index = provider->availableResources().indexOf(
        QRegExp(QString("%1.*").arg(serverName)));
    if (index != -1) {
      auto availableRes = provider->availableResources();
      availableRes.removeAt(index);
      provider->setAvailableResources(availableRes);
    }
  } else if (eventType == SapManager::EventServerNew ||
             eventType == SapManager::EventServerConnected) {
    qDebug() << serverName << "connected";
    if (isServerAppropriate(serverIndex)) {
      auto availableRes = provider->availableResources();
      availableRes.push_back(serverName);
      //TODO protect with mutex
      provider->setAvailableResources(availableRes);
    }
  }
}

}  // namespace MediaProvider
