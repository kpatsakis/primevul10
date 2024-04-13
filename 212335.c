static bool isAddLiquidityEthCall(const EthereumSignTx *msg) {
    if (memcmp(msg->data_initial_chunk.bytes, "\xf3\x05\xd7\x19", 4) == 0)
        return true;

    return false;
}