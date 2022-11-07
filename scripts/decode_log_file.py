"""
"""
from ctypes import *
import matplotlib.pyplot as plt
import datetime as DT

class YourStruct(Structure):
    _fields_ = [('epoch', c_long),
                ('mSecond', c_ulong),
                ('GPSYear', c_uint16),
                ('GPSMonth', c_uint8),
                ('GPSDay', c_uint8),
                ('GPSHour', c_uint8),
                ('GPSMinute', c_uint8),
                ('GPSSecond', c_uint8),
                ('GPSHundredth', c_uint8),
                ('voltage', c_float),
                ('GPSFix', c_bool),
                ('numSats', c_uint8),
                ('HDOP', c_uint8),
                ('latitude', c_long),
                ('longitude', c_long),
                ('GPSSpeed', c_long),
                ('GPSCourse', c_long),
                ('sysCal', c_uint8),
                ('gyroCal', c_uint8),
                ('accelCal', c_uint8),
                ('magCal', c_uint8),
                ('rawAccelX', c_float),
                ('rawAccelY', c_float),
                ('rawAccelZ', c_float),
                ('accelX', c_float),
                ('accelY', c_float),
                ('accelZ', c_float),
                ('rawGyroX', c_float),
                ('rawGyroY', c_float),
                ('rawGyroZ', c_float),
                ('gyroX', c_float),
                ('gyroY', c_float),
                ('gyroZ', c_float),
                ('rawMagX', c_float),
                ('rawMagY', c_float),
                ('rawMagZ', c_float),
                ('magX', c_float),
                ('magY', c_float),
                ('magZ', c_float),
                ('roll', c_float),
                ('pitch', c_float),
                ('yaw', c_float),
                ('linAccelX', c_float),
                ('linAccelY', c_float),
                ('linAccelZ', c_float),
                ('quatW', c_float),
                ('quatX', c_float),
                ('quatY', c_float),
                ('quatZ', c_float),
                ('imuTemp', c_float),
                ('state', c_uint8)]

with open('scripts/data/log_003.bin', 'rb') as file:
    epoch_data = []
    rawAccel_data = []
    accel_data = []

    x = YourStruct()
    while file.readinto(x) == sizeof(x):
        timestamp = DT.datetime.utcfromtimestamp(x.epoch) + DT.timedelta(milliseconds=x.mSecond)
        epoch_data.append(timestamp)
        rawAccel_data.append((x.rawAccelX, x.rawAccelY, x.rawAccelZ))
        accel_data.append((x.accelX, x.accelY, x.accelZ))
        # accel_data.append((x.linAccelX, x.linAccelY, x.linAccelZ))
        

# print(result)
# print(epoch_data)

fig, ax = plt.subplots(2)
ax[0].plot(epoch_data, rawAccel_data)
ax[1].plot(epoch_data, accel_data)

ax[0].set(xlabel='Time', ylabel='Acceleration (m/s/s)',
       title='Raw Acceleration')
ax[1].set(xlabel='Time', ylabel='Acceleration (m/s/s)',
       title='Filtered Acceleration')
ax[0].grid()
ax[1].grid()
ax[0].legend(["X", "Y", "Z"])

# fig.savefig("test.png")
plt.show()