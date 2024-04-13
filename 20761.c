SProcXkbGetControls(ClientPtr client)
{
    REQUEST(xkbGetControlsReq);

    swaps(&stuff->length);
    REQUEST_SIZE_MATCH(xkbGetControlsReq);
    swaps(&stuff->deviceSpec);
    return ProcXkbGetControls(client);
}