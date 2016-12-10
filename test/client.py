#!/usr/bin/env python

import socket , os

def readfile(location_file):
    with open(location_file, "rb") as f:
        byte = f.read()

    return byte

def doit():
    TCP_IP = '127.0.0.1'
    TCP_PORT = 6011
    BUFFER_SIZE = 1024
    message = readfile("program")
    BUFFER_SIZE = len(message)
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    try:
        s.connect((TCP_IP, TCP_PORT))
    except:
        print "connection faild to " + TCP_IP + " port: " + str(TCP_PORT)
        return
    print "sending task to the server"
    try:    
        s.send(message)
    except:
        print "connection faild"
    s.close()
    print "finished"

doit()

