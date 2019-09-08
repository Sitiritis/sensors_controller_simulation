#ifndef SENSORS_CONTROLLER_SIMULATION_DECISION_H
#define SENSORS_CONTROLLER_SIMULATION_DECISION_H

#include "Poco/Timestamp.h"

using namespace Poco;

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

#endif //SENSORS_CONTROLLER_SIMULATION_DECISION_H
