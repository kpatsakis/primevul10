  void Reset() {
    decoder_->Reset(NewExpectedClosure());
    message_loop_.RunAllPending();
  }
