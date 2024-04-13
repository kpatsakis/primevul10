int Jsi_HexStr(const uchar *data, int len, Jsi_DString *dStr, bool decode) {
    int olen = (decode?(len/2+1):(len*2+1));
    Jsi_DSSetLength(dStr, olen);
    if (!decode)
        return jsi_FromHexStr((const char*)data, (uchar*)Jsi_DSValue(dStr));
    jsi_ToHexStr((const uchar*)data, len, Jsi_DSValue(dStr));
    return olen-1;
}