bool shortmess(int x)
{
  return (p_shm != NULL
          && (vim_strchr(p_shm, x) != NULL
              || (vim_strchr(p_shm, 'a') != NULL
                  && vim_strchr((char_u *)SHM_ALL_ABBREVIATIONS, x) != NULL)));
}