import serial
import time

# Open the COM port
ser = serial.Serial('COM1', 300)
ser.write(b'\n')
ser.flush()
time.sleep(1)
ser.read_all()
ser.write(b'*RST\n')
ser.flush()
print('Resetting the device...')
time.sleep(1)
ser.write(b'*CLS\n')
ser.flush()
print('Clearing the device...')
time.sleep(1)
ser.timeout = 5000

#ser.write(b'*RST;*CLS\n')
# Create a loop
# while True:
# 	# Send the "*IDN?" command
# 	ser.write(b'*IDN?\n')
# 	ser.flush()
# 	print('Sending the "*IDN?" command...')

# 	# Read the response
# 	response = ser.readline()

# 	# Print the response
# 	print(response)

# 	# Sleep for 1 second
# 	time.sleep(1)

ser.timeout = 1000
while 1:
	time.sleep(3)
	print('lol')
	ser.read_all()
	time.sleep(1)
	print('writing')
	ser.write(b'*IDN?\n')

# Close the COM port
ser.close()
