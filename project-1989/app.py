from flask import Flask, jsonify, render_template, request
import requests
import base64
from PIL import Image
import numpy as np
from io import BytesIO
import os
from sklearn.neighbors import KDTree

app = Flask(__name__)

folder_dirs = ["static/MG1", "static/MG2"]
tilesAcross = 10
av_colors = {}
tree = {}
for path in folder_dirs:
  av_colors[path] = []
  tree[path] = []

for path in folder_dirs:
  for img_name in os.listdir(path):
    with Image.open(path+"/"+img_name) as img:
      img_array = np.array(img)
      average_color = img_array.mean(axis=(0, 1)).astype(int)
      av_colors[path].append((img_name, average_color))
  tree[path] = KDTree([color[:3] for _, color in av_colors[path]])

@app.route('/', methods=["GET"])
def GET_index():
  '''Route for "/" (frontend)'''
  return render_template("index.html")

@app.route('/makeMosaic', methods=["POST"])
def POST_makeMosaic():
  response = []

  f = request.files["image"]
  for i in range(1, 6):
    f.seek(0)
    r = requests.post("http://127.0.0.1:500%d/" % i, files={'image': f}, data={'tilesAcross': str(15), 'renderedTileSize': str(15)})
    response.append({
      "image": "data:image/png;base64," + r.content.decode('utf-8')
    })

  return jsonify(response)