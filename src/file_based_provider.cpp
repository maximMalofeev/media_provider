#include "file_based_provider.h"
#include <QDir>

namespace MediaProvider {

struct FileBasedProvider::Implementation {
  QStringList filters;

  QStringList getAvailableResources(const QString &origin) {
    QDir originDir(origin);
    originDir.setNameFilters(filters);
    return originDir.entryList(QDir::Files);
  }
};

FileBasedProvider::FileBasedProvider(const QStringList &filters,
                                     QObject *parent)
    : Provider(parent) {
  impl_.reset(new Implementation);
  setOrigin(QDir::currentPath());
  impl_->filters = filters;
}

FileBasedProvider::~FileBasedProvider() = default;

bool FileBasedProvider::setOrigin(const QString &orig) {
  if (orig == "") {
    Provider::setOrigin(QDir::currentPath());
  } else {
    if (!QDir(orig).exists()) {
      setErrorString(orig + " not exists");
      return false;
    } else {
      Provider::setOrigin(orig);
    }
  }

  if (state() == Initialised) {
    initialise();
  }

  return true;
}

void FileBasedProvider::initialise() {
  setState(Initialising);
  setAvailableResources(impl_->getAvailableResources(origin()));
  startTimer(1000);
  setState(Initialised);
}

void FileBasedProvider::timerEvent(QTimerEvent *event) {
  Q_UNUSED(event)
  setAvailableResources(impl_->getAvailableResources(origin()));
}

QString FileBasedProvider::getPath(const QString &resource) const {
  return origin() + "/" + resource;
}

}  // namespace MediaProvider
