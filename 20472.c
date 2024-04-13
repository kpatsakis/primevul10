add_update_var_type(regex_t* reg, enum UpdateVarType type)
{
  UpdateVarType t = (UpdateVarType )type;

  BB_ADD(reg, &t, SIZE_UPDATE_VAR_TYPE);
  return 0;
}