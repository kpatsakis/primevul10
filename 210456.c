ACTION_P3(CreateDataBufferFromCapture, decoder, vc_impl, data_buffer_number) {
  for (int i = 0; i < data_buffer_number; i++) {
    media::VideoCapture::VideoFrameBuffer* buffer;
    buffer = new media::VideoCapture::VideoFrameBuffer();
    buffer->width = arg1.width;
    buffer->height = arg1.height;
    int length = buffer->width * buffer->height * 3 / 2;
    buffer->memory_pointer = new uint8[length];
    buffer->buffer_size = length;
    decoder->OnBufferReady(vc_impl, buffer);
  }
}
