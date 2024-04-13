static bool isRemoveLiquidityEthCall(const EthereumSignTx *msg) {
    if (memcmp(msg->data_initial_chunk.bytes, "\x02\x75\x1c\xec", 4) == 0)
        return true;

    return false;
}