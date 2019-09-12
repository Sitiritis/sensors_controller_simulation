#ifndef SENSORS_CONTROLLER_SIMULATION_SENSORDATACONSUMER_H
#define SENSORS_CONTROLLER_SIMULATION_SENSORDATACONSUMER_H

#include "Poco/Activity.h"
#include "Poco/NotificationQueue.h"

#include <atomic>

#include "../concurrentqueue/blockingconcurrentqueue.h" // TODO: change for just concurrent queue
#include "Decision.h"

using namespace Poco;

class SensorDataConsumer
{
  public:
    SensorDataConsumer(
      moodycamel::BlockingConcurrentQueue<UInt32>& buffer,
      const Timestamp::TimeDiff decisionTime,
      NotificationQueue& decisionQueue
    ):
      _activity(this, &SensorDataConsumer::runActivity),
      _buffer(buffer),
      _decision(Decision(Timestamp().utcTime(), false)),
      _decisionTime(decisionTime),
      _decisionQueue(decisionQueue)
    {}

    void start();
    void stop();

    Decision getDecision() const;

  protected:
    void runActivity();

  private:
    Activity<SensorDataConsumer> _activity;
    moodycamel::BlockingConcurrentQueue<UInt32>& _buffer;
    std::atomic<const Decision> _decision;
    const Timestamp::TimeDiff _decisionTime;
    NotificationQueue& _decisionQueue;
};

#endif //SENSORS_CONTROLLER_SIMULATION_SENSORDATACONSUMER_H
