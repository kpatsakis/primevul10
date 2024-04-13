SProcXkbSetNamedIndicator(ClientPtr client)
{
    REQUEST(xkbSetNamedIndicatorReq);

    swaps(&stuff->length);
    REQUEST_SIZE_MATCH(xkbSetNamedIndicatorReq);
    swaps(&stuff->deviceSpec);
    swaps(&stuff->ledClass);
    swaps(&stuff->ledID);
    swapl(&stuff->indicator);
    swaps(&stuff->virtualMods);
    swapl(&stuff->ctrls);
    return ProcXkbSetNamedIndicator(client);
}