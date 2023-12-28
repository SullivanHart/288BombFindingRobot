# # Serial library:  https://pyserial.readthedocs.io/en/latest/shortintro.html 
from unittest import result
import serial
import time # Time library   
import socket
import subprocess

import os  # import function for finding absolute path to this python script

from tkinter import *
from threading import *
import numpy as np
import matplotlib.pyplot as plt
import os  # import function for finding absolute path to this python script
from matplotlib.figure import Figure
from matplotlib.backends.backend_tkagg import (FigureCanvasTkAgg,
                                               NavigationToolbar2Tk)
import sys


absolute_path = os.path.dirname(__file__) # Absoult path to this python script
relative_path = "./"   # Path to sensor data file relative to this python script (./ means data file is in the same directory as this python script)
full_path = os.path.join(absolute_path, relative_path) # Full path to sensor data file
filename = 'sensor-scan.txt' # Name of file you want to store sensor data from your sensor scan command

# # # TCP Socket BEGIN (See Echo Client example): https://realpython.com/python-sockets/#echo-client-and-server
HOST = "192.168.1.1"  # The server's hostname or IP address
PORT = 288        # The port used by the server
cybot_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)  # Create a socket object
cybot_socket.connect((HOST, PORT))   # Connect to the socket  (Note: Server must first be running)
                    
cybot = cybot_socket.makefile("rbw", buffering=0)  # makefile creates a file object out of a socket:  https://pythontic.com/modules/socket/makefile
# # # TCP Socket END

print("Connected")

# Display IR Plot
def show_plot():
    for widget in plotFrame.winfo_children():
        widget.destroy()

    fig = Figure(figsize=(2,2), dpi=100)
    # A little python magic to make it more convient for you to ajust where you want the data file to live
    # Link for more info: https://towardsthecloud.com/get-relative-path-python 
    # absolute_path = os.path.dirname(__file__) # Absoult path to this python script
    # relative_path = "./"   # Path to sensor data file relative to this python script (./ means data file is in the same directory as this python script
    # full_path = os.path.join(absolute_path, relative_path) # Full path to sensor data file
    # filename = 'sensor-scan.txt' # Name of sensor data file

    # angle_degrees: a vector (i.e., array of numbers) for which each element is an angle at which the sensor makes a distance measurement.
    # Units: degrees
    angle_degrees = [] # Initially empty

    # angle_radians: a vector that contains converted elements of vector angle_degrees into radians 
    # Units radians
    angle_radians = [] # Initially empty

    # distance: a vector, where each element is the corresponding distance measured at each angle in vector angle_degrees
    # Units: meters
    distance = []      # Initially empty

    # Open file containing CyBot sensor scan from 0 - 180 degrees
    file_object = open(full_path + filename,'r') # Open the file: file_object is just a variable for the file "handler" returned by open()
    file_header = file_object.readline() # Read and store the header row (i.e., 1st row) of the file into file_header
    file_data = file_object.readlines()  # Read the rest of the lines of the file into file_data
    file_object.close() # Important to close file one you are done with it!!

    # For each line of the file split into columns, and assign each column to a variable
    for line in file_data:
        data = line.split('\t')    # Split line into columns (by default delineates columns by whitespace)
        if(data[0] == 'END\n'):
            break
        angle_degrees.append(float(data[0]))  # Column 0 holds the angle at which distance was measured
        distance.append(float(data[1]))       # Column 1 holds the distance that was measured at a given angle       

    # Convert python sequence (list of strings) into a numpy array
    angle_degrees = np.array(angle_degrees) # Avoid "TypeError: can't multiply sequence by non-int of type float"
                                            # Link for more info: https://www.freecodecamp.org/news/typeerror-cant-multiply-sequence-by-non-int-of-type-float-solved-python-error/
                                            
    angle_radians = (np.pi/180) * angle_degrees # Convert degrees into radians

    # Create a polar plot
    fig, ax = plt.subplots(subplot_kw={'projection': 'polar'}) # One subplot of type polar
    ax.plot(angle_radians, distance, color='r', linewidth=4.0)  # Plot distance verse angle (in radians), using red, line width 4.0
    ax.set_xlabel('Distance (cm)', fontsize = 14.0)  # Label x axis
    ax.set_ylabel('Angle (degrees)', fontsize = 14.0) # Label y axis
    # ax.xaxis.set_label_coords(0.5, 0.15) # Modify location of x axis label (Typically do not need or want this)
    ax.tick_params(axis='both', which='major', labelsize=14) # set font size of tick labels
    ax.set_rmax(70)                    # Saturate distance at 2.5 meters
    ax.set_ylim([0, 50])
    ax.set_rticks([10, 20, 30, 40, 50])   # Set plot "distance" tick marks at .5, 1, 1.5, 2, and 2.5 meters
    ax.set_rlabel_position(-22.5)     # Adjust location of the radial labels
    ax.set_thetamax(180)              # Saturate angle to 180 degrees
    ax.set_xticks(np.arange(0,np.pi+.1,np.pi/4)) # Set plot "angle" tick marks to pi/4 radians (i.e., displayed at 45 degree) increments
                                                # Note: added .1 to pi to go just beyond pi (i.e., 180 degrees) so that 180 degrees is displayed
    ax.grid(True)                     # Show grid lines

    # Create title for plot (font size = 14pt, y & pad controls title vertical location)
    ax.set_title("Scan from 0 to 180 Degrees", size=14, y=1.0, pad=-24) 
    plt.plot()  # Display plot

    canvas = FigureCanvasTkAgg(fig, master=plotFrame)
    canvas.draw()

    canvas.get_tk_widget().pack()

    toolbar = NavigationToolbar2Tk(canvas, plotFrame)

    toolbar.update()

    canvas.get_tk_widget().pack()
    irScan.configure(bg="grey", fg="black")

# interpret data from CyBot
def read_irScan():
    file_object = open(full_path + filename,'w') # Open the file: file_object is just a variable for the file "handler" returned by open()
    rx_message = bytearray(1)

    while (rx_message.decode() != "END\n"): # Collect sensor data until "END" recieved
            rx_message = cybot.readline()   # Wait for sensor response, readline expects message to end with "\n"
            file_object.write(rx_message.decode())  # Write a line of sensor data to the file

    file_object.close() # Important to close file once you are done with it!!
    show_plot()

def read_objectStats():
    objectDisplay.delete("1.0", "end")
    rx_message = bytearray(1)
    while (rx_message.decode() != "END\n"): # Collect sensor data until "END" recieved
            rx_message = cybot.readline()   # Wait for sensor response, readline expects message to end with "\n"
            objectDisplay.insert(INSERT, rx_message.decode())

def read_micScan():
    volumePrint = cybot.readline()
    volumeDisplay.delete("1.0", "end")
    volumeDisplay.insert(INSERT, volumePrint)

def read_micRead():
    volumePrint = cybot.readline()
    volumeDisplay.delete("1.0", "end")
    volumeDisplay.insert(INSERT, volumePrint)

def show_object_data():
    for widget in scanDataFrame.winfo_children():
        widget.destroy()

    rx_message = bytearray(1)
    objectData = list()

    while (rx_message.decode() != "END\n"):
        rx_message = cybot.readline()
        objectData.append(rx_message)
    
    stringData = ' '.join(objectData)
    display = Label(master=scanDataFrame,
                    text=stringData
                    )
    display.pack(padx=5, pady=5)

# Send commands to CyBot
def turn_left():
    send_message = "a"
    cybot.write(send_message.encode())
    cybot.readline()
    popup_win("Turned Left 5 degrees")

def turn_right():
    send_message = "d"
    cybot.write(send_message.encode())
    cybot.readline()
    popup_win("Turned Right 5 degrees")

def move_forward():
    send_message = "w"
    cybot.write(send_message.encode())
    message = cybot.readline().decode()

    # add send_uart_str in movement.c in void bump if statements
    if message == "left\n" or message == "right\n":
        popup_win(f'Bumped into an object on {message} side!\nMoving around object...')
    
    elif message == "Object!\n":
        popup_win(f'Object 20cm infront of bot!')

    else:
        popup_win(message)

def move_backward():
    send_message = "s"
    cybot.write(send_message.encode())
    message = cybot.readline().decode()
    popup_win(message)

def send_irScan():
    send_message = "m"
    irScan.configure(bg="green", fg="black")
    cybot.write(send_message.encode())
    read_irScan()
    read_objectStats()
    show_plot()

def send_micScan():
    send_message = "r"
    cybot.write(send_message.encode())
    read_micScan()

def send_micRead():
    send_message = "t"
    cybot.write(send_message.encode())
    read_micRead()

def send_playSong():
    send_message = "b"
    cybot.write(send_message.encode())

def popup_win(string):
    top = Toplevel(window)

    notification = Label(master=top, text=string)
    notification.pack()

# Frame configs
window = Tk()
rightSide = Frame(
    master=window,
    width=800,
    height=600,
)
leftSide = Frame(
    master=window,
    width=310,
    height=600,
)
irDisplayFrame = Frame(
    master=rightSide,
)
songFrame = Frame(
    master=leftSide,
    width=310,
    height=100,
    relief=GROOVE,
    borderwidth=5
)
movementFrame = Frame(
    master=leftSide,
    width=310,
    height=300,
    relief=GROOVE,
    borderwidth=5
)
volumeFrame = Frame(
    master=leftSide,
    width=310,
    height=200,
    relief=GROOVE,
    borderwidth=5
)
plotFrame = Frame(
    master=rightSide,
    relief=GROOVE,
    borderwidth=5,
)
scanDataFrame = Frame(
    master=window,
    relief=GROOVE,  
)

# Frame locations
leftSide.pack(side=LEFT, anchor = W)
rightSide.pack(side=RIGHT, anchor = E)
irDisplayFrame.pack(side=TOP, anchor = N)
songFrame.place(x=0, y=0)
movementFrame.place(x=0, y=100)
volumeFrame.place(x=0, y=400)
plotFrame.pack(side = BOTTOM, fill = Y)
noScan = Label(master=plotFrame, text="NO SCAN DATA", width = 90, height = 34)
noScan.pack(side=BOTTOM)
scanDataFrame.pack(side=BOTTOM, anchor=S)

# Object Creation
left = Button(
    master=movementFrame,
    command=turn_left,
    text="Turn Left",
    bg="grey",
    fg="black",
    height=5,
    width=12
)
left.place(x=0, y=100)

right = Button(
    master=movementFrame,
    command=turn_right,
    text="Turn Right",
    bg="grey",
    fg="black",
    height=5,
    width=12
)
right.place(x=200, y=100)

forward = Button(
    master=movementFrame,
    command=move_forward,
    text="Move Forward",
    bg="grey",
    fg="black",
    height=5,
    width=12
)
forward.place(x=100, y=0)

backward = Button(
    master=movementFrame,
    command=move_backward,
    text="Move Backward",
    bg="grey",
    fg="black",
    height=5,
    width=12
)
backward.place(x=100, y=200)

irScan = Button(
    master=rightSide,
    command=send_irScan,
    text="IR Scan",
    bg="grey",
    fg="black",
    height=5,
    width=20
)
irScan.pack(side = LEFT)

micScan = Button(
    master=volumeFrame,
    command=send_micScan,
    text="Mic Scan",
    bg="grey",
    fg="black",
    height=5,
    width=18
)
micScan.place(x=0, y=0)

micRead = Button(
    master=volumeFrame,
    command=send_micRead,
    text="Mic Read",
    bg="grey",
    fg="black",
    height=5,
    width=18
)
micRead.place(x=150, y=0)

volumeDisplay = Text(
    master = volumeFrame,
    bg="grey",
    fg="black",
    height=5,
    width=36
)
volumeDisplay.place(x = 0, y = 100)

objectDisplay = Text(
    master = rightSide,
    bg="grey",
    fg="black",
    height=5,
    width=60
)
objectDisplay.pack(side = RIGHT)

playSong = Button(
    master = songFrame,
    command= send_playSong,
    text="Play Song",
    bg="grey",
    fg="black",
    height=5,
    width=41
)
playSong.place(x = 0, y = 0)

# To delete
window.protocol("WM_DELETE_WINDOW", lambda:on_closing_window())

def on_closing_window():
    cybot.close()
    window.destroy()
    sys.exit()

window.mainloop()
