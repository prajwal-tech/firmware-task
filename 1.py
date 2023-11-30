import serial
import time
import crc16

# Establish UART connection
ser = serial.Serial('COM3', baudrate=9600)  # Replace 'COM3' with the actual port name

# Define the data to transmit
data_to_send = "Hello from PC!"

# Calculate CRC checksum
crc16xmodem = crc16.xmodem(data_to_send.encode('ascii'))
crc_bytes = crc16xmodem.to_bytes(2, 'big')

# Send data with CRC checksum appended
combined_data = data_to_send.encode('ascii') + crc_bytes
ser.write(combined_data)

# Receive data from MCU
received_data = ser.readline().decode('ascii')

# Calculate CRC checksum for received data
received_crc = received_data[-2:]
calculated_crc = crc16.xmodem(received_data[:-2].encode('ascii')).to_bytes(2, 'big')

# Verify CRC checksum
crc_match = received_crc == calculated_crc

# Calculate transmission time
start_time = time.time()
received_data = ser.readline().decode('ascii')
end_time = time.time()
transmission_time = end_time - start_time

# Print received data and CRC verification status
print("Received data:", received_data)
print("CRC verification:", crc_match)
print("Transmission time:", transmission_time)

# Close the serial connection
ser.close()
