import matplotlib.pyplot as plt

f = open("CAN_SPI_2M.txt", 'r')
data1 = f.readlines()[0].split(',')
data1 = [float(i) for i in data1[:-1]]
f.close()

f = open("CAN_SPI_4M.txt", 'r')
data2 = f.readlines()[0].split(',')
data2 = [float(i) for i in data2[:-1]]
f.close()

f = open("CAN_SPI_8M.txt", 'r')
data3 = f.readlines()[0].split(',')
data3 = [float(i) for i in data3[:-1]]
f.close()

f = open("CAN_SPI_10M.txt", 'r')
data4 = f.readlines()[0].split(',')
data4 = [float(i) for i in data4[:-1]]
f.close()


#plt.boxplot(data1)
#plt.boxplot(data2)
#plt.boxplot(data3)
#plt.boxplot(data4)
plt.boxplot([data1, data2, data3, data4])

plt.xticks([1, 2, 3, 4], ["SPI_2M", "SPI_4M", "SPI_8M", "SPI_10M"])
plt.grid()
plt.title("Waveshare RS485/CAN Shield CAN Bandwidth Testing\nPi Zero W")
plt.ylabel("Time (s)")
plt.show()

