#include "sapera_win_stream.h"
#include <SapClassBasic.h>
#include <QDebug>
#include "sapera_win_buffer_processing.h"
#include "sapera_win_resource.h"

namespace MediaProvider {

struct SaperaStream::Implementation {
  State state = Stopped;
  QScopedPointer<SapAcqDevice> sapDevice{};
  QScopedPointer<SapBufferWithTrash> sapBufWithTrash;
  QScopedPointer<SapAcqDeviceToBuf> sapAcqDeviceToBuffer;
  SapBufferProcessing *sapBufferProcessing{};
  SapBufferProcessing *bufferProcessing{};
};

SaperaStream::SaperaStream(const QString &resource, QObject *parent)
    : Stream(parent) {
  impl_.reset(new Implementation);
  impl_->sapDevice.reset(
      new SapAcqDevice{SapLocation{resource.toStdString().c_str()}});
}

SaperaStream::~SaperaStream() {
  if (impl_->sapAcqDeviceToBuffer) {
    impl_->sapAcqDeviceToBuffer->Destroy();
  }
  if (impl_->sapBufWithTrash) {
    impl_->sapBufWithTrash->Destroy();
  }
}

Stream::State SaperaStream::state() const { return impl_->state; }

bool SaperaStream::initialise() {
  impl_->sapBufWithTrash.reset(
      new SapBufferWithTrash{4, impl_->sapDevice.get()});
  if (!impl_->sapBufWithTrash->Create()) {
    impl_->state = Invalid;
    qWarning() << "Unable to create SapBufferWithTrash, reason:"
               << impl_->sapBufWithTrash->GetLastStatus();
    return false;
  }

  impl_->sapBufferProcessing =
      new SapBufferProcessing(impl_->sapBufWithTrash.get(), this);
  if (!impl_->sapBufferProcessing->Create()) {
    impl_->state = Invalid;
    qWarning() << "Unable to create SapBufferProcessing, reason:"
               << impl_->sapBufferProcessing->GetLastStatus();
    return false;
  }
  impl_->sapAcqDeviceToBuffer.reset(new SapAcqDeviceToBuf{
      impl_->sapDevice.get(), impl_->sapBufWithTrash.get(), XferCallback,
      impl_->sapBufferProcessing});
  if (!impl_->sapAcqDeviceToBuffer->Create()) {
    impl_->state = Invalid;
    qWarning() << "Unable to create SapAcqDeviceToBuf, reason:"
               << impl_->sapAcqDeviceToBuffer->GetLastStatus();
    return false;
  }
  impl_->sapAcqDeviceToBuffer->SetAutoEmpty(false);

  QObject::connect(impl_->sapBufferProcessing, &SapBufferProcessing::newFrame,
                   this, &SaperaStream::newFrame);
  return true;
}

SapAcqDevice *SaperaStream::sapDevice() { return impl_->sapDevice.get(); }

void SaperaStream::XferCallback(SapXferCallbackInfo *pInfo) {
  if (pInfo->GetEventType() != SapXferPair::EventEndOfFrame) {
    return;
  }

  auto sapBufProcessing_ =
      static_cast<SapBufferProcessing *>(pInfo->GetContext());
  sapBufProcessing_->Execute();
}

void SaperaStream::start() {
  if (impl_->state == Stopped) {
    impl_->sapAcqDeviceToBuffer->Grab();
    impl_->state = Playing;
    emit stateChanged();
  }
}

void SaperaStream::stop() {
  if (impl_->state == Playing) {
    impl_->state = Stopped;
    emit stateChanged();
    impl_->sapAcqDeviceToBuffer->Freeze();
  }
}

}  // namespace MediaProvider
