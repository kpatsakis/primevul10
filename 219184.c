rfbClientSendString(rfbClientPtr cl, const char *reason)
{
    char *buf;
    int len = strlen(reason);

    rfbLog("rfbClientSendString(\"%s\")\n", reason);

    buf = (char *)malloc(4 + len);
    ((uint32_t *)buf)[0] = Swap32IfLE(len);
    memcpy(buf + 4, reason, len);

    if (rfbWriteExact(cl, buf, 4 + len) < 0)
        rfbLogPerror("rfbClientSendString: write");
    free(buf);

    rfbCloseClient(cl);
}