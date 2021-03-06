#ifndef IMAGERESOURCE_H
#define IMAGERESOURCE_H

#include <media_provider/media_provider.h>

namespace MediaProvider {

class ImageResource : public Resource {
 public:
  ImageResource(const QString& res, QObject* parent = nullptr);
  ~ImageResource();

  QSize size() const override;
  bool setSize(const QSize& size) override;
  QList<QVariant> availableSizes() const override;
  QList<QVariant> availableColorFormats() const override;
  QImage::Format colorFormat() const override;
  bool setColorFormat(QImage::Format format) override;
  Stream* stream() override;

  void initialise() override;

 private:
  struct Implementation;
  QScopedPointer<Implementation> impl_;
};

}  // namespace MediaProvider

#endif  // IMAGERESOURCE_H
