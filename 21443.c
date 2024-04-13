readdir_result_type (struct readdir_result d)
{
#if defined _DIRENT_HAVE_D_TYPE || defined HAVE_STRUCT_DIRENT_D_TYPE
# define D_TYPE_TO_RESULT(source) (source)->d_type,
  return d.type;
#else
# define D_TYPE_TO_RESULT(source)
  return DT_UNKNOWN;
#endif
}