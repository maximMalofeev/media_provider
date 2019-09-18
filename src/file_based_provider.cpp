#include "file_based_provider.h"
#include <QDir>

namespace MediaProvider {

struct FileBasedProvider::Implementation {
  State state = Initialising;
  QString origin;
  QStringList availableResources;
  QStringList filters;

  QStringList getAvailableResources() {
    QDir originDir(origin);
    originDir.setNameFilters(filters);

    return originDir.entryList(QDir::Files);
  }
};

FileBasedProvider::FileBasedProvider(const QStringList &filters,
                                     QObject *parent)
    : Provider(parent) {
  impl_.reset(new Implementation);
  impl_->origin = QDir::currentPath();
  impl_->filters = filters;
  impl_->availableResources = impl_->getAvailableResources();
  impl_->state = Initialised;
  startTimer(1000);
}

FileBasedProvider::~FileBasedProvider() = default;

Provider::State FileBasedProvider::state() const { return impl_->state; }

QString FileBasedProvider::origin() const { return impl_->origin; }

bool FileBasedProvider::setOrigin(const QString &orig) {
  if (impl_->origin != orig) {
    if (orig == "") {
      if (impl_->origin != QDir::currentPath()) {
        impl_->origin = QDir::currentPath();
        emit originChanged();
        setAvailableResources(impl_->getAvailableResources());
      }
      return true;
    } else if (!QDir(orig).exists()) {
      return false;
    }

    impl_->origin = orig;
    emit originChanged();
    setAvailableResources(impl_->getAvailableResources());
  }
  return true;
}

QStringList FileBasedProvider::availableResources() const {
  return impl_->availableResources;
}

void FileBasedProvider::setAvailableResources(
    const QStringList &availableResources) {
  if (availableResources != impl_->availableResources) {
    impl_->availableResources = availableResources;
    emit availableResourcesChanged();
  }
}

void FileBasedProvider::timerEvent(QTimerEvent *event) {
  Q_UNUSED(event)
  setAvailableResources(impl_->getAvailableResources());
}

QString FileBasedProvider::getPath(const QString &resource) const {
  return impl_->origin + "/" + resource;
}

}  // namespace MediaProvider
