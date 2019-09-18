#ifndef SAPERASTREAM_H
#define SAPERASTREAM_H

#include <media_provider/media_provider.h>

class SapAcqDevice;
class SapXferCallbackInfo;

namespace MediaProvider {

class SaperaStream : public Stream {
 public:
  SaperaStream(const QString& resource, QObject* parent = nullptr);
  ~SaperaStream();

  State state() const override;

  bool initialise();
  SapAcqDevice* sapDevice();

  static void XferCallback(SapXferCallbackInfo* pInfo);

 public slots:
  void start() override;
  void stop() override;

 private:
  struct Implementation;
  QScopedPointer<Implementation> impl_;
};

}  // namespace MediaProvider

#endif  // SAPERASTREAM_H
