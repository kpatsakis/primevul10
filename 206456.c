ssize_t Http2Session::OnDWordAlignedPadding(size_t frameLen,
                                            size_t maxPayloadLen) {
  size_t r = (frameLen + 9) % 8;
  if (r == 0) return frameLen;  // If already a multiple of 8, return.

  size_t pad = frameLen + (8 - r);

  // If maxPayloadLen happens to be less than the calculated pad length,
  // use the max instead, even tho this means the frame will not be
  // aligned.
  pad = std::min(maxPayloadLen, pad);
  Debug(this, "using frame size padding: %d", pad);
  return pad;
}