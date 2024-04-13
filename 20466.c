set_bound_node_opt_info(NodeOpt* opt, MinMax* plen)
{
  copy_mml(&(opt->exb.mmd),  plen);
  copy_mml(&(opt->expr.mmd), plen);
  copy_mml(&(opt->map.mmd),  plen);
}