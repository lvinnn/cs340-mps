from flask import Flask, jsonify, request
import requests
import json

app = Flask(__name__)

add_data = {"name": "static", "url": "http://127.0.0.1:8000/", "author": "Elvin Chen"}

r = requests.put("http://127.0.0.1:5000/addMG", json = add_data)
print(r.content)

@app.route('/generate', methods=["GET"])
def generate():
    return {"geom": ["0AAAAA0", "6D988C1", "9432261", "1088841", "3000041", "C145141", "4365300"]}, 200
    