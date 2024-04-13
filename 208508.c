inline bool Parser::IsNaNOrInfinityLiteral(LPCOLESTR str)
{
    // Note: wcscmp crashes when one of the parameters is NULL.
    return str &&
           (wcscmp(_u("NaN"), str) == 0 ||
           wcscmp(_u("Infinity"), str) == 0 ||
               (CheckForNegativeInfinity && wcscmp(_u("-Infinity"), str) == 0));
}