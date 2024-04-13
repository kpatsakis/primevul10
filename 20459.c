alt_merge_node_opt_info(NodeOpt* to, NodeOpt* add, OptEnv* env)
{
  alt_merge_opt_anc_info(&to->anc, &add->anc);
  alt_merge_opt_exact(&to->exb,  &add->exb, env);
  alt_merge_opt_exact(&to->exm,  &add->exm, env);
  alt_merge_opt_exact(&to->expr, &add->expr, env);
  alt_merge_opt_map(env->enc, &to->map, &add->map);

  alt_merge_mml(&to->len, &add->len);
}