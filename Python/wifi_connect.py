import socket              
from time import sleep




s = socket.socket()        
host = '192.168.5.1'
port = 8888              

print("trying to connect")
s.connect((host, port))
print("connected!")

s.send("hiya".encode())

'''
while True: 
    try:
        print("From Server: ", s.recv(1024) 
        s.send(raw_input("Client please type: "))
    except:
        break
'''
for i in range(100):
    s.send("12345678".encode())
    sleep(1)
s.close()
