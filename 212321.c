static void send_signature(void) {
  uint8_t hash[32], sig[64];
  uint8_t v;
  layoutProgress(_("Signing"), 1000);

  /* eip-155 replay protection */
  if (chain_id) {
    /* hash v=chain_id, r=0, s=0 */
    hash_rlp_number(chain_id);
    hash_rlp_length(0, 0);
    hash_rlp_length(0, 0);
  }

  keccak_Final(&keccak_ctx, hash);
  if (ecdsa_sign_digest(&secp256k1, privkey, hash, sig, &v,
                        ethereum_is_canonic) != 0) {
    fsm_sendFailure(FailureType_Failure_Other, "Signing failed");
    ethereum_signing_abort();
    return;
  }

  memzero(privkey, sizeof(privkey));

  /* Send back the result */
  msg_tx_request.has_data_length = false;

  msg_tx_request.has_signature_v = true;
  if (chain_id > MAX_CHAIN_ID) {
    msg_tx_request.signature_v = v;
  } else if (chain_id) {
    msg_tx_request.signature_v = v + 2 * chain_id + 35;
  } else {
    msg_tx_request.signature_v = v + 27;
  }

  msg_tx_request.has_signature_r = true;
  msg_tx_request.signature_r.size = 32;
  memcpy(msg_tx_request.signature_r.bytes, sig, 32);

  msg_tx_request.has_signature_s = true;
  msg_tx_request.signature_s.size = 32;
  memcpy(msg_tx_request.signature_s.bytes, sig + 32, 32);

  // KeepKey custom (for the KeepKey Client)
  msg_tx_request.has_hash = true;
  msg_tx_request.hash.size = sizeof(msg_tx_request.hash.bytes);
  memcpy(msg_tx_request.hash.bytes, hash, msg_tx_request.hash.size);
  msg_tx_request.has_signature_der = true;
  msg_tx_request.signature_der.size =
      ecdsa_sig_to_der(sig, msg_tx_request.signature_der.bytes);

  msg_write(MessageType_MessageType_EthereumTxRequest, &msg_tx_request);

  ethereum_signing_abort();
}