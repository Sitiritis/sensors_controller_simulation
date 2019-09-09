#include "SensorDataConsumer.h"

#include "Poco/DateTimeFormatter.h"

#include <iostream>

#include "DecisionNotification.h"
#include "../common/DeathPillNotification.h"

void SensorDataConsumer::start()
{
  _activity.start();
}

void SensorDataConsumer::stop()
{
  _activity.stop(); // request the activity to stop
  _activity.wait(); // wait (block) until it actually stops
}

Decision SensorDataConsumer::getDecision() const
{
  return _decision.load();
}

void SensorDataConsumer::runActivity()
{
  long long mean = 0;
  long long totalElements = 0;
  Timestamp processingStartTime;

  while (!_activity.isStopped())
  {
    UInt32 curPayload;
    if (_buffer.try_dequeue(curPayload))
    {
      ++totalElements;
      // https://dev.to/nestedsoftware/calculating-a-moving-average-on-streaming-data-5a7k
      mean = mean + ((curPayload - mean) / totalElements);
//    std::cout
//      << "Payload: " << curPayload << "\n"
//      << "Total elements: " << totalElements << "\n"
//      << "Intermediate mean: " << mean << "\n"
//      << "Elements in queue: " << _buffer.size_approx() << "\n\n";

      if (processingStartTime.elapsed() >= _decisionTime)
      {
        const Decision decision = Decision(Timestamp().utcTime(), mean >= 50);
        _decision.store(decision, std::memory_order_release);
        _decisionQueue.enqueueNotification(new DecisionNotification(decision));
        std::cout <<
          "Mean: " << mean << std::endl <<
          "Elements in queue to process: " << _buffer.size_approx() << std::endl <<
          "Decision time: " <<
          DateTimeFormatter::format(
            Timestamp::fromUtcTime(decision.decisionTimestamp),
            "%e.%n.%Y %H:%M:%s"
          ) << std::endl <<
          "Decision: " << decision.decision << std::endl << std::endl;

        mean = 0;
        totalElements = 0;
        processingStartTime.update();
      }
    }
  }

  for (int i = 0; i < _numberWorkers; ++i)
  {
    _decisionQueue.enqueueNotification(new DeathPillNotification());
  }
}
