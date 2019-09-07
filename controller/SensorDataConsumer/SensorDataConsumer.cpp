#include "SensorDataConsumer.h"

#include "Poco/DateTimeFormatter.h"
#include <iostream>

void SensorDataConsumer::start()
{
  _activity.start();
}

void SensorDataConsumer::stop()
{
  _activity.stop(); // request the activity to stop
  _activity.wait(); // wait (block) until it actually stops
}

void SensorDataConsumer::runActivity()
{
  long long mean = 0;
  long long totalElements = 0;
  Timestamp processingStartTime;

  while (!_activity.isStopped())
  {
    UInt32 curPayload;
    _buffer.wait_dequeue(curPayload);
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
      auto decision = Decision(Timestamp().utcTime(), mean >= 50);
      _decision.store(decision, std::memory_order_release);
      std::cout <<
        "Mean: " << mean << "\n" <<
        "Decision time: " <<
          DateTimeFormatter::format(
            Timestamp().fromUtcTime(decision.decisionTimestamp),
            "%e.%n.%Y %H:%M:%s"
          )
          << "\n" <<
        "Decision: " << decision.decision << "\n\n";

      mean = 0;
      totalElements = 0;
      processingStartTime.update();
    }
  }
}
