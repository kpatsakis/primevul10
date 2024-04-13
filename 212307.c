static bool ethereum_signing_check(EthereumSignTx *msg) {
  if (!msg->has_gas_price || !msg->has_gas_limit) {
    return false;
  }

  if (msg->to.size != 20 && msg->to.size != 0) {
    /* Address has wrong length */
    return false;
  }

  // sending transaction to address 0 (contract creation) without a data field
  if (msg->to.size == 0 && (!msg->has_data_length || msg->data_length == 0)) {
    return false;
  }

  if (msg->gas_price.size + msg->gas_limit.size > 30) {
    // sanity check that fee doesn't overflow
    return false;
  }

  return true;
}