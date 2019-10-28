#include "dalsa_lnx_provider.h"
#include <GenApi/GenApi.h>
#include <gevapi.h>
#include <QDebug>
#include <QtConcurrent/QtConcurrent>
#include "dalsa_lnx_resource.h"

namespace MediaProvider {

constexpr int MAX_NETIF = 8;
constexpr int MAX_CAMERAS_PER_NETIF = 32;
constexpr int MAX_CAMERAS = MAX_NETIF * MAX_CAMERAS_PER_NETIF;

const QString ORIGIN = "dalsa_server";
const QString DALSA_MANUFACTURE = "Teledyne DALSA";

const QString DalsaProvider::PROVIDER_NAME = "DALSA_PROVIDER";

struct DalsaProvider::Implementation {
  QFutureWatcher<bool> availableCamerasWatcher;
};

DalsaProvider::DalsaProvider(QObject *parent) : Provider(parent) {
  impl_.reset(new Implementation);
  Provider::setOrigin(ORIGIN);
}

DalsaProvider::~DalsaProvider() {
  if (!impl_->availableCamerasWatcher.isFinished()) {
    impl_->availableCamerasWatcher.waitForFinished();
  }
  qDebug() << "DalsaProvider::~DalsaProvider()";
}

QString DalsaProvider::provider() const { return PROVIDER_NAME; }

bool DalsaProvider::setOrigin(const QString &orig) {
  if (orig == ORIGIN || orig == "") {
    return true;
  }
  setErrorString("Provider doesn't support setOrigin feature");
  return false;
}

void DalsaProvider::initialise() {
  setState(Initialising);
  connect(&impl_->availableCamerasWatcher, &QFutureWatcher<bool>::finished,
          [this]() {
            if (!impl_->availableCamerasWatcher.result()) {
              setState(Invalid);
              return;
            }
            setState(Initialised);
          });

  auto availableCamerasFuture = QtConcurrent::run([this]() {
    GEV_DEVICE_INTERFACE pCamera[MAX_CAMERAS]{};
    int numCamera{0};

    if (GevGetCameraList(pCamera, MAX_CAMERAS, &numCamera) != GEVLIB_OK) {
      setErrorString("Unable to get camera lists");
      return false;
    }

    QStringList availableCameras;
    for (int i = 0; i < numCamera; i++) {
      if (DALSA_MANUFACTURE == pCamera[i].manufacturer) {
        QString cam;
        cam.append(pCamera[i].username);
        availableCameras.push_back(cam);
      }
    }
    setAvailableResources(availableCameras);
    return true;
  });
  impl_->availableCamerasWatcher.setFuture(availableCamerasFuture);
}

Resource *DalsaProvider::createResource(const QString &resource) {
  if (availableResources().contains(resource)) {
    return new DalsaResource(resource);
  }
  setErrorString("There is no requested resource");
  return {};
}

}  // namespace MediaProvider
