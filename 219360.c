string_log_address(address_item *addr, BOOL all_parents, BOOL success)
{
int size = 64;
int ptr = 0;
BOOL add_topaddr = TRUE;
uschar *yield = store_get(size);
address_item *topaddr;

/* Find the ultimate parent */

for (topaddr = addr; topaddr->parent != NULL; topaddr = topaddr->parent);

/* We start with just the local part for pipe, file, and reply deliveries, and
for successful local deliveries from routers that have the log_as_local flag
set. File deliveries from filters can be specified as non-absolute paths in
cases where the transport is goin to complete the path. If there is an error
before this happens (expansion failure) the local part will not be updated, and
so won't necessarily look like a path. Add extra text for this case. */

if (testflag(addr, af_pfr) ||
      (success &&
       addr->router != NULL && addr->router->log_as_local &&
       addr->transport != NULL && addr->transport->info->local))
  {
  if (testflag(addr, af_file) && addr->local_part[0] != '/')
    yield = string_cat(yield, &size, &ptr, CUS"save ", 5);
  yield = string_get_localpart(addr, yield, &size, &ptr);
  }

/* Other deliveries start with the full address. It we have split it into local
part and domain, use those fields. Some early failures can happen before the
splitting is done; in those cases use the original field. */

else
  {
  if (addr->local_part != NULL)
    {
    yield = string_get_localpart(addr, yield, &size, &ptr);
    yield = string_cat(yield, &size, &ptr, US"@", 1);
    yield = string_cat(yield, &size, &ptr, addr->domain,
      Ustrlen(addr->domain) );
    }
  else
    {
    yield = string_cat(yield, &size, &ptr, addr->address, Ustrlen(addr->address));
    }
  yield[ptr] = 0;

  /* If the address we are going to print is the same as the top address,
  and all parents are not being included, don't add on the top address. First
  of all, do a caseless comparison; if this succeeds, do a caseful comparison
  on the local parts. */

  if (strcmpic(yield, topaddr->address) == 0 &&
      Ustrncmp(yield, topaddr->address, Ustrchr(yield, '@') - yield) == 0 &&
      addr->onetime_parent == NULL &&
      (!all_parents || addr->parent == NULL || addr->parent == topaddr))
    add_topaddr = FALSE;
  }

/* If all parents are requested, or this is a local pipe/file/reply, and
there is at least one intermediate parent, show it in brackets, and continue
with all of them if all are wanted. */

if ((all_parents || testflag(addr, af_pfr)) &&
    addr->parent != NULL &&
    addr->parent != topaddr)
  {
  uschar *s = US" (";
  address_item *addr2;
  for (addr2 = addr->parent; addr2 != topaddr; addr2 = addr2->parent)
    {
    yield = string_cat(yield, &size, &ptr, s, 2);
    yield = string_cat(yield, &size, &ptr, addr2->address, Ustrlen(addr2->address));
    if (!all_parents) break;
    s = US", ";
    }
  yield = string_cat(yield, &size, &ptr, US")", 1);
  }

/* Add the top address if it is required */

if (add_topaddr)
  {
  yield = string_cat(yield, &size, &ptr, US" <", 2);

  if (addr->onetime_parent == NULL)
    yield = string_cat(yield, &size, &ptr, topaddr->address,
      Ustrlen(topaddr->address));
  else
    yield = string_cat(yield, &size, &ptr, addr->onetime_parent,
      Ustrlen(addr->onetime_parent));

  yield = string_cat(yield, &size, &ptr, US">", 1);
  }

yield[ptr] = 0;  /* string_cat() leaves space */
return yield;
}