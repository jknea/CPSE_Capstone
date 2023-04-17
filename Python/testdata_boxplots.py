import matplotlib.pyplot as plt
from statistics import mean, median, mode, stdev


def TeensyCANData(verbose = 0):
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

    if verbose:
        print_stats(total_time)
 
    plt.boxplot([total_time])
    plt.xticks([1], ["CAN_1MS_DELAY"])
    plt.grid()
    plt.title("Teensy 4.1 CAN Test (send + receive)\n1Mbps datarate")
    plt.ylabel("Time (us)")
 #   plt.show()
    


def TeensyCANFDData(verbose = 0):
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
    if verbose:
        print_stats(total_time)
    plt.boxplot([total_time])
    plt.xticks([1], ["CAN_1MS_DELAY"])
    plt.grid()
    plt.title("Teensy 4.1 CANFD Test (send + receive)\n64B payload, 80MHz clock, 8Mbps datarate")
    plt.ylabel("Time (us)")
#    plt.show()


    
def TeensyIMUSPIData(verbose = 0):
    f = open("../TestData/TeensyIMUSPIData/SPI_1MS.txt", 'r')
    total_time = f.readlines()[0].split(',')
    total_time = [float(i) for i in total_time[:-1]]
    f.close()
    if verbose:
        print_stats(total_time)

    plt.boxplot([total_time])
    plt.xticks([1], ["SPI_1MS_DELAY"])
    plt.grid()
    plt.title("Teensy 4.1 SPI IMU Test (LIS3DHTR)\n1.6KHz ODR")
    plt.ylabel("Time (us)")
 #   plt.show()


def Teensy1B2M(verbose = 0):
    f = open("../TestData/1b2m0xA1.txt", 'r')
    total_time = f.readlines()[0].split(',')
    total_time = [float(i) for i in total_time[1:-1]]
    for i in range(len(total_time)):
        if (total_time[i] > 490):
            print(i)
    f.close()
    if verbose:
        print_stats(total_time)

    plt.boxplot([total_time])
    plt.xticks([1], ["Zero Torque Command"])
    plt.grid()
    plt.title("Teensy 4.1 One CAN Bus Two Motors\ntwo commands, two replies + reply data")
    plt.ylabel("Time (us)")
    plt.show()

def TeensyMotorComparison(verbose = 0):
    f = open("../TestData/TeensyCANData/CAN_1ms.txt", 'r')
    total_time0 = []
    data = f.readlines()
    f.close()
    for i in range(1, len(data)):
        data[i] = data[i][:-1]   # remove newline character
        nums = [int(val) for val in data[i].split(",")]
        total_time0.append(nums[3])
    f = open("../TestData/1b2m0xA1.txt", 'r')
    total_time1 = f.readlines()[0].split(',')
    total_time1 = [float(i) for i in total_time1[1:-1]]
    f.close()
    plt.boxplot([total_time0, total_time1])
    plt.xticks([1, 2], ["1B1M","1B2M"])
    plt.ylabel("Time (us)")
    plt.grid()
    plt.show()
    if verbose:
        print_stats(total_time0)
        print_stats(total_time1)



def TeensyETHData(verbose = 0):
    f = open("../TestData/eth_256B_1ms_delay_1000samples.txt", 'r')
    total_time = [float(i.replace('\n','')) for i in f.readlines()]
    f.close()
    if verbose:
        print_stats(total_time)        
    plt.boxplot([total_time])
    plt.xticks([1], ["1MS_DELAY"])
    plt.grid()
    plt.title("Teensy 4.1 ETH Test\n256B buffers")
    plt.ylabel("Time (us)")
    plt.show()


def Teensy2B1MData():
    f = open("../TestData/2b1m_can1_can3.txt", 'r')
    total_time0 = [float(i.replace('\n','')) for i in f.readlines()]
    f.close()
        
    plt.boxplot([total_time0])
    plt.xticks([1], ["1MS_DELAY"])
    plt.grid()
    plt.title("Teensy 4.1 Two CAN Buses, One Motor Per Bus")
    plt.ylabel("Time (us)")
    plt.show()
'''
    f = open("../TestData/TeensyCANData/CAN_1ms.txt", 'r')
    total_time = []
    data = f.readlines()
    f.close()
    for i in range(len(data)):
        data[i] = data[i][:-1]   # remove newline character
        nums = [int(val) for val in data[i].split(",")]
        total_time.append(nums[3])
        
    plt.boxplot([total_time0, total_time])
    plt.xticks([1, 2], ["2B1M", "1B1M"])
    plt.grid()
    plt.title("")
    plt.ylabel("Time (us)")
    plt.show()
'''

def print_stats(total_time):
    print("Max of data is : ", max(total_time))
    print("Mean of data is : ", mean(total_time))
    print("Median of data is : ", median(total_time))
    print("Mode of data is : ", mode(total_time))
    print("Std dev of data is : ", stdev(total_time))
    print("mean + 6*Std dev of data is : ",mean(total_time) + 6*stdev(total_time))
    print()




if __name__ == "__main__":
    TeensyETHData(1);
    Teensy2B1MData()
