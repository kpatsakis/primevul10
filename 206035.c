bool Item_cache_str::cache_value()
{
  if (!example)
    return FALSE;
  value_cached= TRUE;
  value_buff.set(buffer, sizeof(buffer), example->collation.collation);
  value= example->str_result(&value_buff);
  if ((null_value= example->null_value))
    value= 0;
  else if (value != &value_buff)
  {
    /*
      We copy string value to avoid changing value if 'item' is table field
      in queries like following (where t1.c is varchar):
      select a, 
             (select a,b,c from t1 where t1.a=t2.a) = ROW(a,2,'a'),
             (select c from t1 where a=t2.a)
        from t2;
    */
    value_buff.copy(*value);
    value= &value_buff;
  }
  return TRUE;
}