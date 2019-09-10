import os


class FlaskConfig(object):
  CONTROLLER_API_PROTOCOL = \
    os.environ.get("CONTROLLER_API_PROTOCOL", "http")
  CONTROLLER_API_HOST_ADDRESS = \
    os.environ.get("CONTROLLER_API_HOST_ADDRESS", "localhost")
  CONTROLLER_API_HOST_PORT = \
    os.environ.get("CONTROLLER_API_PROTOCOL", "7896")
  CONTROLLER_API_ENDPOINT = \
    CONTROLLER_API_PROTOCOL + "://" +\
    CONTROLLER_API_HOST_ADDRESS + ":" +\
    CONTROLLER_API_HOST_PORT
