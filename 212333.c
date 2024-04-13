static bool confirmFromAccountMatch(const EthereumSignTx *msg, char *addremStr) {
    // Determine withdrawal address
    char addressStr[43] = {'0', 'x', '\0'};
    char *fromSrc;
    uint8_t *fromAddress;
    uint8_t addressBytes[20];

    HDNode *node = zx_getDerivedNode(SECP256K1_NAME, msg->address_n,
                                    msg->address_n_count, NULL);
    if (!node) return false;

    if (!hdnode_get_ethereum_pubkeyhash(node, addressBytes)) {
        memzero(node, sizeof(*node));
    }

    fromAddress = (uint8_t *)(msg->data_initial_chunk.bytes + 4 + 5*32 - 20);

    if (memcmp(fromAddress, addressBytes, 20) == 0) {
        fromSrc = "self";
    } else {
        fromSrc = "NOT this wallet";
    }

    for (uint32_t ctr=0; ctr<20; ctr++) {
        snprintf(&addressStr[2+ctr*2], 3, "%02x", fromAddress[ctr]);
    }

    if (!confirm(ButtonRequestType_ButtonRequest_ConfirmOutput, addremStr,
                  "Confirming ETH address is %s: %s", fromSrc, addressStr)) {
       return false;
    }
    return true;
}