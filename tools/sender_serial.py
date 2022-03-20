import time
try:
    import serial
except ModuleNotFoundError:
    print('"serial" module not found, use "pip install pyserial"')
    exit(1)
    
integer_data = False
reordered_format = True

serialPort = serial.Serial(
    port="COM1", baudrate=9600, bytesize=8, timeout=2, stopbits=serial.STOPBITS_ONE
)
            #  x     y     z   yaw  pitch  roll
limits =     [20,   20,  100,  180,    2,    3]
increments = [ 1,  1.5,    1,    1,  0.1,  0.1]
values = [0] * 6
directions = [1] * 6

# valid format is 'x,y,z,yaw,pitch,roll\n', prepare some invalid lines
test_strings = [
    '1,2,3,4,5\n',
    '1,2,3,4,5,\n',
    '1,2,3,4,5,6,\n',
    '1,2,3,4,5,6,7\n',
    'a,b,c,a,b,c\n',
    'a.a,b.b,c.c,a.a,b.b,c.c\n',
    '1.1.1,2,3,4,5,6\n',
    '1,2,3,4,5,aaa\n',
    'NaN,2,3,4,5,6\n',
    '1,2,3,4,5,6',
    '1,2,3,4,5,6\n'
]

# Send some data in invalid format
for test in test_strings:
    print(f'Sending: {test}')
    serialPort.write(test.encode())
    time.sleep(0.2)

# Send long line without newline
serialPort.write(('10,20,30,40,50,60'*100).encode())
time.sleep(0.2)

# Start sending valid data
while True:
    for i, inc in enumerate(increments):
        values[i] = values[i] + directions[i] * inc
        if values[i] >= limits[i] or values[i] <= (-1 * limits[i]):
            directions[i] *= -1
    if reordered_format:
        # yaw,pitch,x,y,z,roll
        reordered = [values[3], values[4], values[0], values[1], values[2], values[5]]
    format_str = '{:d}' if integer_data else '{:.3f}'
    data = ','.join([format_str.format(round(num) if integer_data else num) for num in (reordered if reordered_format else values)])
    res = serialPort.write((data + '\n').encode())
    print(f'Sending {res} bytes: {data}')
    time.sleep(0.1)
