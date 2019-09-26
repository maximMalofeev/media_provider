#include "dalsa_win_stream.h"
#include <SapClassBasic.h>
#include <QDebug>
#include "dalsa_win_buffer_processing.h"
#include "dalsa_win_resource.h"

namespace MediaProvider {

struct SaperaStream::Implementation {
  QScopedPointer<SapAcqDevice> sapDevice;
  QScopedPointer<SapBufferWithTrash> sapBufWithTrash;
  QScopedPointer<SapAcqDeviceToBuf> sapAcqDeviceToBuffer;
  SapBufferProcessing *sapBufferProcessing{};
  SapBufferProcessing *bufferProcessing{};
};

SaperaStream::SaperaStream(const QString &resource, Resource *parent)
    : Stream(parent) {
  impl_.reset(new Implementation);
  impl_->sapDevice.reset(
      new SapAcqDevice{SapLocation{resource.toStdString().c_str()}});
}

SaperaStream::~SaperaStream() {
  stop();
  if (impl_->sapAcqDeviceToBuffer) {
    impl_->sapAcqDeviceToBuffer->Destroy();
  }
  if (impl_->sapBufWithTrash) {
    impl_->sapBufWithTrash->Destroy();
  }
}

bool SaperaStream::initialise() {
  impl_->sapBufWithTrash.reset(
      new SapBufferWithTrash{4, impl_->sapDevice.get()});
  if (!impl_->sapBufWithTrash->Create()) {
    setState(Invalid);
    setErrorString(QString{"Unable to create SapBufferWithTrash, reason: "} +
                   impl_->sapBufWithTrash->GetLastStatus());
    return false;
  }

  impl_->sapBufferProcessing =
      new SapBufferProcessing(impl_->sapBufWithTrash.get(), this);
  if (!impl_->sapBufferProcessing->Create()) {
    setState(Invalid);
    setErrorString(QString{"Unable to create SapBufferProcessing, reason: "} +
                   impl_->sapBufferProcessing->GetLastStatus());
    return false;
  }
  impl_->sapAcqDeviceToBuffer.reset(new SapAcqDeviceToBuf{
      impl_->sapDevice.get(), impl_->sapBufWithTrash.get(), XferCallback,
      impl_->sapBufferProcessing});
  if (!impl_->sapAcqDeviceToBuffer->Create()) {
    setState(Invalid);
    setErrorString(QString{"Unable to create SapAcqDeviceToBuf, reason: "} +
                   impl_->sapAcqDeviceToBuffer->GetLastStatus());
    return false;
  }
  // TODO figure out why i do SetAutoEmpty(false)
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
  if(resource()->state() != Resource::Initialised){
    return;
  }
  if (auto s = state(); s == Invalid || s == Playing) {
    return;
  }
  if(!impl_->sapAcqDeviceToBuffer->Grab()){
    setState(Invalid);
    setErrorString(QString{"Unable to grab, reason"} + impl_->sapAcqDeviceToBuffer->GetLastStatus());
    return;
  }
  setState(Playing);
}

void SaperaStream::stop() {
  if (auto s = state(); s == Invalid || s == Stopped) {
    return;
  }
  impl_->sapAcqDeviceToBuffer->Freeze();
  setState(Stopped);
}

}  // namespace MediaProvider
