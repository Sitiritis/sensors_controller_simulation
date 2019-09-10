#ifndef SENSORS_CONTROLLER_SIMULATION_DECISION_H
#define SENSORS_CONTROLLER_SIMULATION_DECISION_H

#include "Poco/Timestamp.h"
#include "Poco/DateTimeFormatter.h"

#include "cereal/cereal.hpp"

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

    template<class Archive>
    void serialize(Archive& archive)
    {
      std::string formattedTime = DateTimeFormatter::format(
        Timestamp::fromUtcTime(decisionTimestamp),
        "%Y%m%dT%H%M"
      );
      std::string status = (decision) ? "up": "down";

      archive(
        cereal::make_nvp(
          "datetime",
          formattedTime
        ),
        cereal::make_nvp(
          "status",
          status
        )
      );
    }
};

#endif //SENSORS_CONTROLLER_SIMULATION_DECISION_H
