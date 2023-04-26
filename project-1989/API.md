Just run ./start.sh from the project-1989 directory, wait for all the servers to start running, and then upload your image. The number of tiles in each mosaic and size of each tile can be changed in the code of app.py in the base directory.

Each of the MGs will be running on a port starting from 5000, then 5001, 5002, etc. The images that the MGs draw from are in the static folder, each in a separate labeled folder. 

The tilesAcross and renderedTileSize values are passed in the data parameter of the post request as strings