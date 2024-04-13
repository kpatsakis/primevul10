int ethereum_message_verify(const EthereumVerifyMessage *msg) {
  if (msg->signature.size != 65 || msg->address.size != 20) {
    fsm_sendFailure(FailureType_Failure_SyntaxError, _("Malformed data"));
    return 1;
  }

  uint8_t pubkey[65];
  uint8_t hash[32];

  ethereum_message_hash(msg->message.bytes, msg->message.size, hash);

  /* v should be 27, 28 but some implementations use 0,1.  We are
   * compatible with both.
   */
  uint8_t v = msg->signature.bytes[64];
  if (v >= 27) {
    v -= 27;
  }
  if (v >= 2 || ecdsa_recover_pub_from_sig(
                    &secp256k1, pubkey, msg->signature.bytes, hash, v) != 0) {
    return 2;
  }

  struct SHA3_CTX ctx;
  sha3_256_Init(&ctx);
  sha3_Update(&ctx, pubkey + 1, 64);
  keccak_Final(&ctx, hash);

  /* result are the least significant 160 bits */
  if (memcmp(msg->address.bytes, hash + 12, 20) != 0) {
    return 2;
  }
  return 0;
}