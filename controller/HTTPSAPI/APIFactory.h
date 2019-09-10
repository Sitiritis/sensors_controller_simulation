#ifndef SENSORS_CONTROLLER_SIMULATION_APIFACTORY_H
#define SENSORS_CONTROLLER_SIMULATION_APIFACTORY_H

#include "Poco/Net/HTTPRequestHandlerFactory.h"

#include "../SensorDataConsumer/SensorDataConsumer.h"

using namespace Poco::Net;

class APIFactory: public HTTPRequestHandlerFactory
{
  public:
    explicit APIFactory(const SensorDataConsumer& decisionProvider):
      _decisionProvider(decisionProvider)
    {}

    virtual HTTPRequestHandler* createRequestHandler(
      const HTTPServerRequest& request
    ) override;

  private:
    const SensorDataConsumer& _decisionProvider;
};

#endif //SENSORS_CONTROLLER_SIMULATION_APIFACTORY_H
