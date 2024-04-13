SProcXkbListComponents(ClientPtr client)
{
    REQUEST(xkbListComponentsReq);

    swaps(&stuff->length);
    REQUEST_AT_LEAST_SIZE(xkbListComponentsReq);
    swaps(&stuff->deviceSpec);
    swaps(&stuff->maxNames);
    return ProcXkbListComponents(client);
}