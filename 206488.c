ssize_t Http2Session::OnMaxFrameSizePadding(size_t frameLen,
                                            size_t maxPayloadLen) {
  Debug(this, "using max frame size padding: %d", maxPayloadLen);
  return maxPayloadLen;
}