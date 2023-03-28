from flask import Flask, render_template, send_file, request
import os

app = Flask(__name__)
cache = []

# Route for "/" for a web-based interface to this micro-service:
@app.route('/')
def index():
  return render_template("index.html")


# Extract a hidden "uiuc" GIF from a PNG image:
@app.route('/extract', methods=["POST"])
def extract_hidden_gif():
  global cache
  image = request.files['png']
  if not os.path.exists('temp'):
    os.makedirs('temp')
  image.save('temp/image.png')
  path = "temp/"+str(len(cache))+".gif"
  status = os.system("./png-extractGIF temp/image.png " + path)
  if status == 0:
    cache.append(path)
    return send_file(path), 200
  elif status == 65280:
    return "Unsupported Media Type", 415
  else:
    return "Unprocessable Entity", 422

# Get the nth saved "uiuc" GIF:
@app.route('/extract/<int:image_num>', methods=['GET'])
def extract_image(image_num):
  global cache
  print(cache)
  if image_num >= len(cache):
    return "Not Found", 404
  else:
    return send_file(cache[image_num])
  
