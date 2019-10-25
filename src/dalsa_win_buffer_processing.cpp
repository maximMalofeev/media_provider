#include "dalsa_win_buffer_processing.h"
#include <QDebug>

namespace MediaProvider {

DalsaBufferProcessing::DalsaBufferProcessing(SapBuffer* sapBuffer, QObject* parent)
    : SapProcessing(sapBuffer), QObject(parent) {
  SetAutoEmpty(true);
  auto bytesPerPixel = sapBuffer->GetBytesPerPixel();
  frameSize = sapBuffer->GetWidth() * sapBuffer->GetHeight() * bytesPerPixel;
}

BOOL DalsaBufferProcessing::Run() {
  auto buf = GetBuffer();
  UINT64 timestamp{};

  QImage::Format format{};
  if (buf->GetFormat() == SapFormatMono8) {
    format = QImage::Format_Grayscale8;
  } else if(buf->GetFormat() == SapFormatRGB888) {
    format = QImage::Format_RGB888;
  } else{
    return false;
  }

  if (!buf->GetDeviceTimeStamp(&timestamp)) {
    return false;
  }

  uchar* frame = new uchar[frameSize]{0};

  if (!buf->Read(0, frameSize, frame)) {
    return false;
  }

  QImage newImg{frame,
                buf->GetWidth(),
                buf->GetHeight(),
                format,
                [](void* p) { delete static_cast<uchar*>(p); },
                frame};
  emit newFrame(newImg, timestamp / 1000);

  return true;
}

}  // namespace MediaProvider
