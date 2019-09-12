#include "Poco/Net/TCPServer.h"
#include "Poco/Net/HTTPServer.h"
#include "Poco/Util/ServerApplication.h"
#include "Poco/Util/IntValidator.h"
#include "Poco/Util/RegExpValidator.h"

#include <iostream>

#include "SensorTCPConnection/SensorTCPConnectionFactory.h"
#include "SensorDataConsumer/SensorDataConsumer.h"
#include "ManipulatorCommunicator/ManipulatorCommunicator.h"
#include "HTTPSAPI/APIFactory.h"

using namespace Poco;
using namespace Poco::Util;

class ControllerApp: public ServerApplication
{
  public:
    virtual void defineOptions(OptionSet& options) override
    {
      ServerApplication::defineOptions(options);

      options.addOption(
        Option(
          "queue-size",
          "q",
          "The initial size of the queue which is used as a buffer between sensor producer and its consumer threads. The default is 300.",
          false
        )
        .argument("size", true)
        .repeatable(false)
        .validator(new IntValidator(100, 5120))
        .binding("queue.size")
      );

      options.addOption(
        Option(
          "decision-time",
          "t",
          "Time interval in microseconds after which a decision is made. The default is 5000000.",
          false
        )
        .argument("milliseconds", true)
        .repeatable(false)
        .validator(new IntValidator(0, UINT_MAX))
        .binding("decisionTime")
      );

      options.addOption(
        Option(
          "sensors-server-port",
          "s",
          "Port that the controller will listen to for sensors' connection requests. The default is 9874.",
          false
        )
        .argument("port", true)
        .repeatable(false)
        .validator(new IntValidator(1024, 65535))
        .binding("port.sensorServer")
      );

      options.addOption(
        Option(
          "sensor-threads",
          "c",
          "Size of thread pool created to handle sensors. The default is 8.",
          false
        )
        .argument("number", true)
        .repeatable(false)
        .validator(new IntValidator(1, 64))
        .binding("sensorServer.threadPoolSize")
      );

      options.addOption(
        Option(
          "api-port",
          "a",
          "Port on which the controller will be providing HTTPS API. The default is 7896.",
          false
        )
        .argument("port", true)
        .repeatable(false)
        .validator(new IntValidator(1024, 65535))
        .binding("port.api")
      );

      options.addOption(
        Option(
          "api-threads",
          "r",
          "Size of thread pool created to handle api requests. The default is 2.",
          false
        )
        .argument("number", true)
        .repeatable(false)
        .validator(new IntValidator(1, 64))
        .binding("api.threadPoolSize")
      );

      options.addOption(
        Option(
          "manipulator-port",
          "m",
          "Manipulator port, where the decisions of the controller will be submitted. The default is 5698.",
          false
        )
        .argument("port", true)
        .repeatable(false)
        .validator(new IntValidator(1024, 65535))
        .binding("port.manipulator")
      );

      options.addOption(
        Option(
          "manipulator-host",
          "h",
          "Either host name or IP address of the manipulator, where the decisions of the controller will be submitted. The default is 127.0.0.1.",
          false
        )
        .argument("host", true)
        .repeatable(false)
        .validator(new RegExpValidator(
          R"(^(?:(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?))|(([a-zA-Z0-9]|[a-zA-Z0-9][a-zA-Z0-9\-]*[a-zA-Z0-9])\.)*([A-Za-z0-9]|[A-Za-z0-9][A-Za-z0-9\-]*[A-Za-z0-9])$)"
        ))
        .binding("host.manipulator")
      );
    }

    virtual int main(const std::vector<std::string>& args) override
    {
      const UInt16 sensorServerPort =
        static_cast<const UInt16>(config().getUInt("port.sensorServer", 9874));
      const unsigned long bufferSize = static_cast<const unsigned long>(
        config().getUInt64("queue.size", 300)
      );
      const Timestamp::TimeDiff decisionTime = static_cast<const Timestamp::TimeDiff>(
        config().getInt64("decisionTime", 5000000)
      );
      const UInt16 manipulatorPort =
        static_cast<const UInt16>(config().getUInt("port.manipulator", 5698));
      const std::string manipulatorHost =
        config().getString("host.manipulator", "127.0.0.1");
      const UInt16 apiPort =
        static_cast<const UInt16>(config().getUInt("port.api", 7896));
      const int numSensorThreads =
        config().getInt("sensorServer.threadPoolSize", 8);
      const int numApiThreads =
        config().getInt("api.threadPoolSize", 2);

      StreamSocket manipulatorSock;
      try
      {
        manipulatorSock.connect(SocketAddress(manipulatorHost, manipulatorPort));
      }
      catch (const std::exception& ex)
      {
        logger().error(ex.what());
        logger().error(
          "Unable to connect to the manipulator at " +
          manipulatorHost +
          ":" + std::to_string(manipulatorPort)
        );
        return Application::EXIT_UNAVAILABLE;
      }

      NotificationQueue decisionQueue;
      moodycamel::BlockingConcurrentQueue<UInt32> sensorDataBuffer(bufferSize);

      ManipulatorCommunicator manCommunicator(
        manipulatorSock,
        decisionQueue
      );
      SensorDataConsumer sensorDataConsumer(
        sensorDataBuffer,
        decisionTime,
        decisionQueue
      );
      ThreadPool sensorServerThreadPool("sensor_server_thread_pool", 1, numSensorThreads);
      const ServerSocket sensorServerSock(sensorServerPort);
      TCPServer sensorServer(
        new SensorTCPConnectionFactory(sensorDataBuffer),
        sensorServerThreadPool,
        sensorServerSock
      );
      ThreadPool apiServerThreadPool("api_server_thread_pool", 1, numApiThreads);
      const ServerSocket apiServerSock(apiPort);
      HTTPServer apiServer(
        new APIFactory(sensorDataConsumer),
        apiServerThreadPool,
        apiServerSock,
        new HTTPServerParams()
      );

      manCommunicator.start();
      sensorDataConsumer.start();
      sensorServer.start();
      apiServer.start();
      logger().information("TCP server for sensors is UP.");

      waitForTerminationRequest();

      logger().information("Shutting down the TCP server for sensors.");
      apiServer.stop();
      sensorServer.stop();
      sensorDataConsumer.stop();
      manCommunicator.stop();

      manipulatorSock.close();

      return Application::EXIT_OK;
    }
};

POCO_SERVER_MAIN(ControllerApp)
