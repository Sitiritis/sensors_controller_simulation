#include "SensorTCPConnectionFactory.h"

TCPServerConnection *SensorTCPConnectionFactory::createConnection(const StreamSocket &socket)
{
  return new SensorTCPConnection(socket, _buffer);
}
