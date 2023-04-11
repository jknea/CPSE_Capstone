import matplotlib.pyplot as plt
from statistics import mean, median, mode, stdev
    

def WaveshareCANData():
    f = open("../TestData/WaveshareCANData/CAN_SPI_2M.txt", 'r')
    data1 = f.readlines()[0].split(',')
    data1 = [float(i)*1000000 for i in data1[:-1]]
    f.close()

    f = open("../TestData/WaveshareCANData/CAN_SPI_4M.txt", 'r')
    data2 = f.readlines()[0].split(',')
    data2 = [float(i)*1000000 for i in data2[:-1]]
    f.close()

    f = open("../TestData/WaveshareCANData/CAN_SPI_8M.txt", 'r')
    data3 = f.readlines()[0].split(',')
    data3 = [float(i)*1000000 for i in data3[:-1]]
    f.close()

    f = open("../TestData/WaveshareCANData/CAN_SPI_10M.txt", 'r')
    data4 = f.readlines()[0].split(',')
    data4 = [float(i)*1000000 for i in data4[:-1]]
    f.close()

    print("Max of data is : ", max(data1))
    print("Mean of data is : ", mean(data1))
    print("Median of data is : ", median(data1))
    print("Mode of data is : ", mode(data1))
    print("Std dev of data is : ", stdev(data1))
    print("mean + 6*Std dev of data is : ",mean(data1) + 6*stdev(data1))
    print()
    print("Max of data is : ", max(data2))
    print("Mean of data is : ", mean(data2))
    print("Median of data is : ", median(data2))
    print("Mode of data is : ", mode(data2))
    print("Std dev of data is : ", stdev(data2))
    print()
    print("Max of data is : ", max(data3))
    print("Mean of data is : ", mean(data3))
    print("Median of data is : ", median(data3))
    print("Mode of data is : ", mode(data3))
    print("Std dev of data is : ", stdev(data3))
    print()
    print("Max of data is : ", max(data4))
    print("Mean of data is : ", mean(data4))
    print("Median of data is : ", median(data4))
    print("Mode of data is : ", mode(data4))
    print("Std dev of data is : ", stdev(data4))
    print()


    plt.boxplot([data1, data2, data3, data4])
    plt.xticks([1, 2, 3, 4], ["SPI_2M", "SPI_4M", "SPI_8M", "SPI_10M"])
    plt.grid()
    plt.title("Waveshare RS485/CAN Shield CAN Bandwidth Testing\nPi Zero W")
    plt.ylabel("Time (us)")
   # plt.show()



def TeensyCANData():
    f = open("../TestData/TeensyCANData/CAN_1ms.txt", 'r')
    send_time = []
    wait_time = []
    recv_time = []
    total_time = []
    data = f.readlines()
    for i in range(len(data)):
        data[i] = data[i][:-1]   # remove newline character
        nums = [int(val) for val in data[i].split(",")]
        send_time.append(nums[0])
        wait_time.append(nums[1])
        recv_time.append(nums[2])
        total_time.append(nums[3])
    f.close()
    print("Max of data is : ", max(total_time))
    print("Mean of data is : ", mean(total_time))
    print("Median of data is : ", median(total_time))
    print("Mode of data is : ", mode(total_time))
    print("Std dev of data is : ", stdev(total_time))
    print("mean + 6*Std dev of data is : ",mean(total_time) + 6*stdev(total_time))
    print()
    plt.boxplot([total_time])
    plt.xticks([1], ["CAN_1MS_DELAY"])
    plt.grid()
    plt.title("Teensy 4.1 CAN Test (send + receive)\n1Mbps datarate")
    plt.ylabel("Time (us)")
 #   plt.show()
    


def TeensyCANFDData():
    f = open("../TestData/TeensyCANFDData/CANFD1MS.txt", 'r')
    send_time = []
    wait_time = []
    recv_time = []
    total_time = []
    data = f.readlines()
    for i in range(len(data)):
        data[i] = data[i][:-1]   # remove newline character
        nums = [int(val) for val in data[i].split(",")]
        send_time.append(nums[0])
        wait_time.append(nums[1])
        recv_time.append(nums[2])
        total_time.append(nums[3])
    f.close()
    print("Max of data is : ", max(total_time))
    print("Mean of data is : ", mean(total_time))
    print("Median of data is : ", median(total_time))
    print("Mode of data is : ", mode(total_time))
    print("Std dev of data is : ", stdev(total_time))
    print("mean + 6*Std dev of data is : ",mean(total_time) + 6*stdev(total_time))
    print()
    plt.boxplot([total_time])
    plt.xticks([1], ["CAN_1MS_DELAY"])
    plt.grid()
    plt.title("Teensy 4.1 CANFD Test (send + receive)\n64B payload, 80MHz clock, 8Mbps datarate")
    plt.ylabel("Time (us)")
#    plt.show()


    
def TeensyIMUSPIData():
    f = open("../TestData/TeensyIMUSPIData/SPI_1MS.txt", 'r')
    total_time = f.readlines()[0].split(',')
    total_time = [float(i) for i in total_time[:-1]]
    f.close()
    print("Max of data is : ", max(total_time))
    print("Mean of data is : ", mean(total_time))
    print("Median of data is : ", median(total_time))
    print("Mode of data is : ", mode(total_time))
    print("Std dev of data is : ", stdev(total_time))
    print("mean + 6*Std dev of data is : ",mean(total_time) + 6*stdev(total_time))
    print()

    plt.boxplot([total_time])
    plt.xticks([1], ["SPI_1MS_DELAY"])
    plt.grid()
    plt.title("Teensy 4.1 SPI IMU Test (LIS3DHTR)\n1.6KHz ODR")
    plt.ylabel("Time (us)")
 #   plt.show()


if __name__ == "__main__":
    #directory = input("Enter the directory to process: ")
    #if directory == "WaveshareCANData":
        WaveshareCANData()
    #elif directory == "TeensyCANData":
        TeensyCANData()
    #elif directory == "TeensyCANFDData":
        TeensyCANFDData()
    #elif directory == "TeensyIMUSPIData":
        TeensyIMUSPIData()
