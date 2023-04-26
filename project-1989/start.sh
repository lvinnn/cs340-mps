#!/bin/bash

# Run the app.py in the current directory
python3 -m flask run &
echo "Started app.py in the current directory..."

# Run the app.py in the /MGs/MG1 directory
cd MGs/MG1
python3 -m flask run -p 5001 &
echo "Started app.py in /MGs/MG1 directory..."

# Run the app.py in the /MGs/MG2 directory
cd ../MG2
python3 -m flask run -p 5002 &
echo "Started app.py in /MGs/MG2 directory..."

# Run the app.py in the /MGs/MG3 directory
cd ../MG3
python3 -m flask run -p 5003 &
echo "Started app.py in /MGs/MG3 directory..."
# Run the app.py in the /MGs/MG3 directory

cd ../MG4
python3 -m flask run -p 5004 &
echo "Started app.py in /MGs/MG4 directory..."

# Run the app.py in the /MGs/MG3 directory
cd ../MG5
python3 -m flask run -p 5005 &
echo "Started app.py in /MGs/MG5 directory..."