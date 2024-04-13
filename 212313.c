void ethereumFormatAmount(const bignum256 *amnt, const TokenType *token,
                          uint32_t cid, char *buf, int buflen) {
  bignum256 bn1e9;
  bn_read_uint32(1000000000, &bn1e9);
  const char *suffix = NULL;
  int decimals = 18;
  if (token == UnknownToken) {
    strlcpy(buf, "Unknown token value", buflen);
    return;
  } else if (token != NULL) {
    suffix = token->ticker;
    decimals = token->decimals;
  } else if (bn_is_less(amnt, &bn1e9)) {
    suffix = " Wei";
    decimals = 0;
  } else {
    if (tx_type == 1 || tx_type == 6) {
      suffix = " WAN";
    } else {
      // constants from trezor-common/defs/ethereum/networks.json
      switch (cid) {
        case 1:
          suffix = " ETH";
          break;  // Ethereum
        case 2:
          suffix = " EXP";
          break;  // Expanse
        case 3:
          suffix = " tROP";
          break;  // Ethereum Testnet Ropsten
        case 4:
          suffix = " tRIN";
          break;  // Ethereum Testnet Rinkeby
        case 8:
          suffix = " UBQ";
          break;  // UBIQ
        case 20:
          suffix = " EOSC";
          break;  // EOS Classic
        case 28:
          suffix = " ETSC";
          break;  // Ethereum Social
        case 30:
          suffix = " RBTC";
          break;  // RSK
        case 31:
          suffix = " tRBTC";
          break;  // RSK Testnet
        case 42:
          suffix = " tKOV";
          break;  // Ethereum Testnet Kovan
        case 61:
          suffix = " ETC";
          break;  // Ethereum Classic
        case 62:
          suffix = " tETC";
          break;  // Ethereum Classic Testnet
        case 64:
          suffix = " ELLA";
          break;  // Ellaism
        case 820:
          suffix = " CLO";
          break;  // Callisto
        case 1987:
          suffix = " EGEM";
          break;  // EtherGem
        default:
          suffix = " UNKN";
          break;  // unknown chain
      }
    }
  }
  bn_format(amnt, NULL, suffix, decimals, 0, false, buf, buflen);
}