#include "Poco/Net/StreamSocket.h"
#include "Poco/Net/NetException.h"
#include "Poco/Util/ServerApplication.h"
#include "Poco/Util/IntValidator.h"
#include "Poco/Util/RegExpValidator.h"
#include "Poco/Random.h"
#include "Poco/Timer.h"

#include <iostream>

using namespace Poco;
using namespace Poco::Util;
using namespace Poco::Net;

class SensorApp: public ServerApplication
{
  private:
    class PayloadSender
    {
      public:
        explicit PayloadSender(StreamSocket& sock): _sock(sock), _rng()
        {
          _rng.seed();
        }

        void onTimer(Timer& timer)
        {
          try
          {
            Timestamp::UtcTimeVal curTime = Timestamp().utcTime();
            UInt32 payload = _rng.next(100);
            _sock.sendBytes(&curTime, sizeof(Timestamp::UtcTimeVal));
            _sock.sendBytes(&payload, sizeof(UInt32));
          }
          catch (Poco::Net::ConnectionResetException& ex)
          {
            // Lost connection to the server
            std::cout << ex.what() << std::endl;
            timer.restart(0); // stop the timer
          }
          catch (std::exception& ex)
          {
            std::cout << ex.what() << std::endl;
            timer.restart(0); // stop the timer
          }
        }

      private:
        StreamSocket& _sock;
        Random _rng;
    };

  public:
    virtual void defineOptions(OptionSet& options) override
    {
      ServerApplication::defineOptions(options);

      options.addOption(
        Option(
          "port",
          "p",
          "Host port to connect to. The default is 9874.",
          false
        )
        .argument("port-number", true)
        .repeatable(false)
        .validator(new IntValidator(1024, 65535))
        .binding("port")
      );

      options.addOption(
        Option(
          "host",
          "h",
          "Either host name or IP address of the host to connect to. The default is 127.0.0.1.",
          false
        )
        .argument("host", true)
        .repeatable(false)
        .validator(new RegExpValidator(
          R"(^(?:(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?))|(([a-zA-Z0-9]|[a-zA-Z0-9][a-zA-Z0-9\-]*[a-zA-Z0-9])\.)*([A-Za-z0-9]|[A-Za-z0-9][A-Za-z0-9\-]*[A-Za-z0-9])$)"
         )) // either IP address of a host name
        .binding("host")
      );

      options.addOption(
        Option(
          "requests-per-second",
          "n",
          "Number of requests per second. The default is 300.",
          false
        )
        .argument("num-requests", true)
        .repeatable(false)
        .validator(new IntValidator(1, 1000))
        .binding("rps")
      );
    }

    virtual int main(const std::vector<std::string>&) noexcept override
    {
      // Get the necessary config values
      const UInt16 port = static_cast<const UInt16>(config().getUInt("port", 9874));
      const std::string host = config().getString("host", "127.0.0.1");
      const long requestsPerSecond = static_cast<const long>(config().getInt64("rps", 300));

      // Create a TCP socket & establish the connection
      StreamSocket sock;
      try
      {
        sock.connect(SocketAddress(host, port));
      }
      catch (const std::exception& ex)
      {
        logger().error(ex.what());
        logger().error(
          "Unable to connect to the controller at " +
          host +
          ":" + std::to_string(port)
        );
        return Application::EXIT_UNAVAILABLE;
      }

      PayloadSender payloadSender(sock);
      Timer requestSenderTimer(0, 1000 / requestsPerSecond);
      requestSenderTimer.start(
        TimerCallback<PayloadSender>(payloadSender, &PayloadSender::onTimer)
      );
      waitForTerminationRequest();
      requestSenderTimer.stop();

      return Application::EXIT_OK;
    }
};

POCO_SERVER_MAIN(SensorApp)
