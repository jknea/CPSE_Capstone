import matplotlib.pyplot as plt
from statistics import mean

def WaveshareCANData():
    f = open("WaveshareCANData/CAN_SPI_2M.txt", 'r')
    data1 = f.readlines()[0].split(',')
    data1 = [float(i) for i in data1[:-1]]
    f.close()

    f = open("WaveshareCANData/CAN_SPI_4M.txt", 'r')
    data2 = f.readlines()[0].split(',')
    data2 = [float(i) for i in data2[:-1]]
    f.close()

    f = open("WaveshareCANData/CAN_SPI_8M.txt", 'r')
    data3 = f.readlines()[0].split(',')
    data3 = [float(i) for i in data3[:-1]]
    f.close()

    f = open("WaveshareCANData/CAN_SPI_10M.txt", 'r')
    data4 = f.readlines()[0].split(',')
    data4 = [float(i) for i in data4[:-1]]
    f.close()

    plt.boxplot([data1, data2, data3, data4])

    plt.xticks([1, 2, 3, 4], ["SPI_2M", "SPI_4M", "SPI_8M", "SPI_10M"])
    plt.grid()
    plt.title("Waveshare RS485/CAN Shield CAN Bandwidth Testing\nPi Zero W")
    plt.ylabel("Time (s)")
    plt.show()



def TeensyCANData():
    f = open("TeensyCANData/CAN_1ms.txt", 'r')
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

    print(mean(total_time))
    plt.boxplot([total_time])
    plt.xticks([1], ["CAN_1MS_DELAY"])
    plt.grid()
    plt.title("Teensy 4.1 CAN Test")
    plt.ylabel("Time (s)")
    plt.show()

'''
    data1 = [float(i) for i in data1[:-1]]
    f.close()

    f = open("WaveshareCANData/CAN_SPI_4M.txt", 'r')
    data2 = f.readlines()[0].split(',')
    data2 = [float(i) for i in data2[:-1]]
    f.close()

    f = open("WaveshareCANData/CAN_SPI_8M.txt", 'r')
    data3 = f.readlines()[0].split(',')
    data3 = [float(i) for i in data3[:-1]]
    f.close()

    f = open("WaveshareCANData/CAN_SPI_10M.txt", 'r')
    data4 = f.readlines()[0].split(',')
    data4 = [float(i) for i in data4[:-1]]
    f.close()

    plt.boxplot([data1, data2, data3, data4])

    plt.xticks([1, 2, 3, 4], ["SPI_2M", "SPI_4M", "SPI_8M", "SPI_10M"])
    plt.grid()
    plt.title("Waveshare RS485/CAN Shield CAN Bandwidth Testing\nPi Zero W")
    plt.ylabel("Time (s)")
    plt.show()
'''


if __name__ == "__main__":
    directory = input("Enter the directory to process: ")
    if directory == "WaveshareCANData":
        WaveshareCANData()
    elif directory == "TeensyCANData":
        TeensyCANData()



'''
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


'''