**About**  
This directory contains RPi C codes for client and multi-server devices.  The multi-client file is a single client to a multi-server.  I ran the multi-server file on RPi4 and connected my laptop and another RPi (3B+) as clients.  I was able to read data at 100Hz from both clients.  

For one device (pi zero w) sending two floats (to Pi4) and receiving one float:  
100Hz, actual 99 Hz  
200Hz, actual 194 Hz  
300Hz, actual 287 Hz  
400Hz, actual 376 Hz  
500Hz, actual 467 Hz  

**ToDos:**  
- Extend and test with a third client (remote base station)  
- Modify client codes to also receive data (control torques sent from multi-server?)  
- Try using Pi Zero W devices as the clients

**Sources:**  
Socket Programming in C/C++ - GeeksforGeeks  
https://www.geeksforgeeks.org/socket-programming-cc/  

Time delay in C - GeeksforGeeks  
https://www.geeksforgeeks.org/time-delay-c/  

TCP Server-Client implementation in C - GeeksforGeeks  
https://www.geeksforgeeks.org/tcp-server-client-implementation-in-c/  

Socket Programming in C/C++: Handling multiple clients on server without multi threading - GeeksforGeeks  
https://www.geeksforgeeks.org/socket-programming-in-cc-handling-multiple-clients-on-server-without-multi-threading/  

This Article is extremely helpful and provides a walkthrough of RPi WiFi AP setup:  
https://learn.sparkfun.com/tutorials/setting-up-a-raspberry-pi-3-as-an-access-point/all  
