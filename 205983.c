bool Item_splocal::set_value(THD *thd, sp_rcontext *ctx, Item **it)
{
  return ctx->set_variable(thd, get_var_idx(), it);
}