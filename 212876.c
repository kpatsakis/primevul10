pattern_match lua_text_pattern::match_location(const string &s) const
{
    // lua_text_pattern is only used if a special non-regex op is detected (^F
    // for "armour && ego", for instance), and in those situations, it's
    // unclear what exactly to use for the matched text here (especially in
    // more complicated expressions that include things like <<>>, !!, etc).
    return matches(s)
        ? pattern_match::succeeded(s)
        : pattern_match::failed(s);
}