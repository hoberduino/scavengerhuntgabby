#import RPi.GPIO as GPIO
import os
import serial
import sys
from subprocess import Popen
import time

#GPIO.setmode(GPIO.BCM)

#GPIO.setup(17, GPIO.IN, pull_up_down=GPIO.PUD_UP)
#GPIO.setup(18, GPIO.IN, pull_up_down=GPIO.PUD_UP)
#GPIO.setup(24, GPIO.IN, pull_up_down=GPIO.PUD_UP)

mov1 = ("/home/pi/Videos/Gabby_1.mp4")
mov2 = ("/home/pi/Videos/Gabby_2.mp4")
mov3 = ("/home/pi/Videos/Gabby_3.mp4")
mov4 = ("/home/pi/Videos/Gabby_4.mp4")
mov5 = ("/home/pi/Videos/Gabby_5.mp4")
mov6 = ("/home/pi/Videos/Gabby_6.mp4")
mov7 = ("/home/pi/Videos/Gabby_7.mp4")
mov8 = ("/home/pi/Videos/Gabby_8.mp4")
mov9 = ("/home/pi/Videos/Gabby_9.mp4")
mov10 = ("/home/pi/Videos/Gabby_10.mp4")
mov11 = ("/home/pi/Videos/Gabby_11.mp4")
mov12 = ("/home/pi/Videos/Gabby_12.mp4")
mov13 = ("/home/pi/Videos/Gabby_13.mp4")
mov14 = ("/home/pi/Videos/Gabby_14.mp4")
mov15 = ("/home/pi/Videos/Gabby_15.mp4")
mov_loop = ("/home/pi/Videos/kaia_test.mp4")

ser = serial.Serial('/dev/ttyACM0',9600)

#last_state1 = True
#last_state2 = True

#input_state1 = True
#input_state2 = True
#quit_video = True

player = False

while True:
    #Read states of inputs
    #input_state1 = GPIO.input(17)
    #input_state2 = GPIO.input(18)
    #quit_video = GPIO.input(24)
    time.sleep(0.5)
    
    read_serial=ser.readline()
	s[0] = str(int (ser.readline(),16))
	print s[0]
	print read_serial

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