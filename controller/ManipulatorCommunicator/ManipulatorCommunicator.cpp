#include "ManipulatorCommunicator.h"

#include "Poco/Net/NetException.h"

#include <iostream>

#include "../SensorDataConsumer/DecisionNotification.h"
#include "../common/DeathPillNotification.h"

void ManipulatorCommunicator::start()
{
  _activity.start();
}

void ManipulatorCommunicator::stop()
{
  _activity.stop(); // request the activity to stop
  _activity.wait(); // wait (block) until it actually stops
}

void ManipulatorCommunicator::runActivity()
{
  while (!_activity.isStopped())
  {
    AutoPtr<Notification> notif(_decisionQueue.dequeueNotification());
    if (auto pNotif = notif.get())
    {
      if (dynamic_cast<DeathPillNotification*>(pNotif))
      {
        std::cout << "Shutting down manipulator communicator.\n";
        break;
      }
      else
      {
        if (auto pDecisionNotif = dynamic_cast<const DecisionNotification*>(pNotif))
        {
          try
          {
            _manipulatorSock.sendBytes(
              &(pDecisionNotif->decision.decisionTimestamp),
              sizeof(Timestamp::UtcTimeVal)
            );
            _manipulatorSock.sendBytes(
              &(pDecisionNotif->decision.decision),
              sizeof(bool)
            );
          }
          catch (Poco::Net::ConnectionResetException& ex)
          {
            // Lost connection to the manipulator
            std::cout << ex.what() << "\n";
            std::cout << "Lost connection to the manipulator.\n";

            // TODO: stop the server if manipulator goes down
            while (!_activity.isStopped())
            {
              _decisionQueue.clear();
            }
          }
          catch (std::exception& ex)
          {
            std::cout << ex.what() << "\n";
          }
        }
        else
        {
          std::cout << "Manipulator communicator received an unknown notification";
        }
      }
    }
  }
}
