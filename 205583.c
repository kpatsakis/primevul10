discrete_component_transfer_func (gint C, RsvgNodeComponentTransferFunc * user_data)
{
    gint k;

    if (!user_data->nbTableValues)
        return C;

    k = (C * user_data->nbTableValues) / 255;

    return user_data->tableValues[CLAMP (k, 0, user_data->nbTableValues - 1)];
}