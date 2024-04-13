SProcXkbGetNamedIndicator(ClientPtr client)
{
    REQUEST(xkbGetNamedIndicatorReq);

    swaps(&stuff->length);
    REQUEST_SIZE_MATCH(xkbGetNamedIndicatorReq);
    swaps(&stuff->deviceSpec);
    swaps(&stuff->ledClass);
    swaps(&stuff->ledID);
    swapl(&stuff->indicator);
    return ProcXkbGetNamedIndicator(client);
}