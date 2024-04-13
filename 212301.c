void ethereum_signing_init(EthereumSignTx *msg, const HDNode *node,
                           bool needs_confirm) {
  ethereum_signing = true;
  sha3_256_Init(&keccak_ctx);

  memset(&msg_tx_request, 0, sizeof(EthereumTxRequest));
  /* set fields to 0, to avoid conditions later */
  if (!msg->has_value) msg->value.size = 0;
  if (!msg->has_data_initial_chunk) msg->data_initial_chunk.size = 0;
  if (!msg->has_to) msg->to.size = 0;
  if (!msg->has_nonce) msg->nonce.size = 0;

  /* eip-155 chain id */
  if (msg->has_chain_id) {
    if (msg->chain_id < 1) {
      fsm_sendFailure(FailureType_Failure_SyntaxError,
                      _("Chain Id out of bounds"));
      ethereum_signing_abort();
      return;
    }
    chain_id = msg->chain_id;
  } else {
    chain_id = 0;
  }

  /* Wanchain txtype */
  if (msg->has_tx_type) {
    if (msg->tx_type == 1 || msg->tx_type == 6) {
      tx_type = msg->tx_type;
    } else {
      fsm_sendFailure(FailureType_Failure_SyntaxError,
                      _("Txtype out of bounds"));
      ethereum_signing_abort();
      return;
    }
  } else {
    tx_type = 0;
  }

  if (msg->has_data_length && msg->data_length > 0) {
    if (!msg->has_data_initial_chunk || msg->data_initial_chunk.size == 0) {
      fsm_sendFailure(FailureType_Failure_Other,
                      _("Data length provided, but no initial chunk"));
      ethereum_signing_abort();
      return;
    }
    /* Our encoding only supports transactions up to 2^24 bytes.  To
     * prevent exceeding the limit we use a stricter limit on data length.
     */
    if (msg->data_length > 16000000) {
      fsm_sendFailure(FailureType_Failure_SyntaxError,
                      _("Data length exceeds limit"));
      ethereum_signing_abort();
      return;
    }
    data_total = msg->data_length;
  } else {
    data_total = 0;
  }
  if (msg->data_initial_chunk.size > data_total) {
    fsm_sendFailure(FailureType_Failure_Other,
                    _("Invalid size of initial chunk"));
    ethereum_signing_abort();
    return;
  }

  const TokenType *token = NULL;

  // safety checks
  if (!ethereum_signing_check(msg)) {
    fsm_sendFailure(FailureType_Failure_SyntaxError, _("Safety check failed"));
    ethereum_signing_abort();
    return;
  }

  bool data_needs_confirm = true;
  if (ethereum_contractHandled(data_total, msg, node)) {
    if (!ethereum_contractConfirmed(data_total, msg, node)) {
      fsm_sendFailure(FailureType_Failure_ActionCancelled,
                      "Signing cancelled by user");
      ethereum_signing_abort();
      return;
    }
    needs_confirm = false;
    data_needs_confirm = false;
  }

  // Detect THORChain transaction data in memo
  if (ethereum_isThorchainTx(msg)) {
    if (token == NULL && data_total > 0 && data_needs_confirm) {
      char swap_data[256] = {'\0'};
      uint8_t swap_data_len = ethereum_extractThorchainData(msg, swap_data);
      if (!thorchain_parseConfirmMemo(swap_data, swap_data_len)) {
        fsm_sendFailure(FailureType_Failure_Other, _("Malformed THORChain swap data"));
        ethereum_signing_abort();
        return;
      }
      needs_confirm = false;
      data_needs_confirm = false;
    }
  }

  // detect ERC-20 token
  if (data_total == 68 && ethereum_isStandardERC20Transfer(msg)) {
    token = tokenByChainAddress(chain_id, msg->to.bytes);
  }

  bool is_approve = false;
  if (data_total == 68 && ethereum_isStandardERC20Approve(msg)) {
    token = tokenByChainAddress(chain_id, msg->to.bytes);
    is_approve = true;
  }

  char confirm_body_message[BODY_CHAR_MAX];
  if (needs_confirm) {
    memset(confirm_body_message, 0, sizeof(confirm_body_message));
    if (token != NULL) {
      layoutEthereumConfirmTx(
          msg->data_initial_chunk.bytes + 16, 20,
          msg->data_initial_chunk.bytes + 36, 32, token, confirm_body_message,
          sizeof(confirm_body_message), /*approve=*/is_approve);
    } else {
      layoutEthereumConfirmTx(msg->to.bytes, msg->to.size, msg->value.bytes,
                              msg->value.size, NULL, confirm_body_message,
                              sizeof(confirm_body_message), /*approve=*/false);
    }
    bool is_transfer = msg->address_type == OutputAddressType_TRANSFER;
    const char *title;
    ButtonRequestType BRT;
    if (is_approve) {
      title = "Approve";
      BRT = ButtonRequestType_ButtonRequest_ConfirmOutput;
    } else if (is_transfer) {
      title = "Transfer";
      BRT = ButtonRequestType_ButtonRequest_ConfirmTransferToAccount;
    } else {
      title = "Send";
      BRT = ButtonRequestType_ButtonRequest_ConfirmOutput;
    }
    if (!confirm(BRT, title, "%s", confirm_body_message)) {
      fsm_sendFailure(FailureType_Failure_ActionCancelled,
                      "Signing cancelled by user");
      ethereum_signing_abort();
      return;
    }
  }

  memset(confirm_body_message, 0, sizeof(confirm_body_message));
  if (token == NULL && data_total > 0 && data_needs_confirm) {
    // KeepKey custom: warn the user that they're trying to do something
    // that is potentially dangerous. People (generally) aren't great at
    // parsing raw transaction data, and we can't effectively show them
    // what they're about to do in the general case.
    if (!storage_isPolicyEnabled("AdvancedMode")) {
      (void)review(
          ButtonRequestType_ButtonRequest_Other, "Warning",
          "Signing of arbitrary ETH contract data is recommended only for "
          "experienced users. Enable 'AdvancedMode' policy to dismiss.");
    }

    layoutEthereumData(msg->data_initial_chunk.bytes,
                       msg->data_initial_chunk.size, data_total,
                       confirm_body_message, sizeof(confirm_body_message));
    if (!confirm(ButtonRequestType_ButtonRequest_ConfirmOutput,
                 "Confirm Ethereum Data", "%s", confirm_body_message)) {
      fsm_sendFailure(FailureType_Failure_ActionCancelled, NULL);
      ethereum_signing_abort();
      return;
    }
  }

  if (is_approve) {
    token = NULL;
  }

  memset(confirm_body_message, 0, sizeof(confirm_body_message));
  layoutEthereumFee(msg->value.bytes, msg->value.size, msg->gas_price.bytes,
                    msg->gas_price.size, msg->gas_limit.bytes,
                    msg->gas_limit.size, token != NULL, confirm_body_message,
                    sizeof(confirm_body_message));
  if (!confirm(ButtonRequestType_ButtonRequest_SignTx, "Transaction", "%s",
               confirm_body_message)) {
    fsm_sendFailure(FailureType_Failure_ActionCancelled,
                    "Signing cancelled by user");
    ethereum_signing_abort();
    return;
  }

  /* Stage 1: Calculate total RLP length */
  uint32_t rlp_length = 0;
  layoutProgress(_("Signing"), 0);

  rlp_length += rlp_calculate_length(msg->nonce.size, msg->nonce.bytes[0]);
  rlp_length +=
      rlp_calculate_length(msg->gas_price.size, msg->gas_price.bytes[0]);
  rlp_length +=
      rlp_calculate_length(msg->gas_limit.size, msg->gas_limit.bytes[0]);
  rlp_length += rlp_calculate_length(msg->to.size, msg->to.bytes[0]);
  rlp_length += rlp_calculate_length(msg->value.size, msg->value.bytes[0]);
  rlp_length +=
      rlp_calculate_length(data_total, msg->data_initial_chunk.bytes[0]);
  if (tx_type) {
    rlp_length += rlp_calculate_number_length(tx_type);
  }
  if (chain_id) {
    rlp_length += rlp_calculate_number_length(chain_id);
    rlp_length += rlp_calculate_length(0, 0);
    rlp_length += rlp_calculate_length(0, 0);
  }

  /* Stage 2: Store header fields */
  hash_rlp_list_length(rlp_length);
  layoutProgress(_("Signing"), 100);

  if (tx_type) {
    hash_rlp_number(tx_type);
  }
  hash_rlp_field(msg->nonce.bytes, msg->nonce.size);
  hash_rlp_field(msg->gas_price.bytes, msg->gas_price.size);
  hash_rlp_field(msg->gas_limit.bytes, msg->gas_limit.size);
  hash_rlp_field(msg->to.bytes, msg->to.size);
  hash_rlp_field(msg->value.bytes, msg->value.size);
  hash_rlp_length(data_total, msg->data_initial_chunk.bytes[0]);
  hash_data(msg->data_initial_chunk.bytes, msg->data_initial_chunk.size);
  data_left = data_total - msg->data_initial_chunk.size;

  memcpy(privkey, node->private_key, 32);

  if (data_left > 0) {
    send_request_chunk();
  } else {
    send_signature();
  }
}