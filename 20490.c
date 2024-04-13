add_save_type(regex_t* reg, enum SaveType type)
{
  SaveType t = (SaveType )type;

  BB_ADD(reg, &t, SIZE_SAVE_TYPE);
  return 0;
}