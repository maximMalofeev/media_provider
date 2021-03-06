#ifndef VIDEOPROVIDER_H
#define VIDEOPROVIDER_H

#include "file_based_provider.h"

namespace MediaProvider {

class VideoProvider : public FileBasedProvider {
 public:
  VideoProvider(QObject* parent = nullptr);
  ~VideoProvider();

  QString provider() const override;
  Resource *createResource(const QString &resource) override;

  static const QString PROVIDER_NAME;
 private:
  struct Implementation;
  QScopedPointer<Implementation> impl_;
};

}  // namespace MediaProvider

#endif  // VIDEOPROVIDER_H
