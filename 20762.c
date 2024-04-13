SProcXkbGetIndicatorMap(ClientPtr client)
{
    REQUEST(xkbGetIndicatorMapReq);

    swaps(&stuff->length);
    REQUEST_SIZE_MATCH(xkbGetIndicatorMapReq);
    swaps(&stuff->deviceSpec);
    swapl(&stuff->which);
    return ProcXkbGetIndicatorMap(client);
}