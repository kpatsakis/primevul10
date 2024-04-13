bool zx_confirmZxLiquidTx(uint32_t data_total, const EthereumSignTx *msg) {
    (void)data_total;
    const TokenType *token;
    char constr1[40], constr2[40], *arStr = "";
    uint8_t *tokenAddress, *deadlineBytes;
    bignum256 tokenAmount, tokenMinAmount, ethMinAmount;
    uint64_t deadline;

    if (isAddLiquidityEthCall(msg)) {
        arStr = "uniswap add liquidity";
    } else if (isRemoveLiquidityEthCall(msg)) {
        arStr = "uniswap remove liquidity";
    } else {
        return false;
    }

    tokenAddress = (uint8_t *)(msg->data_initial_chunk.bytes + 4 + 32 - 20);
    token = tokenByChainAddress(msg->chain_id, tokenAddress);
    bn_from_bytes(msg->data_initial_chunk.bytes + 4 + 32, 32, &tokenAmount);
    bn_from_bytes(msg->data_initial_chunk.bytes + 4 + 2*32, 32, &tokenMinAmount);
    bn_from_bytes(msg->data_initial_chunk.bytes + 4 + 3*32, 32, &ethMinAmount);
    deadlineBytes = (uint8_t *)(msg->data_initial_chunk.bytes + 4 + 6*32 - 8);
    deadline = ((uint64_t)deadlineBytes[0] << 8*7) |
               ((uint64_t)deadlineBytes[1] << 8*6) |
               ((uint64_t)deadlineBytes[2] << 8*5) |
               ((uint64_t)deadlineBytes[3] << 8*4) |
               ((uint64_t)deadlineBytes[4] << 8*3) |
               ((uint64_t)deadlineBytes[5] << 8*2) |
               ((uint64_t)deadlineBytes[6] << 8*1) |
               ((uint64_t)deadlineBytes[7]);
        
    char tokbuf[32];
    ethereumFormatAmount(&tokenAmount, token, msg->chain_id, tokbuf, sizeof(tokbuf));
    snprintf(constr1, 32, "%s", tokbuf);
    ethereumFormatAmount(&tokenMinAmount, token, msg->chain_id, tokbuf, sizeof(tokbuf));
    snprintf(constr2, 32, "%s", tokbuf);
    confirm(ButtonRequestType_ButtonRequest_ConfirmOutput, arStr,
                 "%s\nMinimum %s", constr1, constr2);
    if (!confirmFromAccountMatch(msg, arStr)) {
        return false;
    }
    
    ethereumFormatAmount(&ethMinAmount, NULL, msg->chain_id, tokbuf, sizeof(tokbuf));
    snprintf(constr1, 32, "%s", tokbuf);
    confirm(ButtonRequestType_ButtonRequest_ConfirmOutput, arStr,
                 "Minimum %s", constr1);

    confirm(ButtonRequestType_ButtonRequest_ConfirmOutput, arStr,
                 "Deadline %s", ctime((const time_t *)&deadline));
    
    return true;
}