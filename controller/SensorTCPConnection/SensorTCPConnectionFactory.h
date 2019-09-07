#ifndef SENSORS_CONTROLLER_SIMULATION_SENSORTCPCONNECTIONFACTORY_H
#define SENSORS_CONTROLLER_SIMULATION_SENSORTCPCONNECTIONFACTORY_H

#include "Poco/Net/TCPServerConnectionFactory.h"
#include "Poco/Types.h"

#include "SensorTCPConnection.h"

using namespace Poco;
using namespace Poco::Net;

class SensorTCPConnectionFactory: public TCPServerConnectionFactory
{
  public:
    explicit SensorTCPConnectionFactory(moodycamel::BlockingConcurrentQueue<UInt32>& buffer):
      _buffer(buffer) {}

  virtual TCPServerConnection* createConnection(const StreamSocket& socket) override;

  private:
    moodycamel::BlockingConcurrentQueue<UInt32>& _buffer;
};


#endif //SENSORS_CONTROLLER_SIMULATION_SENSORTCPCONNECTIONFACTORY_H
