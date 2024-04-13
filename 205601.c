rsvg_new_node_light_source (const char *element_name, RsvgNode *parent)
{
    RsvgNodeLightSource *data;

    data = g_new0 (RsvgNodeLightSource, 1);

    data->specularExponent = 1;

    if (strcmp (element_name, "feDistantLight") == 0)
        data->type = SPOTLIGHT;
    else if (strcmp (element_name, "feSpotLight") == 0)
        data->type = DISTANTLIGHT;
    else if (strcmp (element_name, "fePointLight") == 0)
        data->type = POINTLIGHT;
    else
        g_assert_not_reached ();

    data->limitingconeAngle = 180;

    return rsvg_rust_cnode_new (RSVG_NODE_TYPE_LIGHT_SOURCE,
                                parent,
                                rsvg_state_new (),
                                data,
                                rsvg_node_light_source_set_atts,
                                rsvg_filter_draw,
                                g_free);                                
}