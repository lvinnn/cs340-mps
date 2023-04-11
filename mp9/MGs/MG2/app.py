from flask import Flask, jsonify, request
import requests
import json
import random

app = Flask(__name__)

add_data = {"name": "dynamic", "url": "sp23-cs340-037.cs.illinois.edu:8000", "author": "Elvin Chen"}

r = requests.put("http://sp23-cs340-adm.cs.illinois.edu:34999/addMG", json = add_data)

@app.route('/generate', methods=["GET"])
def generate():
    grid = ["988088c", "1000004", "1000004", "0000000", "1000004", "1000004", "3220226"]

    for row in range(7):
        for col in range(7):
            binary = bin(int(grid[row][col], 16))[2:].zfill(4)
            # if row == 0:
            #     print(binary)
            binary_array = [bool(int(bit)) for bit in binary]
            count = 0
            for i in binary_array:
                if i == 1:
                    count += 1
            # print(count)
            if count < 2:
                for i in range(2-count):
                    direction = random.randint(0, 3)
                    x, y = 0,0
                    if direction == 0:
                        x, y = row-1, col
                    elif direction == 1:
                        x, y = row, col + 1
                    elif direction == 2:
                        x, y = row + 1, col
                    else:
                        x, y = row, col - 1
                    if 0 <= x < 7 and 0 <= y < 7:
                        check = bin(int(grid[x][y], 16))[2:].zfill(4)
                        check_array = [bool(int(bit)) for bit in check]
                        c = 0
                        for j in check_array:
                            if j == 1:
                                c += 1
                        if c < 2:
                            print(row, col, x, y)
                            check_array[(direction+2)%4] = 1
                            hex_digit = hex(int(''.join(['1' if bit else '0' for bit in check_array]), 2))[2:]
                            grid[x] = grid[x][:y] + hex_digit + grid[x][y+1:]
                            binary_array[direction] = 1
                            hex_digit = hex(int(''.join(['1' if bit else '0' for bit in binary_array]), 2))[2:]
                            grid[row] = grid[row][:col] + hex_digit + grid[row][col+1:]
    return {"geom": grid}, 200