compile_length_option_node(EnclosureNode* node, regex_t* reg)
{
  int tlen;
  OnigOptionType prev = reg->options;

  reg->options = node->o.options;
  tlen = compile_length_tree(NODE_ENCLOSURE_BODY(node), reg);
  reg->options = prev;

  return tlen;
}