#include "sapera_lnx_stream.h"
#include <GenApi/GenApi.h>
#include <gevapi.h>
#include "sapera_lnx_buffer_processing.h"
#include <QDebug>

namespace MediaProvider {

struct SaperaStream::Implementation {
  SapBufferProcessing* sapBufProcessing{};
  GEV_CAMERA_HANDLE handle{};
};

SaperaStream::SaperaStream(Resource* parent)
    : Stream(parent) {
  impl_.reset(new Implementation);
}

SaperaStream::~SaperaStream() {
  qDebug() << "SaperaStream::~SaperaStream()";
  stop();
}

void SaperaStream::setCameraHandle(void *handle)
{
  impl_->handle = handle;
  impl_->sapBufProcessing = new SapBufferProcessing{impl_->handle, this};
  if(!impl_->sapBufProcessing->isInitialised()){
    setState(Invalid);
    setErrorString("Unable to initialise buffer processing");
    return;
  }
  connect(impl_->sapBufProcessing, &SapBufferProcessing::newFrame, this,
          &SaperaStream::newFrame);
  connect(impl_->sapBufProcessing, &SapBufferProcessing::error, this,
          &SaperaStream::onBufferProcessingError);
}

void SaperaStream::start() {
  if(state() == Playing){
    return;
  }
  if (GevStartImageTransfer(impl_->handle, -1) != GEVLIB_OK) {
    setState(Invalid);
    setErrorString("Unable to start image transfer");
    return;
  }
  impl_->sapBufProcessing->start(QThread::Priority::TimeCriticalPriority);
  setState(Playing);
}

void SaperaStream::stop() {
  if(impl_->sapBufProcessing->isRunning()){
    impl_->sapBufProcessing->stop();
    impl_->sapBufProcessing->wait();
    GevStopImageTransfer(impl_->handle);
  }
  if (state() != Stopped) {
    setState(Stopped);
  }
}

void SaperaStream::onBufferProcessingError(QString errorStr) {
  impl_->sapBufProcessing->stop();
  setErrorString(errorStr);
  setState(Invalid);
}

}  // namespace MediaProvider
