SProcXkbGetDeviceInfo(ClientPtr client)
{
    REQUEST(xkbGetDeviceInfoReq);

    swaps(&stuff->length);
    REQUEST_SIZE_MATCH(xkbGetDeviceInfoReq);
    swaps(&stuff->deviceSpec);
    swaps(&stuff->wanted);
    swaps(&stuff->ledClass);
    swaps(&stuff->ledID);
    return ProcXkbGetDeviceInfo(client);
}