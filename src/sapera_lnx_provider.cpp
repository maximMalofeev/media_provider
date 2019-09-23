#include "sapera_lnx_provider.h"
#include <GenApi/GenApi.h>
#include <gevapi.h>

namespace MediaProvider {

constexpr int MAX_NETIF = 8;
constexpr int MAX_CAMERAS_PER_NETIF = 32;
constexpr int MAX_CAMERAS = MAX_NETIF * MAX_CAMERAS_PER_NETIF;

const QString ORIGIN = "GIGE-V";
const QString DALSA_MANUFACTURE = "Teledyne DALSA";

struct SaperaProvider::Implementation {
  GEV_DEVICE_INTERFACE pCamera[MAX_CAMERAS]{};
  int numCamera{0};
};

SaperaProvider::SaperaProvider(QObject *parent) : Provider(parent) {
  impl_.reset(new Implementation);
  Provider::setOrigin(ORIGIN);
  if (GevGetCameraList(impl_->pCamera, MAX_CAMERAS, &impl_->numCamera) !=
      GEVLIB_OK) {
    setState(Invalid);
    setErrorString("Unable to get camera lists");
    return;
  }
  QStringList availableCameras;
  for (int i = 0; i < impl_->numCamera; i++) {
    if (DALSA_MANUFACTURE == impl_->pCamera[i].manufacturer) {
      QString cam;
      cam.append(impl_->pCamera[i].username);
      availableCameras.push_back(cam);
    }
  }
  setAvailableResources(availableCameras);
  setState(Initialised);
}

SaperaProvider::~SaperaProvider() {}

bool SaperaProvider::setOrigin(const QString &orig) {
  if (orig == ORIGIN || orig == "") {
    return true;
  }
  setErrorString("Provider doesn't support setOrigin feature");
  return false;
}

Resource *SaperaProvider::createResource(const QString &resource) {
  if(availableResources().contains(resource)){
    return {};
  }
  return {};
}

}  // namespace MediaProvider
