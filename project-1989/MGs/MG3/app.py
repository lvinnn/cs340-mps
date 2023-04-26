from flask import Flask, jsonify, render_template, request
import base64
from PIL import Image
import numpy as np
from io import BytesIO
import os
from sklearn.neighbors import KDTree

app = Flask(__name__)
mg_path = "../../static/MG3"
tilesAcross = 10

av_colors = {}
tree = {}
av_colors[mg_path] = []
tree[mg_path] = []

for img_name in os.listdir(mg_path):
    with Image.open(mg_path+"/"+img_name) as img:
        img_array = np.array(img)
        average_color = img_array.mean(axis=(0, 1)).astype(int)
        av_colors[mg_path].append((img_name, average_color))
tree[mg_path] = KDTree([color[:3] for _, color in av_colors[mg_path]])

@app.route('/', methods=["POST"])
def mosaic():
    global mg_path
    global tilesAcross
    tilesAcross = int(request.form['tilesAcross'])
    renderedTileSize = int(request.form['renderedTileSize'])

    f = request.files['image']
    img = Image.open(f.stream)

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
                img_resized = img_cropped.resize((tile_size, tile_size))
                crop = img_resized

            img.paste(crop, (x_coord,y_coord))
        
    tilesVert = int(height/tile_size)
    img = img.crop((0,0,tilesAcross*tile_size,tilesVert*tile_size))
    img = img.resize((renderedTileSize*tilesAcross, renderedTileSize*tilesVert))

    buffer = BytesIO()
    img.save(buffer, format='PNG')
    img_bytes = buffer.getvalue()

    return base64.b64encode(img_bytes), 200