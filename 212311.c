bool ethereum_isStandardERC20Transfer(const EthereumSignTx *msg) {
  if (msg->has_to && msg->to.size == 20 && msg->value.size == 0 &&
      msg->data_initial_chunk.size == 68 &&
      memcmp(msg->data_initial_chunk.bytes,
             "\xa9\x05\x9c\xbb\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00",
             16) == 0) {
    return true;
  }
  return false;
}