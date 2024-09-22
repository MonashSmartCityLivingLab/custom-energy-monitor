# setup flask file
from flask import Flask
import flask

app = Flask(__name__)


@app.route("/")
def hello_world():
    return "This is the energy monitor server!"


@app.route("/receive_data", methods=["POST"])
def receive_data():
    if flask.request.method == "POST":
        data = flask.request.get_json()
        print(data)
        return "Data received"


if __name__ == "__main__":
    app.run(host="0.0.0.0", port=5001)
