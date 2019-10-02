#ifndef SAPERARESOURCE_H
#define SAPERARESOURCE_H

#include <media_provider/media_provider.h>

namespace MediaProvider {

class DalsaResource : public Resource {
 public:
  DalsaResource(const QString& res, QObject* parent = nullptr);
  ~DalsaResource();

  QSize size() const;
  bool setSize(const QSize& size);
  QList<QVariant> availableSizes() const;
  QList<QVariant> availableColorFormats() const;
  QImage::Format colorFormat() const;
  bool setColorFormat(QImage::Format format);
  Stream* stream();

 private:
  struct Implementation;
  QScopedPointer<Implementation> impl_;
};

}  // namespace MediaProvider

#endif  // SAPERARESOURCE_H
