string_cat(uschar *string, int *size, int *ptr, const uschar *s, int count)
{
int p = *ptr;

if (p + count >= *size)
  {
  int oldsize = *size;

  /* Mostly, string_cat() is used to build small strings of a few hundred
  characters at most. There are times, however, when the strings are very much
  longer (for example, a lookup that returns a vast number of alias addresses).
  To try to keep things reasonable, we use increments whose size depends on the
  existing length of the string. */

  int inc = (oldsize < 4096)? 100 : 1024;
  while (*size <= p + count) *size += inc;

  /* New string */

  if (string == NULL) string = store_get(*size);

  /* Try to extend an existing allocation. If the result of calling
  store_extend() is false, either there isn't room in the current memory block,
  or this string is not the top item on the dynamic store stack. We then have
  to get a new chunk of store and copy the old string. When building large
  strings, it is helpful to call store_release() on the old string, to release
  memory blocks that have become empty. (The block will be freed if the string
  is at its start.) However, we can do this only if we know that the old string
  was the last item on the dynamic memory stack. This is the case if it matches
  store_last_get. */

  else if (!store_extend(string, oldsize, *size))
    {
    BOOL release_ok = store_last_get[store_pool] == string;
    uschar *newstring = store_get(*size);
    memcpy(newstring, string, p);
    if (release_ok) store_release(string);
    string = newstring;
    }
  }

/* Because we always specify the exact number of characters to copy, we can
use memcpy(), which is likely to be more efficient than strncopy() because the
latter has to check for zero bytes. */

memcpy(string + p, s, count);
*ptr = p + count;
return string;
}