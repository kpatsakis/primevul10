bool ethereum_getStandardERC20Recipient(const EthereumSignTx *msg,
                                        char *address, size_t len) {
  if (len < 2 * 20 + 1) return false;

  data2hex(msg->data_initial_chunk.bytes + 16, 20, address);
  return true;
}