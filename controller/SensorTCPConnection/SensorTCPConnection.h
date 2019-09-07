#ifndef SENSORS_CONTROLLER_SIMULATION_SENSORTCPCONNECTION_H
#define SENSORS_CONTROLLER_SIMULATION_SENSORTCPCONNECTION_H

#include "Poco/Net/TCPServerConnection.h"
#include "Poco/Types.h"

#include "../concurrentqueue/blockingconcurrentqueue.h"

using namespace Poco;
using namespace Poco::Net;

class SensorTCPConnection: public TCPServerConnection
{
  public:
    SensorTCPConnection(
      const StreamSocket& s,
      moodycamel::BlockingConcurrentQueue<UInt32>& buffer
    ):
      TCPServerConnection(s), _buffer(buffer) {}

    virtual void run() override;

  private:
    moodycamel::BlockingConcurrentQueue<UInt32>& _buffer;
};

#endif //SENSORS_CONTROLLER_SIMULATION_SENSORTCPCONNECTION_H
