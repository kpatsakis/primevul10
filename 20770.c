SProcXkbSetGeometry(ClientPtr client)
{
    REQUEST(xkbSetGeometryReq);

    swaps(&stuff->length);
    REQUEST_AT_LEAST_SIZE(xkbSetGeometryReq);
    swaps(&stuff->deviceSpec);
    swapl(&stuff->name);
    swaps(&stuff->widthMM);
    swaps(&stuff->heightMM);
    swaps(&stuff->nProperties);
    swaps(&stuff->nColors);
    swaps(&stuff->nDoodads);
    swaps(&stuff->nKeyAliases);
    return ProcXkbSetGeometry(client);
}