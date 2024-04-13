void Commissioner::HandleJoinerExpirationTimer(void)
{
    TimeMilli now = TimerMilli::GetNow();

    // Remove Joiners.
    for (Joiner *joiner = &mJoiners[0]; joiner < OT_ARRAY_END(mJoiners); joiner++)
    {
        if (!joiner->mValid)
        {
            continue;
        }

        if (now >= joiner->mExpirationTime)
        {
            otLogDebgMeshCoP("removing joiner due to timeout or successfully joined");
            RemoveJoiner(&joiner->mEui64, 0); // remove immediately
        }
    }

    UpdateJoinerExpirationTimer();
}