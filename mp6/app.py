from flask import Flask, render_template, request, jsonify
import os
import requests
from datetime import datetime, timedelta, time

app = Flask(__name__)

cache = ({}, 0)

# Route for "/" (frontend):
@app.route('/')
def index():
  return render_template("index.html")


# Route for "/weather" (middleware):
@app.route('/weather', methods=["POST"])
def POST_weather():
  global cache

  server_url = os.getenv('COURSES_MICROSERVICE_URL')
  course = request.form["course"]
  sub = ""
  num = ""
  for i in range(len(course)):
    if course[i].isnumeric():
      sub = course[0:i]
      num = course[i:]
      sub = rem_spaces(sub).upper()
      num = rem_spaces(num)
      break

  c = get_cached_weather()[0]
  if(c != {} and c["course"] == sub + " " + num):
    return c, 200

  resp = requests.get(server_url + "/%s/%s" % (sub, num))
  if resp.status_code != 200:
    return {}, 400

  t = military_time(resp.json()["Start Time"])
  next_class = days_left(resp.json()["Days of Week"], t).date()
  delta = days_left(resp.json()["Days of Week"], t) + timedelta(minutes=t[3]) - datetime.now()
  nextCourseMeeting = str(next_class) + " " + t[1]
  forecastTime = str(next_class) + " " + t[2]
  
  next_class = str(next_class) + t[0]

  resp = requests.get("https://api.weather.gov/points/40.1125,-88.2284")
  resp = requests.get(resp.json()["properties"]["forecastHourly"])
  temp = 0
  shortForecast = ""
  exp = ""
  if abs(delta.total_seconds()) > 518400:
    temp = "forecast unavailable"
    shortForecast = "forecast unavailable"
    exp = datetime.now() + timedelta(hours=144)
  else:
    for i in resp.json()["properties"]["periods"]:
      if i["startTime"] == next_class:
        temp = i["temperature"]
        shortForecast = i["shortForecast"]
        iso_string = resp.json()["properties"]["validTimes"]

        start_time_str = iso_string.split("+")[0]
        start_time = datetime.fromisoformat(start_time_str)

        duration_str = iso_string.split("/")[1]
        duration = timedelta()
        if "D" in duration_str:
          duration += timedelta(days=int(duration_str.split("D")[0].split("P")[1]))
        if "H" in duration_str:
          duration += timedelta(hours=int(duration_str.split("H")[0].split("T")[1]))

        exp = start_time + duration
        break
  
  output = {
    "course": "%s %s" % (sub, num),
    "nextCourseMeeting": nextCourseMeeting,
    "forecastTime": forecastTime,
    "temperature": temp,
    "shortForecast": shortForecast
  }
  cache = output, exp
  return output, 200


# Route for "/weatherCache" (middleware/backend):
@app.route('/weatherCache')
def get_cached_weather():
  global cache
  if cache[0] != {} and datetime.now() > cache[1]:
    cache = {}, 0
  if cache[0] == {}:
    return {}, 200

  return cache[0], 200


def rem_spaces(str):
  for i in range(len(str)):
    if str[i] != " ":
      str = str[i:]
      break
  for i in range(len(str)):
    if str[i] == " ":
      str = str[:i]
      break
  return str

def days_left(str, st):
  days_left = 7
  today = datetime.now()

  for i in str:
    day = -1
    if i == 'M':
      day = 0
    elif i == 'T':
      day = 1
    elif i == 'W':
      day = 2
    elif i == 'R':
      day = 3
    elif i == 'F':
      day = 4
    
    x = (day - today.weekday()) % 7
    if x == 0:
      if int(st[2][0:2]) < today.hour:
        x = 7
      elif int(st[2][0:2]) == today.hour:
        if int(st[2][3:5]) < today.minute:
          x = 7
    if x < days_left:
      days_left = x
  return today + timedelta(days_left)

def military_time(str):
  t = int(str[0:2])
  if str[6:] == "PM":
    t = int(str[0:2])+12
    if str[0:2] == "12":
      t = 12
  m = "T%s:00:00-05:00" % (t)
  m1 = "%s:00:00" % (t)
  n = "%s:%s:00" % (t, str[3:5])
  x = int(str[3:5]) + t*60
  return (m,n,m1,x)