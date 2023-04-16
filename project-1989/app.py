from flask import Flask, jsonify, render_template, request
import base64

app = Flask(__name__)

@app.route('/', methods=["GET"])
def GET_index():
  '''Route for "/" (frontend)'''
  return render_template("index.html")

@app.route('/makeMosaic', methods=["POST"])
def POST_makeMosaic():
  f = request.files["image"]
  
  ...
  
  # Example - Prepare a single mosaic response:
  response = []
  with open("static/favicon.png", "rb") as f:
    buffer = f.read()
  b64 = base64.b64encode(buffer)
  response.append({
    "image": "data:image/png;base64," + b64.decode('utf-8')
  })

  return jsonify(response)
