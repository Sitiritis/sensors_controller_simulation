#ifndef SENSORS_CONTROLLER_SIMULATION_DECISIONNOTIFICATION_H
#define SENSORS_CONTROLLER_SIMULATION_DECISIONNOTIFICATION_H

#include "Decision.h"

class DecisionNotification: public Notification
{
  public:
    explicit DecisionNotification(const Decision dec): decision(dec) {}

    const Decision decision;
};

#endif //SENSORS_CONTROLLER_SIMULATION_DECISIONNOTIFICATION_H
