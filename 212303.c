static void layoutEthereumConfirmTx(const uint8_t *to, uint32_t to_len,
                                    const uint8_t *value, uint32_t value_len,
                                    const TokenType *token, char *out_str,
                                    size_t out_str_len, bool approve) {
  bignum256 val;
  uint8_t pad_val[32];
  memset(pad_val, 0, sizeof(pad_val));
  memcpy(pad_val + (32 - value_len), value, value_len);
  bn_read_be(pad_val, &val);

  char amount[32];
  if (token == NULL) {
    if (bn_is_zero(&val)) {
      strcpy(amount, _("message"));
    } else {
      ethereumFormatAmount(&val, NULL, chain_id, amount, sizeof(amount));
    }
  } else {
    ethereumFormatAmount(&val, token, chain_id, amount, sizeof(amount));
  }

  char addr[43] = "0x";
  if (to_len) {
    ethereum_address_checksum(to, addr + 2, false, chain_id);
  }

  bool approve_all =
      approve && value_len == 32 &&
      memcmp(value, "\xff\xff\xff\xff\xff\xff\xff\xff", 8) == 0 &&
      memcmp(value + 8, "\xff\xff\xff\xff\xff\xff\xff\xff", 8) == 0 &&
      memcmp(value + 16, "\xff\xff\xff\xff\xff\xff\xff\xff", 8) == 0 &&
      memcmp(value + 24, "\xff\xff\xff\xff\xff\xff\xff\xff", 8) == 0;

  const char *address = addr;
  if (to_len && makerdao_isOasisDEXAddress(to, chain_id)) {
    address = "OasisDEX";
  }

  int cx;
  if (approve && bn_is_zero(&val) && token) {
    cx = snprintf(out_str, out_str_len, "Remove ability for %s to withdraw %s?",
                  address, token->ticker + 1);
  } else if (approve_all) {
    cx = snprintf(out_str, out_str_len,
                  "Unlock full %s balance for withdrawal by %s?",
                  token->ticker + 1, address);
  } else if (approve) {
    cx = snprintf(out_str, out_str_len, "Approve withdrawal of up to %s by %s?",
                  amount, address);
  } else {
    cx = snprintf(out_str, out_str_len, "Send %s to %s", amount,
                  to_len ? address : "new contract?");
  }

  if (out_str_len <= (size_t)cx) {
    /*error detected. Clear the buffer */
    memset(out_str, 0, out_str_len);
  }
}