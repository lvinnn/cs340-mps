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
    r = requests.post("http://127.0.0.1:500%d/" % i, files={'image': f}, data={'tilesAcross': str(10), 'renderedTileSize': str(10)})
    response.append({
      "image": "data:image/png;base64," + r.content.decode('utf-8')
    })

  return jsonify(response)

def mosaic(img, mg_path):
  global tilesAcross
  width, height = img.size
  tile_size = int(width/tilesAcross)

  for i in range(int(height/tile_size)):
    for j in range(tilesAcross):
      x_coord, y_coord = j*tile_size, i*tile_size
      crop = img.crop((x_coord, y_coord, x_coord + tile_size, y_coord + tile_size))
      crop_array = np.array(crop)
      average_color = crop_array.mean(axis=(0, 1)).astype(int)[0:3]
      global tree
      distances, ind = tree[mg_path].query(average_color.reshape(1, -1))
      min_img = av_colors[mg_path][ind[0][0]][0]

      with Image.open(mg_path+"/"+min_img) as sample_image:
        min_dim = min(sample_image.width, sample_image.height)
        img_cropped = sample_image.crop((0, 0, min_dim, min_dim))
        img_resized = img_cropped.resize((tile_size, tile_size), resample=Image.LANCZOS)
        crop = img_resized

      img.paste(crop, (x_coord,y_coord))
    
  img = img.crop((0,0,tilesAcross*tile_size,int(height/tile_size)*tile_size))

  buffer = BytesIO()
  img.save(buffer, format='PNG')
  img_bytes = buffer.getvalue()

  return base64.b64encode(img_bytes)