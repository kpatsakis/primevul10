rfbClientConnFailed(rfbClientPtr cl,
                    char *reason)
{
    char *buf;
    int len = strlen(reason);

    rfbLog("rfbClientConnFailed(\"%s\")\n", reason);

    buf = (char *)malloc(8 + len);
    ((uint32_t *)buf)[0] = 0;
    ((uint32_t *)buf)[1] = Swap32IfLE(len);
    memcpy(buf + 8, reason, len);

    if (WriteExact(cl, buf, 8 + len) < 0)
        rfbLogPerror("rfbClientConnFailed: write");
    free(buf);

    rfbCloseClient(cl);
}