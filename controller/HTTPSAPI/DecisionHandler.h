#ifndef SENSORS_CONTROLLER_SIMULATION_DECISIONHANDLER_H
#define SENSORS_CONTROLLER_SIMULATION_DECISIONHANDLER_H

#include "Poco/Net/HTTPRequestHandler.h"

#include "../SensorDataConsumer/SensorDataConsumer.h"

using Poco::Net::HTTPRequestHandler;
using Poco::Net::HTTPServerRequest;
using Poco::Net::HTTPServerResponse;

class DecisionHandler: public HTTPRequestHandler
{
  public:
    explicit DecisionHandler(const SensorDataConsumer& decisionProvider):
      _decisionProvider(decisionProvider)
    {}

    virtual void handleRequest(
      HTTPServerRequest& request, HTTPServerResponse& response
    ) override;

  private:
    const SensorDataConsumer& _decisionProvider;
};

#endif //SENSORS_CONTROLLER_SIMULATION_DECISIONHANDLER_H
