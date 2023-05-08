#!/bin/bash

cd sharedMiddleware/1989-shared-middleware
python3 -m flask run -p 5000 &
cd ../..
echo "Started middleware..."

python3 -m flask run -p 4000 &
echo "Started reducer..."

cd MGs/MG1
python3 -m flask run -p 5001 &
echo "Started app.py in /MGs/MG1 directory..."

cd ../MG2
python3 -m flask run -p 5002 &
echo "Started app.py in /MGs/MG2 directory..."

cd ../MG3
python3 -m flask run -p 5003 &
echo "Started app.py in /MGs/MG3 directory..."

cd ../MG4
python3 -m flask run -p 5004 &
echo "Started app.py in /MGs/MG4 directory..."

cd ../MG5
python3 -m flask run -p 5005 &
echo "Started app.py in /MGs/MG5 directory..."

cd ../MG6
python3 -m flask run -p 5006 &
echo "Started app.py in /MGs/MG6 directory..."

cd ../MG7
python3 -m flask run -p 5007 &
echo "Started app.py in /MGs/MG7 directory..."

cd ../MG8
python3 -m flask run -p 5008 &
echo "Started app.py in /MGs/MG8 directory..."

cd ../MG9
python3 -m flask run -p 5009 &
echo "Started app.py in /MGs/MG9 directory..."

cd ../MG10
python3 -m flask run -p 5010 &
echo "Started app.py in /MGs/MG10 directory..."

cd ../MG11
python3 -m flask run -p 5011 &
echo "Started app.py in /MGs/MG11 directory..."

cd ../MG12
python3 -m flask run -p 5012 &
echo "Started app.py in /MGs/MG12 directory..."

cd ../MG13
python3 -m flask run -p 5013 &
echo "Started app.py in /MGs/MG13 directory..."