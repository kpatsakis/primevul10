static Int128 addrrange_end(AddrRange r)
{
    return int128_add(r.start, r.size);
}