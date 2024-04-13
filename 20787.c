SProcXkbPerClientFlags(ClientPtr client)
{
    REQUEST(xkbPerClientFlagsReq);

    swaps(&stuff->length);
    REQUEST_SIZE_MATCH(xkbPerClientFlagsReq);
    swaps(&stuff->deviceSpec);
    swapl(&stuff->change);
    swapl(&stuff->value);
    swapl(&stuff->ctrlsToChange);
    swapl(&stuff->autoCtrls);
    swapl(&stuff->autoCtrlValues);
    return ProcXkbPerClientFlags(client);
}