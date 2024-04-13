string lua_text_pattern::new_fn_name()
{
    return make_stringf("__ch_stash_search_%u", lfndx++);
}