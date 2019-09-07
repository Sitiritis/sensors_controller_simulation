#ifndef SENSORS_CONTROLLER_SIMULATION_SENSORDATACONSUMER_H
#define SENSORS_CONTROLLER_SIMULATION_SENSORDATACONSUMER_H

#include "Poco/Activity.h"
#include "Poco/Timestamp.h"

#include <atomic>

#include "../concurrentqueue/blockingconcurrentqueue.h"

using namespace Poco;

class SensorDataConsumer
{
  public:
    SensorDataConsumer(
      moodycamel::BlockingConcurrentQueue<UInt32>& buffer,
      const Timestamp::TimeDiff decisionTime
    ):
      _activity(this, &SensorDataConsumer::runActivity),
      _buffer(buffer),
      _decision(Decision(Timestamp().utcTime(), false)),
      _decisionTime(decisionTime)
    {}

    void start();

    void stop();

  protected:
    void runActivity();

  private:
    struct Decision
    {
      public:
        Decision(const Timestamp::UtcTimeVal timestamp, const bool result):
          decisionTimestamp(timestamp),
          decision(result)
        {}

        const Timestamp::UtcTimeVal decisionTimestamp;
        const bool decision;
    };

    Activity<SensorDataConsumer> _activity;
    moodycamel::BlockingConcurrentQueue<UInt32>& _buffer;
    std::atomic<const Decision> _decision;
    const Timestamp::TimeDiff _decisionTime;
};

#endif //SENSORS_CONTROLLER_SIMULATION_SENSORDATACONSUMER_H
