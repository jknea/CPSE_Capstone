**About**  
This is for using I2C with the Rapsberry Pi and BMX160 IMU.  I have successfully read accelerometer data from the BMX160 by recreating minimal code from the IMU GitHub Repo (see sources).  

**Setup**  
Ensure i2c-tools is installed:  
$ sudo apt-get install i2c-tools  

To get the i2c/smbus.h file, perform the following:  
$ sudo apt-get install libi2c-dev  

Ensure i2c interface is enabled in raspi-config.  


**ToDos:**  
- finish converting relevant BMX160 c++ code into c code.  


Sources:  
IMU link: https://wiki.dfrobot.com/BMX160_9_Axis_Sensor_Module_SKU_SEN0373  
IMU GitHub Repo: https://github.com/DFRobot/DFRobot_BMX160  
https://www.kernel.org/doc/html/v5.4/i2c/dev-interface.html  
https://www.kernel.org/doc/Documentation/i2c/dev-interface  
https://www.waveshare.com/wiki/Raspberry_Pi_Tutorial_Series:_I2C  
https://elinux.org/Interfacing_with_I2C_Devices  
https://cdn.sparkfun.com/assets/2/2/8/7/d/BMX160_datasheet.pdf  
