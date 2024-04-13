void bn_from_bytes(const uint8_t *value, size_t value_len, bignum256 *val) {
  uint8_t pad_val[32];
  memset(pad_val, 0, sizeof(pad_val));
  memcpy(pad_val + (32 - value_len), value, value_len);
  bn_read_be(pad_val, val);
  memzero(pad_val, sizeof(pad_val));
}