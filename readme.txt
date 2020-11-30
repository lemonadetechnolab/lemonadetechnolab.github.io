This repository contains all the files necessary to succesfully emulate the basics of Industry 4.0 on the example of the Lemonade Factory. 

For the full functionality to be available, the following applications are required:

*Python 3.0
*Jupyter Notebooks
*Arduino (in order to check what the address is of the Arduino Serial Port)
*GitHub/GitBash

Prior to the start of the demonstration, the following things have to be executed:

1. Using Terminal/cmd start the local server by typing $ python3 server_local.py while located in the directory of the repository

!Unfortunately, this could not be automated via a python script, because the server keeps running until stopped and Jupyter Notebooks occupies the whole kernel with this process, herewith preventing any further scripts to be run

2. Open localhost:8000/index_local.html to be able to track the status of the machine and check the visualised graph of turbidity as well as the history of orders

Now the explanation of the different files present:

All files that start with barcode_ make up the endpoints of the barcodes that are placed on the various details of the factory. The pages of the pump details also contain a counter for how many times they have been turned on. In order for this information to be available after each run, a git push has to be done. Information on handling git is further described below.

datalog_turb.csv is the file, which is used for storing measurements of the turbidity sensor, which are then visualised on the index_local.html page

reset_csv.sh and reset_order.sh are bash scripts that can be executed from the Jupyter Notebook. They provide the possibility to replace the Turbidity datalog csv file and the status.html page in cases when the lists have become too long. Upon replacement, these files are stored with the respective date in the data_history folder.

index_local.html is the container for visualising the current state of the machine. It parses datalog_turb.csv and status.html and, using reloader.js, constantly checks if either of the files was updated and displays this on one page.

status.html is the container for the state of pumps (ON/OFF + number of turns), status of the liquid levels (OK/LOW/HIGH) and the order history

clean_code.ino is the code for the Arduino. It is already uploaded onto the Arduino, but in case any changes have to be made, it is available with comments.

Explanation of the GitHub functionality:

*GitHub Pages was used to host the index pages, which are accessible through the barcodes.

*Git connection is made through SSH, so a new SSH key has to be generated on each new device and added to the GitHub account to provide access. A guide to doing this can be found here: https://docs.github.com/en/free-pro-team@latest/github/authenticating-to-github/generating-a-new-ssh-key-and-adding-it-to-the-ssh-agent

*If it is the first time running the demonstration on a particular device, the repository has to be pulled by running the git_clone.sh either through terminal or through the cell in Jupyter Notebooks

*At the end of the session, please commit the changes by running the git_commit.sh either through terminal or through the cell in Jupyter Notebooks

!The reason that the git commits and push was not integrated within the looping script is due to the large delays (~5s) that it creates, during which the Serial Data Stream is not being read by the device, resulting in possible missed data, such as a new order or an update of the status of the pumps/sensors

Hopefully this readme explains the process of running the factory sufficiently. In case you have any further questions, please contact me at: velli.maxim@gmail.com 

Enjoy the Lemonade!
-Maxim Velli & the Technolab Team