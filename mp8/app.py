import json
from flask import Flask, jsonify, send_file, render_template, request
import requests
import os
import io
import boto3
import base64
import dotenv

app = Flask(__name__)
client = boto3.client('s3', endpoint_url="http://127.0.0.1:9000", aws_access_key_id="ROOTNAME", aws_secret_access_key = "CHANGEME123")
s3 = boto3.resource('s3', endpoint_url="http://127.0.0.1:9000", aws_access_key_id="ROOTNAME", aws_secret_access_key = "CHANGEME123")
bucket = s3.Bucket("bucket")
if not bucket.creation_date:
    client.create_bucket(Bucket = "bucket")

state = {
    "colormap": "cividis",
    "real": 0.36,
    "imag": -0.09,
    "height": 0.01,
    "dim": 512,
    "iter": 128
}


# http://127.0.0.1:34000/mandelbrot/cividis/0.36:-0.09:0.01:512:128

@app.route('/')
def index():
    return render_template('index.html')

@app.route('/all')
def all():
    return render_template('all.html')

@app.route('/moveUp', methods=["POST"])
def moveUp():
    state["imag"] += state["height"]*0.25
    return "Success", 200

@app.route('/moveDown', methods=["POST"])
def moveDown():
    state["imag"] -= state["height"]*0.25
    return "Success", 200

@app.route('/moveLeft', methods=["POST"])
def moveLeft():
    state["real"] -= state["height"]*0.25
    return "Success", 200

@app.route('/moveRight', methods=["POST"])
def moveRight():
    state["real"] += state["height"]*0.25
    return "Success", 200

@app.route('/zoomIn', methods=["POST"])
def zoomIn():
    state["height"] /= 1.4
    return "Success", 200

@app.route('/zoomOut', methods=["POST"])
def zoomOut():
    state["height"] *= 1.4
    return "Success", 200

@app.route('/smallerImage', methods=["POST"])
def smallerImage():
    state["dim"] /= 1.25
    return "Success", 200

@app.route('/largerImage', methods=["POST"])
def largerImage():
    state["dim"] *= 1.25
    return "Success", 200

@app.route('/moreIterations', methods=["POST"])
def moreIterations():
    state["iter"] *= 2
    return "Success", 200

@app.route('/lessIterations', methods=["POST"])
def lessIterations():
    state["iter"] /= 2
    return "Success", 200

@app.route('/changeColorMap', methods=["POST"])
def changeColorMap():
    state["colormap"] = request.get_json()["colormap"]
    return "Success", 200



@app.route('/mandelbrot', methods=["GET"])
def mandelbrot():
    mandelbrot_url = "%s/%s:%s:%s:%s:%s" % (state["colormap"], \
                                            str(state["real"]), \
                                            str(state["imag"]), \
                                            str(state["height"]), \
                                            str(state["dim"]), \
                                            str(state["iter"]))

    results = client.list_objects(Bucket='bucket', Prefix=mandelbrot_url)
    image = None
    if "Contents" in results:
        image = client.get_object(Key = mandelbrot_url, Bucket = "bucket")["Body"]
        return image
    else:
        image = requests.get("http://127.0.0.1:34000/mandelbrot/" + mandelbrot_url)
        client.put_object(Key = mandelbrot_url, Body = image.content, Bucket = "bucket")
        return image.content
@app.route('/storage', methods=["GET"])
def storage():
    results = client.list_objects(Bucket='bucket')
    cache_list = []
    if "Contents" in results:
        for k in results["Contents"]:
            key_pair = k["Key"]
            image_pair = "data:image/png;base64," + str(client.get_object(Key = k["Key"], Bucket = "bucket")["Body"])
            cache_list.append({"key": key_pair, "image": image_pair})
    print(cache_list)
    return cache_list, 200

@app.route('/getState', methods=["GET"])
def getState():
    return state, 200

@app.route('/clearCache', methods=["GET"])
def clearCache():
    results = client.list_objects(Bucket='bucket')
    if "Contents" in results:
        for k in results["Contents"]:
            client.delete_object(Bucket = "bucket", Key = k["Key"])
    return "Success", 200


@app.route('/resetTo', methods=["POST"])
def resetTo():
    params = request.get_json()
    for k in params:
        state[k] = params[k]
    return "Success", 200