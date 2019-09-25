#include "sapera_lnx_provider.h"
#include <GenApi/GenApi.h>
#include <gevapi.h>
#include "sapera_lnx_resource.h"
#include <QtConcurrent/QtConcurrent>
#include <QDebug>

namespace MediaProvider {

constexpr int MAX_NETIF = 8;
constexpr int MAX_CAMERAS_PER_NETIF = 32;
constexpr int MAX_CAMERAS = MAX_NETIF * MAX_CAMERAS_PER_NETIF;

const QString ORIGIN = "GIGE-V";
const QString DALSA_MANUFACTURE = "Teledyne DALSA";

struct SaperaProvider::Implementation {
  QFutureWatcher<bool> availableCamerasWatcher;
};

SaperaProvider::SaperaProvider(QObject *parent) : Provider(parent) {
  GEVLIB_CONFIG_OPTIONS options{};
  GevGetLibraryConfigOptions(&options);
  options.logLevel = GEV_LOG_LEVEL_DEBUG;
  GevSetLibraryConfigOptions(&options);

  impl_.reset(new Implementation);
  Provider::setOrigin(ORIGIN);

  connect(&impl_->availableCamerasWatcher, &QFutureWatcher<bool>::finished, [this](){
    if(!impl_->availableCamerasWatcher.result()){
      setState(Invalid);
      return;
    }
    setState(Initialised);
  });

  auto availableCamerasFuture = QtConcurrent::run([this](){
    GEV_DEVICE_INTERFACE pCamera[MAX_CAMERAS]{};
    int numCamera{0};

    if (GevGetCameraList(pCamera, MAX_CAMERAS, &numCamera) !=
        GEVLIB_OK) {
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

SaperaProvider::~SaperaProvider() {
  GevApiUninitialize();
  qDebug() << "SaperaProvider::~SaperaProvider()";
}

bool SaperaProvider::setOrigin(const QString &orig) {
  if (orig == ORIGIN || orig == "") {
    return true;
  }
  setErrorString("Provider doesn't support setOrigin feature");
  return false;
}

Resource *SaperaProvider::createResource(const QString &resource) {
  if(availableResources().contains(resource)){
    return new SaperaResource(resource);
  }
  setErrorString("There is no requested resource");
  return {};
}

}  // namespace MediaProvider
