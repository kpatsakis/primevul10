bool ethereum_getStandardERC20Coin(const EthereumSignTx *msg, CoinType *coin) {
  const CoinType *found =
      coinByChainAddress(msg->has_chain_id ? msg->chain_id : 1, msg->to.bytes);
  if (found) {
    memcpy(coin, found, sizeof(*coin));
    return true;
  }

  const TokenType *token =
      tokenByChainAddress(msg->has_chain_id ? msg->chain_id : 1, msg->to.bytes);
  if (token == UnknownToken) return false;

  coinFromToken(coin, token);
  return true;
}