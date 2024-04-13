SProcXkbGetState(ClientPtr client)
{
    REQUEST(xkbGetStateReq);

    swaps(&stuff->length);
    REQUEST_SIZE_MATCH(xkbGetStateReq);
    swaps(&stuff->deviceSpec);
    return ProcXkbGetState(client);
}