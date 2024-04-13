void Commissioner::ClearJoiners(void)
{
    for (Joiner *joiner = &mJoiners[0]; joiner < OT_ARRAY_END(mJoiners); joiner++)
    {
        joiner->mValid = false;
    }

    SendCommissionerSet();
}