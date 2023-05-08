from flask import Flask, jsonify, render_template, request, make_response
import requests
import base64
from PIL import Image
import numpy as np
from io import BytesIO
import os
from sklearn.neighbors import KDTree

app = Flask(__name__)

middleURL = "http://127.0.0.1:5000/registerReducer"
d = {"name": "reducer", "url": "http://127.0.0.1:4000/reduceMosaic", "author": "elvinwc2"}
requests.put(middleURL, d)

@app.route("/reduceMosaic", methods=["POST"])
def reduce():
    baseImage = request.files["baseImage"]
    mosaic1 = request.files["mosaic1"]
    mosaic2 = request.files["mosaic2"]

    renderedTileSize = int(request.args.get("renderedTileSize"))
    tilesAcross = int(request.args.get("tilesAcross"))
    fileFormat = request.args.get("fileFormat")

    # Your reduce logic here:
    bimg = Image.open(baseImage.stream)
    mimg1 = Image.open(mosaic1.stream)
    mimg2 = Image.open(mosaic2.stream)
    width, height = bimg.size

    baseTileSize = width / tilesAcross
    baseTilesVert = int(height / baseTileSize)
    reducedImage = Image.new('RGBA', mimg1.size, (255, 255, 255, 255))

    for i in range(baseTilesVert):
      for j in range(tilesAcross):
        x_coord, y_coord = j*baseTileSize, i*baseTileSize
        crop = bimg.crop((x_coord, y_coord, x_coord + baseTileSize, y_coord + baseTileSize))
        crop_array = np.array(crop)
        avc = crop_array.mean(axis=(0, 1)).astype(int)[0:3]
        x_coord, y_coord = j*renderedTileSize, i*renderedTileSize
        crop1 = mimg1.crop((x_coord, y_coord, x_coord + renderedTileSize, y_coord + renderedTileSize))
        crop2 = mimg2.crop((x_coord, y_coord, x_coord + renderedTileSize, y_coord + renderedTileSize))
        avc1 = np.array(crop1).mean(axis=(0, 1)).astype(int)[0:3]
        avc2 = np.array(crop2).mean(axis=(0, 1)).astype(int)[0:3]
        if np.linalg.norm(np.array(avc1) - np.array(avc)) < np.linalg.norm(np.array(avc2) - np.array(avc)):
          # use mimg1
          reducedImage.paste(crop1, (x_coord, y_coord))
        else:
          reducedImage.paste(crop2, (x_coord, y_coord))

    img_file = BytesIO()
    reducedImage.save(img_file, format=fileFormat)

    r = make_response(img_file.getvalue())

    # Return a reduced mosaic that combines the best of mosaic1 and mosaic2:
    return make_response(r)