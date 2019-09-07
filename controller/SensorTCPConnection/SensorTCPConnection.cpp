#include "SensorTCPConnection.h"

#include "Poco/Net/SocketStream.h"
#include "Poco/Net/NetException.h"
#include "Poco/Timestamp.h"

#include <iostream>

void SensorTCPConnection::run()
{
  StreamSocket sock = socket();
  sock.setReceiveTimeout(Timespan(1, 0));

  Timestamp::UtcTimeVal sensorTime;
  UInt32 payload;
  while (true)
  {
    try
    {
      if (
        (sock.receiveBytes(&sensorTime, sizeof(Timestamp::UtcTimeVal)) > 0) &&
        (sock.receiveBytes(&payload, sizeof(UInt32)) > 0)
      )
      {
        _buffer.enqueue(payload);
      }
      else
      {
        break;
      }
    }
    catch (NetException& ex)
    {
      // Sensor is disconnected
      std::cout << sock.peerAddress() << " sensor has unexpectedly disconnected.\n";
      break;
    }
    catch (TimeoutException& ex)
    {
      std::cout << sock.peerAddress() << " sensor did not send any data within 1 second.\n";
      break;
    }
    catch (std::exception& ex)
    {
      std::cout << ex.what() << "\n";
      break;
    }
  }

  std::cout << "Finished serving " << sock.peerAddress() << " sensor.\n";
}
