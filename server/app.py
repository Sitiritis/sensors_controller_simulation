from flask import Flask, render_template, jsonify

app = Flask(__name__)

from config import FlaskConfig as ConfigObject
app.config.from_object(ConfigObject)


import requests as r


def request_status_from_controller(endpoint: str) -> dict:
  return r.get(endpoint).json()


@app.route("/", methods=["GET"])
def index():
  return render_template("index.html.j2")


@app.route("/status", methods=["GET"])
def controller_status():
  response = {}
  resp_code = 200

  try:
    response = request_status_from_controller(app.config["CONTROLLER_API_ENDPOINT"])
  except:
    resp_code = 503

  return jsonify(response), resp_code


if __name__ == '__main__':
  app.run()
