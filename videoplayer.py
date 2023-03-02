#import RPi.GPIO as GPIO
import os
import serial
import signal
import sys
from subprocess import Popen
from omxplayer.player import OMXPlayer
import time

#GPIO.setmode(GPIO.BCM)

#GPIO.setup(17, GPIO.IN, pull_up_down=GPIO.PUD_UP)
#GPIO.setup(18, GPIO.IN, pull_up_down=GPIO.PUD_UP)
#GPIO.setup(24, GPIO.IN, pull_up_down=GPIO.PUD_UP)

# username: pi
# password: raspberry
# First, you’ll need to install the Raspberry Pi operating system (previously called Raspbian) onto a microSD card. 
# The Raspberry Pi website provides a tool called Raspberry Pi Imager to prepare the microSD card on Windows, macOS, and Ubuntu.
# Download and run the Raspberry Pi Imager, and select the Raspberry Pi OS (other) option from the menu, then the Raspberry Pi OS Lite (32-bit) option. 
# The lite option is all we need, since we don’t need a full desktop for this project. The Imager will prepare the SD card with the OS.
# Not bullseye. need older version
# OS loads on SD card, nothing else goes on card. Transfer files via USB hard drive.

# Install omxplayer
# sudo apt-get update
# sudo apt-get install python3-pip omxplayer fbi

# Mount USB drive
# ls -l /dev/disk/by-uuid/
# sudo mkdir /media/usb
# sudo chown -R pi:pi /media/usb
# sudo mount /dev/sda1 /media/usb -o uid=pi,gid=pi
# sudo umount /media/usb

# find usb
# ls /dev/tty*

# Need to use python3
# sudo apt-get install python3-serial

mov1 = ("/home/pi/videos/Gabby_1.mp4")
mov2 = ("/home/pi/videos/Gabby_2.mp4")
mov3 = ("/home/pi/videos/Gabby_3.mp4")
mov4 = ("/home/pi/videos/Gabby_4.mp4")
mov5 = ("/home/pi/videos/Gabby_5.mp4")
mov6 = ("/home/pi/videos/Gabby_6.mp4")
mov7 = ("/home/pi/videos/Gabby_7.mp4")
mov8 = ("/home/pi/videos/Gabby_8.mp4")
mov9 = ("/home/pi/videos/Gabby_9.mp4")
mov10 = ("/home/pi/videos/Gabby_10.mp4")
mov11 = ("/home/pi/videos/Gabby_11.mp4")
mov12 = ("/home/pi/videos/Gabby_12.mp4")
mov13 = ("/home/pi/videos/Gabby_13.mp4")
mov14 = ("/home/pi/videos/Gabby_14.mp4")
mov15 = ("/home/pi/videos/Gabby_15.mp4")
mov_loop = ("/home/pi/videos/kaia_bday.mp4") # 300 s

movies = [mov1, mov2, mov3, mov4, mov5, mov6, mov7, mov8, mov9, mov10, mov11, mov12, mov13, mov14, mov15]
movie_times = [31, 11, 93, 20, 12, 24, 51, 37, 50, 85, 50, 33, 37, 56, 10]

#ser = serial.Serial('/dev/ttyACM0',9600)
ser = serial.Serial('/dev/ttyUSB0',9600)

#last_state1 = True
#last_state2 = True

#input_state1 = True
#input_state2 = True
#quit_video = True

gabby_count = 0

while True:
    #Read states of inputs
    #input_state1 = GPIO.input(17)
    #input_state2 = GPIO.input(18)
    #quit_video = GPIO.input(24)
    
    os.system('clear')
    omxc = OMXPlayer(mov_loop)
    while(True)
        
        time.sleep(0.2)
        
        read_serial=ser.readline().decode('utf-8')
        if "DOUG" in read_serial
            mov_num = int (read_serial[4:6])
            if mov_num > 0 and mov_num < 16:
                omxc.load(movies[mov_num - 1])
                time.sleep(movie_times[mov_num - 1])
                gabby_count = 399
        
        gabby_count = gabby_count + 1
        if gabby_count > 299:
            gabby_count = 0
            omxc.load(mov_loop)
            time.sleep(1.1)

    
    time.sleep(2.0)

    #If GPIO(17) is shorted to Ground
    #if input_state1 != last_state1:
    #    if (player and not input_state1):
    #        os.system('killall omxplayer.bin')
    #        omxc = Popen(['omxplayer', '-b', movie1])
    #        player = True
    #    elif not input_state1:
    #        omxc = Popen(['omxplayer', '-b', movie1])
    #        player = True

    #If GPIO(18) is shorted to Ground
    #elif input_state2 != last_state2:
    #    if (player and not input_state2):
    #        os.system('killall omxplayer.bin')
    #        omxc = Popen(['omxplayer', '-b', movie2])
    #        player = True
    #    elif not input_state2:
    #        omxc = Popen(['omxplayer', '-b', movie2])
    #        player = True

    #If omxplayer is running and GIOP(17) and GPIO(18) are not shorted to Ground
    #elif (player and input_state1 and input_state2):
    #    os.system('killall omxplayer.bin')
    #    player = False

    #GPIO(24) to close omxplayer manually - used during debug
    #if quit_video == False:
    #    os.system('killall omxplayer.bin')
    #    player = False

    #Set last_input states
    #last_state1 = input_state1
    #last_state2 = input_state2