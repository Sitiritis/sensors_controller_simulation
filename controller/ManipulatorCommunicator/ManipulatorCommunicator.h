#ifndef SENSORS_CONTROLLER_SIMULATION_MANIPULATORCOMMUNICATOR_H
#define SENSORS_CONTROLLER_SIMULATION_MANIPULATORCOMMUNICATOR_H

#include "Poco/Activity.h"
#include "Poco/NotificationQueue.h"
#include "Poco/Net/StreamSocket.h"

using namespace Poco;
using namespace Poco::Net;

class ManipulatorCommunicator
{
  public:
    explicit ManipulatorCommunicator(
      StreamSocket& manipulatorSock,
      NotificationQueue& decisionQueue
    ):
      _activity(this, &ManipulatorCommunicator::runActivity),
      _manipulatorSock(manipulatorSock),
      _decisionQueue(decisionQueue)
    {}

    void start();
    void stop();

  protected:
    void runActivity();

  private:
    Activity<ManipulatorCommunicator> _activity;
    NotificationQueue& _decisionQueue;
    StreamSocket& _manipulatorSock;
};


#endif //SENSORS_CONTROLLER_SIMULATION_MANIPULATORCOMMUNICATOR_H
