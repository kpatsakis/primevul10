SProcXkbSetDeviceInfo(ClientPtr client)
{
    REQUEST(xkbSetDeviceInfoReq);

    swaps(&stuff->length);
    REQUEST_AT_LEAST_SIZE(xkbSetDeviceInfoReq);
    swaps(&stuff->deviceSpec);
    swaps(&stuff->change);
    swaps(&stuff->nDeviceLedFBs);
    return ProcXkbSetDeviceInfo(client);
}