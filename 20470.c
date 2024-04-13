compile_option_node(EnclosureNode* node, regex_t* reg, ScanEnv* env)
{
  int r;
  OnigOptionType prev = reg->options;

  reg->options = node->o.options;
  r = compile_tree(NODE_ENCLOSURE_BODY(node), reg, env);
  reg->options = prev;

  return r;
}