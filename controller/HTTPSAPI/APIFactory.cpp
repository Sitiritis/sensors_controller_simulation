#include "APIFactory.h"

#include "DecisionHandler.h"

HTTPRequestHandler* APIFactory::createRequestHandler(const HTTPServerRequest &request)
{
  return new DecisionHandler(_decisionProvider);
}
