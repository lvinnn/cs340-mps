from app import app
import pytest
import json, io, os
import hashlib
import requests

@pytest.fixture(autouse=True, scope='session')
def find_vm():
  global vmHost
  global historyStart
  HISTORY_START_TIMEOUT = -999

  # Find vmHost from mapping:
  with open("test_vm-mappings.json", "r") as f:
    vmMap = json.load(f)

  with open("NETID.txt", "r") as f:
    netid = f.read().strip().lower()

  m = hashlib.sha256()
  m.update(netid.encode())
  hash = m.hexdigest()

  if hash not in vmMap:
    pytest.fail(f"The NETID.txt file contains the NetID \"{netid}\", but no such entry can be mapped to a VM.")
  else:
    vmHost = "http://" + vmMap[hash] + ":5000"

  # Verify server is running:
  try:
    r = requests.get(f"{vmHost}/", verify=False, timeout=2)
    r.raise_for_status()
  except requests.exceptions.Timeout:
    pytest.fail(f"Timeout on connection to {vmHost}/.  Is your MP running on your VM?", pytrace=False)
  
  # Find starting point of history
  for i in range(100):
    r = requests.get(f"{vmHost}/extract/{i}")

    if r.status_code == 404:
      historyStart = i
      print(f"History starts at {historyStart}")
      return
    elif r.status_code != 200:
      r.raise_for_status()

  pytest.fail(f"Failed to find history starting point.")


def test_vm_no_hidden_gif():
  test_file = open('sample/no-uiuc-chunk.png', 'rb')
  data = {'png': ('no-uiuc-chunk.png', test_file.read())}
  url = f"{vmHost}/extract"

  print(f"Testing: POST {url}")
  r = requests.post(url, files=data)
  assert(r.status_code == 415)

def test_vm_hidden_gif_waf():
  test_file = open('sample/waf.png', 'rb')
  data = {'png': ('waf.png', test_file.read())}
  url = f"{vmHost}/extract"

  print(f"Testing: POST {url}")
  r = requests.post(url, files=data)
  assert(r.status_code == 200)
  assert(r.content == open('sample/waf.gif','rb').read())

def test_vm_hidden_gif_natalia():
  test_file = open('sample/natalia.png', 'rb')
  data = {'png': ('natalia.png', test_file.read())}
  url = f"{vmHost}/extract"

  print(f"Testing: POST {url}")
  r = requests.post(url, files=data)
  assert(r.status_code == 200)
  assert(r.content == open('sample/natalia.gif','rb').read())


def test_vm_no_saved_gif():
  url = f'{vmHost}/extract/{historyStart + 100}'

  print(f"Testing: GET {url}")
  r = requests.get(url)
  assert(r.status_code == 404)

def test_vm_get_gif_waf():
  url = f'{vmHost}/extract/{historyStart}'

  print(f"Testing: GET {url}")
  r = requests.get(url)
  assert(r.status_code == 200)
  assert(r.content == open('sample/waf.gif','rb').read())

def test_vm_get_gif_natalia():
  url = f'{vmHost}/extract/{historyStart + 1}'

  print(f"Testing: GET {url}")
  r = requests.get(url)
  assert(r.status_code == 200)
  assert(r.content == open('sample/natalia.gif','rb').read())
