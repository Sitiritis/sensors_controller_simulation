#include "Poco/Net/TCPServer.h"
#include "Poco/Net/TCPServerConnection.h"
#include "Poco/Net/TCPServerConnectionFactory.h"
#include "Poco/Net/NetException.h"
#include "Poco/Util/ServerApplication.h"
#include "Poco/Util/IntValidator.h"
#include <Poco/DateTimeFormatter.h>

#include <iostream>

using namespace Poco;
using namespace Poco::Util;
using namespace Poco::Net;

class ManipulatorApp: public ServerApplication
{
  private:
    class ControllerTCPConnection: public TCPServerConnection
    {
      public:
        ControllerTCPConnection(
          const StreamSocket& s,
          const Timespan& receiveTimeout
        ):
          TCPServerConnection(s), _receiveTimeout(receiveTimeout) {}

        virtual void run() override
        {
          StreamSocket sock = socket();
          sock.setReceiveTimeout(_receiveTimeout);

          Timestamp::UtcTimeVal decisionTime;
          bool decision;
          while (true)
          {
            try
            {
              if (
                (sock.receiveBytes(&decisionTime, sizeof(Timestamp::UtcTimeVal)) > 0) &&
                (sock.receiveBytes(&decision, sizeof(bool) > 0))
              )
              {
                std::cout <<
                  "Decision time: " <<
                    DateTimeFormatter::format(
                      Timestamp::fromUtcTime(decisionTime),
                      "%e.%n.%Y %H:%M:%s"
                    ) << std::endl <<
                  "Decision: " << decision << std::endl << std::endl;
              }
            }
            catch (Poco::Net::ConnectionResetException& ex)
            {
              // Lost connection to the server
              std::cout << ex.what() << std::endl;
              std::cout << "Lost connection to the controller " << sock.peerAddress() << "." << std::endl;
              break;
            }
            catch (TimeoutException& ex)
            {
              std::cout <<
                sock.peerAddress() <<
                " controller did not send any data within the specified timeout interval." <<
                std::endl;
              break;
            }
            catch (std::exception& ex)
            {
              std::cout << ex.what() << std::endl;
              break;
            }
          }

          std::cout << "Finished serving " << sock.peerAddress() << " controller." << std::endl;
        }

      private:
        const Timespan _receiveTimeout;
    };

    class ControllerTCPConnectionFactory: public TCPServerConnectionFactory
    {
      public:
        explicit ControllerTCPConnectionFactory(const Timespan& receiveTimeout):
          _receiveTimeout(receiveTimeout)
        {}

        ControllerTCPConnectionFactory(
          const long timeoutSeconds,
          const long timeoutMicroseconds
        ):
          _receiveTimeout(timeoutSeconds, timeoutMicroseconds)
        {}

        virtual TCPServerConnection* createConnection(const StreamSocket& socket) override
        {
          return new ControllerTCPConnection(socket, _receiveTimeout);
        }

      private:
        const Timespan _receiveTimeout;
    };

  public:
    virtual void defineOptions(OptionSet& options) override
    {
      ServerApplication::defineOptions(options);

      options.addOption(
        Option(
          "port",
          "p",
          "Port of to listen to for the controller connection. The default is 5698.",
          false
        )
        .argument("port-number", true)
        .repeatable(false)
        .validator(new IntValidator(1024, 65535))
        .binding("port")
      );

      options.addOption(
        Option(
          "timeout",
          "t",
          "If controller did not send any data within the specified time, the connection is dropped. The default is 10.",
          false
        )
        .argument("seconds", true)
        .repeatable(false)
        .validator(new IntValidator(1, INT_MAX))
        .binding("timeout.seconds")
      );
    }

    virtual int main(const std::vector<std::string>&) noexcept override
    {
      const UInt16 sensorServerPort =
        static_cast<const UInt16>(config().getUInt("port", 5698));
      const long receiveTimeout =
        static_cast<long>(config().getInt64("timeout.seconds", 10));

      TCPServer srv(
        new ControllerTCPConnectionFactory(receiveTimeout, 0),
        sensorServerPort
      );
      srv.start();
      logger().information("TCP server for controller is UP.");
      waitForTerminationRequest();
      logger().information("Shutting down the TCP server for manipulator.");
      srv.stop();

      return Application::EXIT_OK;
    }
};

POCO_SERVER_MAIN(ManipulatorApp)
