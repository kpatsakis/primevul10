table_component_transfer_func (gint C, RsvgNodeComponentTransferFunc * user_data)
{
    guint k;
    gint vk, vk1, distancefromlast;
    guint num_values;

    if (!user_data->nbTableValues)
        return C;

    num_values = user_data->nbTableValues;

    k = (C * (num_values - 1)) / 255;

    vk = user_data->tableValues[MIN (k, num_values - 1)];
    vk1 = user_data->tableValues[MIN (k + 1, num_values - 1)];

    distancefromlast = (C * (user_data->nbTableValues - 1)) - k * 255;

    return vk + distancefromlast * (vk1 - vk) / 255;
}