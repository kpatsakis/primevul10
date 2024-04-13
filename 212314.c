static void layoutEthereumFee(const uint8_t *value, uint32_t value_len,
                              const uint8_t *gas_price, uint32_t gas_price_len,
                              const uint8_t *gas_limit, uint32_t gas_limit_len,
                              bool is_token, char *out_str,
                              size_t out_str_len) {
  bignum256 val, gas;
  uint8_t pad_val[32];
  char tx_value[32];
  char gas_value[32];

  memzero(tx_value, sizeof(tx_value));
  memzero(gas_value, sizeof(gas_value));

  memset(pad_val, 0, sizeof(pad_val));
  memcpy(pad_val + (32 - gas_price_len), gas_price, gas_price_len);
  bn_read_be(pad_val, &val);

  memset(pad_val, 0, sizeof(pad_val));
  memcpy(pad_val + (32 - gas_limit_len), gas_limit, gas_limit_len);
  bn_read_be(pad_val, &gas);
  bn_multiply(&val, &gas, &secp256k1.prime);

  ethereumFormatAmount(&gas, NULL, chain_id, gas_value, sizeof(gas_value));

  memset(pad_val, 0, sizeof(pad_val));
  memcpy(pad_val + (32 - value_len), value, value_len);
  bn_read_be(pad_val, &val);

  if (bn_is_zero(&val)) {
    strcpy(tx_value, is_token ? _("the tokens") : _("the message"));
  } else {
    ethereumFormatAmount(&val, NULL, chain_id, tx_value, sizeof(tx_value));
  }

  if ((uint32_t)snprintf(
          out_str, out_str_len,
          _("Send %s from your wallet, paying up to %s for gas?"), tx_value,
          gas_value) >= out_str_len) {
    /*error detected.  Clear the buffer */
    memset(out_str, 0, out_str_len);
  }
}