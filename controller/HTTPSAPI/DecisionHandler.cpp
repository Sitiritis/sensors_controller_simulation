#include "DecisionHandler.h"

#include "Poco/Net/HTTPServerResponse.h"

#include "cereal/archives/json.hpp"

void DecisionHandler::handleRequest(HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response)
{
  response.setContentType("application/json");

  cereal::JSONOutputArchive oArchive(response.send());
  _decisionProvider.getDecision().serialize(oArchive);
}
