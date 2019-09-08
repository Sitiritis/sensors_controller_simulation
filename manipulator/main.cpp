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
        explicit ControllerTCPConnection(
          const StreamSocket& s
        ):
          TCPServerConnection(s) {}

        virtual void run() override
        {
          StreamSocket sock = socket();

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
                    ) << "\n" <<
                  "Decision: " << decision << "\n\n";
              }
              else
              {
                break;
              }
            }
            catch (Poco::Net::ConnectionResetException& ex)
            {
              // Lost connection to the server
              std::cout << ex.what() << "\n";
              std::cout << "Lost connection to the controller " << sock.peerAddress() << ".\n";
            }
            catch (std::exception& ex)
            {
              std::cout << ex.what() << "\n";
            }
          }

          std::cout << "Finished serving " << sock.peerAddress() << " controller.\n";
        }
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
    }

    virtual int main(const std::vector<std::string>&) noexcept override
    {
      const UInt16 sensorServerPort =
        static_cast<const UInt16>(config().getUInt("port", 5698));

      TCPServer srv(
        new TCPServerConnectionFactoryImpl<ControllerTCPConnection>(),
        sensorServerPort
      );
      srv.start();
      logger().information("TCP server for manipulator is UP.");
      waitForTerminationRequest();
      logger().information("Shutting down the TCP server for manipulator.");
      srv.stop();

      return Application::EXIT_OK;
    }
};

POCO_SERVER_MAIN(ManipulatorApp)
