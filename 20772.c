SProcXkbGetIndicatorState(ClientPtr client)
{
    REQUEST(xkbGetIndicatorStateReq);

    swaps(&stuff->length);
    REQUEST_SIZE_MATCH(xkbGetIndicatorStateReq);
    swaps(&stuff->deviceSpec);
    return ProcXkbGetIndicatorState(client);
}