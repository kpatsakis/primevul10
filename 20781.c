SProcXkbSetMap(ClientPtr client)
{
    REQUEST(xkbSetMapReq);

    swaps(&stuff->length);
    REQUEST_AT_LEAST_SIZE(xkbSetMapReq);
    swaps(&stuff->deviceSpec);
    swaps(&stuff->present);
    swaps(&stuff->flags);
    swaps(&stuff->totalSyms);
    swaps(&stuff->totalActs);
    swaps(&stuff->virtualMods);
    return ProcXkbSetMap(client);
}