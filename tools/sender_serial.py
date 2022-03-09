import serial
import time

serialPort = serial.Serial(
    port="COM1", baudrate=9600, bytesize=8, timeout=2, stopbits=serial.STOPBITS_ONE
)

#serialPort = serial.Serial('COM1')
limits = [100, 100, 100, 180, 2, 3]
values = [0.0, 0.0, 0.001, 0.0, 0.0, 0.0]
increments = [0.0, 0.0, 0.000, 1.0, 0.1, 0.1]
#values = [0.0] * 6

while True:
    for i, inc in enumerate(increments):
        val = values[i] + inc
        
        if (val > limits[i]):
            val = - 2 * limits[i] + val
        values[i] = val
    data = ','.join(['{:.3f}'.format(num) for num in values])
    res = serialPort.write((data + '\n').encode())
    print(f'Sending {res} bytes: {data}')
    time.sleep(0.1)