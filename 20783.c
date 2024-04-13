SProcXkbSetIndicatorMap(ClientPtr client)
{
    REQUEST(xkbSetIndicatorMapReq);

    swaps(&stuff->length);
    REQUEST_AT_LEAST_SIZE(xkbSetIndicatorMapReq);
    swaps(&stuff->deviceSpec);
    swapl(&stuff->which);
    return ProcXkbSetIndicatorMap(client);
}