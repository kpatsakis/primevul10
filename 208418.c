static uint32_t *insecure_flag(int opt_idx, int opt_flags)
{
  if (opt_flags & OPT_LOCAL)
    switch ((int)options[opt_idx].indir) {
    case PV_STL:        return &curwin->w_p_stl_flags;
    case PV_FDE:        return &curwin->w_p_fde_flags;
    case PV_FDT:        return &curwin->w_p_fdt_flags;
    case PV_INDE:       return &curbuf->b_p_inde_flags;
    case PV_FEX:        return &curbuf->b_p_fex_flags;
    case PV_INEX:       return &curbuf->b_p_inex_flags;
    }

  /* Nothing special, return global flags field. */
  return &options[opt_idx].flags;
}