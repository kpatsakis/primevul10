bool ethereum_getStandardERC20Amount(const EthereumSignTx *msg,
                                     void **tx_out_amount) {
  const ExchangeType *exchange = &msg->exchange_type;
  size_t size =
      exchange->signed_exchange_response.responseV2.deposit_amount.size;
  if (32 < size) return false;

  // Make sure the value in data_initial_chunk contains the correct number of
  // leading zeroes (as compared to what the exchange contract wants).
  char *value = (char *)msg->data_initial_chunk.bytes + 36;
  if (memcmp(value,
             "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
             "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00",
             32 - size) != 0)
    return false;

  *tx_out_amount = value + (32 - size);
  return true;
}