  media::StatisticsCB NewStatisticsCB() {
    return base::Bind(&media::MockStatisticsCB::OnStatistics,
                      base::Unretained(&statistics_cb_object_));
  }
