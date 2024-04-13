SProcXkbSetNames(ClientPtr client)
{
    REQUEST(xkbSetNamesReq);

    swaps(&stuff->length);
    REQUEST_AT_LEAST_SIZE(xkbSetNamesReq);
    swaps(&stuff->deviceSpec);
    swaps(&stuff->virtualMods);
    swapl(&stuff->which);
    swapl(&stuff->indicators);
    swaps(&stuff->totalKTLevelNames);
    return ProcXkbSetNames(client);
}