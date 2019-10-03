#include "dalsa_lnx_stream.h"
#include <GenApi/GenApi.h>
#include <gevapi.h>
#include <QDebug>
#include "dalsa_lnx_buffer_processing.h"

namespace MediaProvider {

struct DalsaStream::Implementation {
  DalsaBufferProcessing* sapBufProcessing{};
  GEV_CAMERA_HANDLE handle{};
};

DalsaStream::DalsaStream(Resource* parent) : Stream(parent) {
  impl_.reset(new Implementation);
}

DalsaStream::~DalsaStream() {
  qDebug() << "DalsaStream::~DalsaStream()";
  stop();
}

void DalsaStream::setCameraHandle(void* handle) {
  impl_->handle = handle;
  impl_->sapBufProcessing = new DalsaBufferProcessing{impl_->handle, this};
  if (!impl_->sapBufProcessing->isInitialised()) {
    setState(Invalid);
    setErrorString("Unable to initialise buffer processing");
    return;
  }
  connect(impl_->sapBufProcessing, &DalsaBufferProcessing::newFrame, this,
          &DalsaStream::newFrame);
  connect(impl_->sapBufProcessing, &DalsaBufferProcessing::error, this,
          &DalsaStream::onBufferProcessingError);
}

void DalsaStream::start() {
  if (resource()->state() != Resource::Initialised) {
    return;
  }
  if (state() == Playing) {
    return;
  }
  if (impl_->handle) {
    if (GevStartTransfer(impl_->handle, -1) != GEVLIB_OK) {
      setState(Invalid);
      setErrorString("Unable to start image transfer");
      return;
    }
    impl_->sapBufProcessing->start(QThread::Priority::TimeCriticalPriority);
    setState(Playing);
  }
}

void DalsaStream::stop() {
  if (impl_->handle && impl_->sapBufProcessing->isRunning()) {
    impl_->sapBufProcessing->stop();
    impl_->sapBufProcessing->wait();
    GevStopTransfer(impl_->handle);
  }
  if (state() != Invalid) {
    setState(Stopped);
  }
}

void DalsaStream::onBufferProcessingError(QString errorStr) {
  impl_->sapBufProcessing->stop();
  setErrorString(errorStr);
  setState(Invalid);
}

}  // namespace MediaProvider
