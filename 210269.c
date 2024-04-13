Jsi_Value* jsi_ValueMakeBlobDup(Jsi_Interp *interp, Jsi_Value **ret, unsigned char *s, int len) {
    if (len<0) len = Jsi_Strlen((char*)s);
    uchar *dp = (uchar*)Jsi_Malloc(len+1);
    memcpy(dp, s, len);
    dp[len] = 0;
    return Jsi_ValueMakeBlob(interp, ret, dp, len);
}