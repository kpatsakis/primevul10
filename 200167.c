reverse_symbol_list (symbol *sym)
{
  symbol *result;
  symbol *next;

  result = NULL;
  while (sym)
    {
      next = SYMBOL_NEXT (sym);
      SYMBOL_NEXT (sym) = result;
      result = sym;
      sym = next;
    }
  return result;
}