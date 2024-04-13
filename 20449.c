clear_node_opt_info(NodeOpt* opt)
{
  clear_mml(&opt->len);
  clear_opt_anc_info(&opt->anc);
  clear_opt_exact(&opt->exb);
  clear_opt_exact(&opt->exm);
  clear_opt_exact(&opt->expr);
  clear_opt_map(&opt->map);
}