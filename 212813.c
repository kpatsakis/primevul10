static int _clua_panic(lua_State *ls)
{
    UNUSED(ls);
    if (crawl_state.need_save && !crawl_state.saving_game
        && !crawl_state.updating_scores)
    {
        save_game(true);
    }
    return 0;
}