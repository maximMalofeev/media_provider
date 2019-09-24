#ifndef SAPERARESOURCE_H
#define SAPERARESOURCE_H

#include <media_provider/media_provider.h>

namespace MediaProvider {

class SaperaResource : public Resource {
 public:
  SaperaResource(const QString& res, QObject* parent = nullptr);
  ~SaperaResource();

  QSize size() const;
  bool setSize(const QSize& size);
  QList<QVariant> availableSizes() const;
  QList<QVariant> availableColorFormats() const;
  QImage::Format colorFormat() const;
  bool setColorFormat(const QImage::Format format);
  Stream* stream();

 private:
  struct Implementation;
  QScopedPointer<Implementation> impl_;
};

}  // namespace MediaProvider

#endif  // SAPERARESOURCE_H
