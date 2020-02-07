#include "dalsa_win_stream.h"
#include <SapClassBasic.h>
#include <QDebug>
#include "dalsa_win_buffer_processing.h"
#include "dalsa_win_resource.h"

namespace MediaProvider {

struct DalsaStream::Implementation {
  QScopedPointer<SapAcqDevice> sapDevice;
  QScopedPointer<SapBufferWithTrash> sapBufWithTrash;
  QScopedPointer<SapAcqDeviceToBuf> sapAcqDeviceToBuffer;
  DalsaBufferProcessing *sapBufferProcessing{};
  DalsaBufferProcessing *bufferProcessing{};
};

DalsaStream::DalsaStream(const QString &resource, Resource *parent)
    : Stream(parent) {
  impl_.reset(new Implementation);
  impl_->sapDevice.reset(new SapAcqDevice{
      SapLocation{SapManager::GetServerIndex(resource.toStdString().c_str())}});
}

DalsaStream::~DalsaStream() {
  stop();
  if (impl_->sapAcqDeviceToBuffer) {
    impl_->sapAcqDeviceToBuffer->Destroy();
  }
  if (impl_->sapBufWithTrash) {
    impl_->sapBufWithTrash->Destroy();
  }
  if (impl_->sapDevice) {
    impl_->sapDevice->Destroy();
  }
}

bool DalsaStream::initialise() {
  impl_->sapBufWithTrash.reset(
      new SapBufferWithTrash{4, impl_->sapDevice.get()});
  if (!impl_->sapBufWithTrash->Create()) {
    setErrorString(QString{"Unable to create SapBufferWithTrash, reason: "} +
                   impl_->sapBufWithTrash->GetLastStatus());
    setState(Invalid);
    return false;
  }

  impl_->sapBufferProcessing =
      new DalsaBufferProcessing(impl_->sapBufWithTrash.get(), this);
  if (!impl_->sapBufferProcessing->Create()) {
    setErrorString(QString{"Unable to create SapBufferProcessing, reason: "} +
                   impl_->sapBufferProcessing->GetLastStatus());
    setState(Invalid);
    return false;
  }
  impl_->sapAcqDeviceToBuffer.reset(new SapAcqDeviceToBuf{
      impl_->sapDevice.get(), impl_->sapBufWithTrash.get(), XferCallback,
      impl_->sapBufferProcessing});
  if (!impl_->sapAcqDeviceToBuffer->Create()) {
    setErrorString(QString{"Unable to create SapAcqDeviceToBuf, reason: "} +
                   impl_->sapAcqDeviceToBuffer->GetLastStatus());
    setState(Invalid);
    return false;
  }
  // TODO figure out why i do SetAutoEmpty(false)
  impl_->sapAcqDeviceToBuffer->SetAutoEmpty(false);

  QObject::connect(impl_->sapBufferProcessing, &DalsaBufferProcessing::newFrame,
                   this, &DalsaStream::newFrame);
  return true;
}

SapAcqDevice *DalsaStream::sapDevice() { return impl_->sapDevice.get(); }

void DalsaStream::XferCallback(SapXferCallbackInfo *pInfo) {
  if (pInfo->GetEventType() != SapXferPair::EventEndOfFrame) {
    return;
  }

  auto sapBufProcessing =
      static_cast<DalsaBufferProcessing *>(pInfo->GetContext());
  sapBufProcessing->Execute();
}

void DalsaStream::start() {
  if (resource()->state() != Resource::Initialised) {
    return;
  }
  if (auto s = state(); s == Invalid || s == Playing) {
    return;
  }
  if (!impl_->sapAcqDeviceToBuffer->Grab()) {
    setErrorString(QString{"Unable to grab, reason"} +
                   impl_->sapAcqDeviceToBuffer->GetLastStatus());
    setState(Invalid);
    return;
  }
  setState(Playing);
}

void DalsaStream::stop() {
  if (auto s = state(); s == Invalid || s == Stopped) {
    return;
  }
  impl_->sapAcqDeviceToBuffer->Freeze();
  impl_->sapAcqDeviceToBuffer->Wait(5000);
  setState(Stopped);
}

}  // namespace MediaProvider
