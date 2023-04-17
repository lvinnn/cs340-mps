from flask import Flask, jsonify, render_template, request
import base64
from PIL import Image
import numpy as np
from io import BytesIO
import os

app = Flask(__name__)

folder_dirs = ["static/MG1", "static/MG2"]
av_colors = {"static/MG1" : {}, "static/MG2" : {}}

for path in folder_dirs:
  for img_name in os.listdir(path):
    with Image.open(path+"/"+img_name) as img:
      img_array = np.array(img)
      average_color = img_array.mean(axis=(0, 1))
      average_color = average_color.astype(int)
      av_colors[path][str(img_name)] = average_color

@app.route('/', methods=["GET"])
def GET_index():
  '''Route for "/" (frontend)'''
  return render_template("index.html")

@app.route('/makeMosaic', methods=["POST"])
def POST_makeMosaic():
  f = request.files["image"]
  img = Image.open(BytesIO(f.read()))
  response = []
  
  global folder_dirs
  for path in folder_dirs:
    b64 = mosaic(img, path)
    response.append({
      "image": "data:image/png;base64," + b64.decode('utf-8')
    })

  return jsonify(response)

def mosaic(img, mg_path):
  tiles_across = 70
  width, height = img.size
  tile_size = int(width/tiles_across)

  for i in range(int(height/tile_size)):
    for j in range(tiles_across):
      x_coord, y_coord = j*tile_size, i*tile_size
      crop = img.crop((x_coord, y_coord, x_coord + tile_size, y_coord + tile_size))
      crop_array = np.array(crop)
      average_color = crop_array.mean(axis=(0, 1))
      average_color = average_color.astype(int)
      global av_colors
      min_dist = 999999999999
      min_img = None
      for col in av_colors[mg_path]:
        distance = np.linalg.norm(av_colors[mg_path][col][0:3] - average_color[0:3], axis=0)
        if distance < min_dist:
          min_dist = distance
          min_img = col
      
      with Image.open(mg_path+"/"+min_img) as sample_image:
        min_dim = min(sample_image.width, sample_image.height)
        img_cropped = sample_image.crop((0, 0, min_dim, min_dim))
        img_resized = img_cropped.resize((tile_size, tile_size), resample=Image.LANCZOS)
        crop = img_resized

      img.paste(crop, (x_coord,y_coord))
    
  img.crop((0,0,tiles_across*tile_size,int(height/tile_size)*tile_size))

  buffer = BytesIO()
  img.save(buffer, format='PNG')
  img_bytes = buffer.getvalue()

  return base64.b64encode(img_bytes)