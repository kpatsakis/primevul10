rsvg_node_component_transfer_function_free (gpointer impl)
{
    RsvgNodeComponentTransferFunc *filter = impl;

    if (filter->nbTableValues)
        g_free (filter->tableValues);

    g_free (filter);
}