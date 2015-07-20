#!/usr/bin/python

#
# Created: 23.06.2013 20:00:51
#  Author: Dennis Schuett, dev.shyd.de
#  port for arduino by kovo
# 

import os
import serial
import string
import time

# bytes sent when radio keys are pressed
CDC_END_CMD = chr(0x14)
CDC_PLAY = chr(0xE4)
CDC_STOP = chr(0x10)
CDC_NEXT = chr(0xF8)
CDC_PREV = chr(0x78)
CDC_SEEK_FWD = chr(0xD8)
CDC_SEEK_RWD = chr(0x58)
CDC_CD1 = chr(0x0C)
CDC_CD2 = chr(0x8C)
CDC_CD3 = chr(0x4C)
CDC_CD4 = chr(0xCC)
CDC_CD5 = chr(0x2C)
CDC_CD6 = chr(0xAC)
CDC_CDSET = chr(0x38)
CDC_SCAN = chr(0xA0)
CDC_SFL = chr(0x60)
CDC_PLAY_NORMAL = chr(0x08)
CDC_PREV_CD = chr(0x18)


# control bytes to set the display
CMD_SCAN = chr(0xCA)
CMD_SHFFL = chr(0xCB)
CMD_PLAY = chr(0xCC)
CMD_STOPED = chr(0xCD)
CMD_SHFFL_OFF = chr(0xCE)

CD_MASK = 0xC0

cd_filename = "./last_cd"
global ser
ser = None
play_state = 'stoped';
timeout_seek = 0.5
timeout_normal = 2

# send current cd no. to radio and save it for next reboot
def set_cd_no(val):
	ser.write(val)
	fo = open(cd_filename, "wb")
	fo.write(val)
	fo.close()

# load new cd-dir
def play_cd(cd_no):
	c = ser.read()
	if c == CDC_END_CMD:
		if ser.read() == CDC_CDSET:
			play_plst(cd_no)

	elif c == CDC_CDSET: #audi concert unit sent just this no end_cmd
		play_plst(cd_no)


# play new playlst
def play_plst(plst_no):
	r = os.popen("mpc lsplaylists| grep cd" + str(plst_no)).read()
	if r != "":
		set_cd_no(chr(CD_MASK + plst_no))
		os.popen("mpc clear")
		os.popen("mpc load cd" + str(plst_no))
		os.popen("mpc play")
	else:
		print "no playlist no. {}".format(plst_no)

# init serial and last cd no. for correct radio display
def connect():
	while(1):
		try:
			global ser
			ser = serial.Serial('/dev/ttyUSB0', 9600)
			ser.timeout = timeout_normal #we need a timeout to update the track no.
			ser.writeTimeout = timeout_normal
			print play_state
			try:
				fo = open(cd_filename, "rb")
				cd = fo.read(1)
				fo.close()
				ser.write(cd)
			except:
				print "no last cd no. seting to one"
				play_cd(1)
			return
		except:
			time.sleep(2) #wait before retrying

try:
	
	#os.popen("/etc/init.d/mpd restart") #restart mpd due to alsa-equalizer
	connect()

	# read cmds from the radio and act like a cd changer
	while(1):
		try:
			c = ser.read()	
			
			if c == CDC_PLAY:
				if ser.read() == CDC_END_CMD:
					#os.popen("/etc/init.d/mpd start")
					os.popen("mpc play")
					os.popen("mpc repeat on")
					

			elif c == CDC_STOP:
				if ser.read() == CDC_END_CMD:
					os.popen("mpc pause")
					# stop service to sync current status
					#os.popen("/etc/init.d/mpd stop")

			elif c == CDC_NEXT:
				os.popen("mpc next")

			elif c == CDC_PREV:
				os.popen("mpc prev")

			elif c == CDC_SEEK_FWD:
				#ser.timeout = timeout_seek
				#while(1):
				os.popen("mpc seek +00:00:01")
				#	if ser.read() == CDC_PLAY and ser.read() == CDC_END_CMD:
				#		break
				#ser.timeout = timeout_normal

			elif c == CDC_SEEK_RWD:
				#ser.timeout = timeout_seek
				#while(1):
				os.popen("mpc seek -00:00:01")
				#	if ser.read() == CDC_PLAY and ser.read() == CDC_END_CMD:
				#		break
				#ser.timeout = timeout_normal

			elif c == CDC_CD1:
				play_cd(1)

			elif c == CDC_CD2:
				play_cd(2)

			elif c == CDC_CD3:
				play_cd(3)

			elif c == CDC_CD4:
				play_cd(4)

			elif c == CDC_CD5:
				play_cd(5)

			elif c == CDC_CD6:
				play_cd(6)

			elif c == CDC_CDSET:
				if ser.read() == CDC_END_CMD:
					fo = open(cd_filename, "rb")
					cd = fo.read(1)
					fo.close()
					if cd == chr(0xC1):
						play_plst(2)
					elif cd == chr(0xC2):
						play_plst(3)
					elif cd == chr(0xC3):
						play_plst(4)
					elif cd == chr(0xC4):
						play_plst(5)
					elif cd == chr(0xC5):
						play_plst(6)
					elif cd == chr(0xC6):
						play_plst(1)

			elif c == CDC_PREV_CD:
				if ser.read() == CDC_END_CMD:
					fo = open(cd_filename, "rb")
					cd = fo.read(1)
					fo.close()
					if cd == chr(0xC1):
						play_plst(6)
					elif cd == chr(0xC2):
						play_plst(1)
					elif cd == chr(0xC3):
						play_plst(2)
					elif cd == chr(0xC4):
						play_plst(3)
					elif cd == chr(0xC5):
						play_plst(4)
					elif cd == chr(0xC6):
						play_plst(5)
			elif c == CDC_SCAN:
				os.popen("mpc update")

			elif c == CDC_SFL:
				os.popen("mpc random on")

			elif c == CDC_PLAY_NORMAL:
				if ser.read() == CDC_END_CMD:
					os.popen("mpc play")
					os.popen("mpc random off")

			#get current track no. (radio display will be delayed up to {timeout_normal})
			mpc = os.popen("mpc |grep ] #").read()
			try:
				#if play_state is not 'playing':
				#	play_state = 'playing'
				#	ser.write(CMD_PLAY)
				mpc = mpc.split("/", 1)
				mpc = mpc[0].split("#", 1)
				tr = chr(string.atoi(mpc[1], 16))
				ser.write(tr)
			except:
				print "not playing[1]"
				play_state = 'stoped'
				#no tr number = mpd is stoped or not playing
				ser.write(CMD_STOPED)

			#get current playmode
			mpc = os.popen("mpc | grep random:").read()
			try:
				mpc = mpc.split("random: ", 1)
				if mpc[1].startswith("on"):
					ser.write(CMD_SHFFL)
				elif mpc[1].startswith("off"):
					ser.write(CMD_SHFFL_OFF)
			except:
				print "not playing[2]"
			mpc = os.popen("mpc |grep \"] #\"").read()
			try:
				mpc = mpc.split("]", 1)
				mpc = mpc[0].split("[", 1)
				if mpc[1] == "playing":
					play_state='playing'
					ser.write(CMD_PLAY)
				elif mpc [1] == "paused":
					play_state = 'paused'
					ser.write(CMD_STOPED)
			except:
				print "no playing [3]"
				play_state = 'stoped'
				ser.write(CMD_STOPED)
		except (serial.SerialException, serial.SerialTimeoutException):
			print "serial port unavailable, reconnecting..."
			ser.close()
			connect()
		except:
			raise
except (KeyboardInterrupt):
	if ser is not None:
		ser.close()
		print "port closed!"
	print "KeyboardInterrupt detected, exiting..."
