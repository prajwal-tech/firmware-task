#include <stdio.h>
#include <stdint.h>
#include <string.h> // Include string.h for memcpy function

// Define UART RX and TX pins
#define UART_RX_PIN 2
#define UART_TX_PIN 3

// Define CRC8 polynomial
const uint8_t crc8_polynomial[8] = {0x07, 0x0B, 0x13, 0x27, 0x4B, 0x95, 0xAF, 0xEF};

// Define EEPROM memory address
const int eeprom_address = 0;

// Function to calculate CRC8 checksum
uint8_t calculate_crc8(uint8_t *data, int length) {
  uint8_t crc = 0x00;

  for (int i = 0; i < length; i++) {
    uint8_t byte_to_process = data[i];

    for (int j = 0; j < 8; j++) {
      if ((crc & 0x80) ^ (byte_to_process & 0x80)) {
        crc = (crc << 1) ^ crc8_polynomial[7];
        byte_to_process <<= 1;
      } else {
        crc <<= 1;
        byte_to_process <<= 1;
      }
    }
  }

  return crc;
}

int main() {
  // Initialize UART communication
  const char* port_name = "COM1"; // Replace with the actual port name
  FILE *uart_stream = fopen(port_name, "r+b");
  if (!uart_stream) {
    printf("Error opening UART port '%s'\n", port_name);
    return 1;
  }

  setvbuf(uart_stream, NULL, _IONBF, 0); // Set unbuffered I/O for UART

  // Initialize EEPROM (simulated)
  uint8_t eeprom[512]; // Simulated EEPROM memory

  while (1) {
    // Receive data from PC and store in EEPROM with CRC8 checksum
    uint8_t data[1024];
    int length = fread(data, 1, 1024, uart_stream);

    if (length > 0) {
      // Calculate CRC8 checksum
      uint8_t checksum = calculate_crc8(data, length);

      // Store data with checksum in EEPROM
      memcpy(eeprom + eeprom_address, data, length);
      eeprom[eeprom_address + length] = checksum;

      // Print received data length
      printf("Received data length: %d\n", length);
    }

    // Read data from EEPROM and send to PC
    if (eeprom[eeprom_address] != 0) {
      uint8_t data[1024];
      int length = eeprom[eeprom_address + 1];

      // Read data from EEPROM
      memcpy(data, eeprom + eeprom_address, length);

      // Verify CRC8 checksum
      uint8_t checksum = eeprom[eeprom_address + length];
      uint8_t calculated_checksum = calculate_crc8(data, length);

      if (checksum == calculated_checksum) {
        // Send data to PC
        fwrite(data, 1, length, uart_stream);

        // Print transmitted data length
        printf("Transmitted data length: %d\n", length);
      } else {
        printf("Data corruption detected!\n");
      }

      // Reset EEPROM address
      eeprom[eeprom_address] = 0;
    }
  }

  fclose(uart_stream); // Close UART stream

  return 0;
}
