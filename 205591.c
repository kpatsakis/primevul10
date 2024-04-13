node_is_filter_primitive (RsvgNode *node)
{
    RsvgNodeType type = rsvg_node_get_type (node);

    return type > RSVG_NODE_TYPE_FILTER_PRIMITIVE_FIRST && type < RSVG_NODE_TYPE_FILTER_PRIMITIVE_LAST;
}