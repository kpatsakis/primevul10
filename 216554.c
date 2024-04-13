void Commissioner::UpdateJoinerExpirationTimer(void)
{
    TimeMilli now         = TimerMilli::GetNow();
    uint32_t  nextTimeout = TimeMilli::kMaxDuration;

    // Check if timer should be set for next Joiner.
    for (Joiner *joiner = &mJoiners[0]; joiner < OT_ARRAY_END(mJoiners); joiner++)
    {
        uint32_t diff;

        if (!joiner->mValid)
        {
            continue;
        }

        if (now >= joiner->mExpirationTime)
        {
            nextTimeout = 0;
            break;
        }

        diff = joiner->mExpirationTime - now;

        if (diff < nextTimeout)
        {
            nextTimeout = diff;
        }
    }

    if (nextTimeout != TimeMilli::kMaxDuration)
    {
        // Update the timer to the timeout of the next Joiner.
        mJoinerExpirationTimer.Start(nextTimeout);
    }
    else
    {
        // No Joiners, stop the timer.
        mJoinerExpirationTimer.Stop();
    }
}