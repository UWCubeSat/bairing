import serial
import struct
import time
import numpy as np
from ahrs.filters import Madgwick
from cube import CubeRenderer

sync = b'\xEF\xBE\xAD\xDE'

def reset(ser):
  ser.write(sync)
  ser.write(b'\x01\x00')
  ser.write(b'\x00')

def begin(ser):
  ser.write(sync)
  ser.write(b'\x03\x00')
  ser.write(b'\x10')
  ser.write(b'\x32\0x00')  # 50 ms period
  
def receive(ser):
  recognized = False
  pattern = [ 0, 0, 0, 0 ]
  while not recognized:
    pattern.append(ser.read(1))  # sync pattern
    pattern.pop(0)
    
    recognized = True
    for i in range(0, 4):
      recognized = recognized and (pattern[i] == sync[i:i+1])

  rcvd = ser.read(2)
  len = struct.unpack('<H', rcvd)[0]  # packet length
  
  cmd = ser.read(1)

  data = ser.read(len - 1)
  
  return [cmd, data]
    
def main():
  connecting = True
  while connecting:
    try:
      ser = serial.Serial('COM4')
      connecting = False
    except serial.serialutil.SerialException:
      print("Connection to COM4 timed out, retrying...")
  
  reset(ser)
  print("resetting")

  time.sleep(3)
  
  begin(ser)

  # q_last = np.array([0.7071, -0.7071, 0.0, 0.0])
  q_last = np.array([1., 0., 0., 0.])
  madgwick = Madgwick(Dt = 0.05, q0 = q_last)
  
  renderer = CubeRenderer()
  
  while True:
    [cmd, data] = receive(ser)
    if cmd == b'\x10':
      raw = struct.unpack('<fffffffff', data)  # 3 float vectors, 9 floats
      mag = raw[0:3]
      acc = raw[3:6]
      gyro = raw[6:9]
      
      # q_last = madgwick.updateMARG(q_last, gyr=gyro, acc=acc, mag=mag)
      q_last = madgwick.updateIMU(q_last, gyr=gyro, acc=acc)
      print(q_last)
      renderer.render(q_last)

if __name__ == '__main__':
  main()