  void InitializeDecoderSuccessfully() {
    decoder_->Initialize(
        NULL, NewExpectedStatusCB(PIPELINE_OK), NewStatisticsCB());
    message_loop_.RunAllPending();
  }
