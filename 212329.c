bool ethereum_isThorchainTx(const EthereumSignTx *msg) {
  if (msg->has_to && msg->to.size == 20 &&
      memcmp(msg->data_initial_chunk.bytes,
             "\x1f\xec\xe7\xb4\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", 
             16) == 0) {
    return true;
  }
  return false;
}