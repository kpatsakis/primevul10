SProcXkbBell(ClientPtr client)
{
    REQUEST(xkbBellReq);

    swaps(&stuff->length);
    REQUEST_SIZE_MATCH(xkbBellReq);
    swaps(&stuff->deviceSpec);
    swaps(&stuff->bellClass);
    swaps(&stuff->bellID);
    swapl(&stuff->name);
    swapl(&stuff->window);
    swaps(&stuff->pitch);
    swaps(&stuff->duration);
    return ProcXkbBell(client);
}