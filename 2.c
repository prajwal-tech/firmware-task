#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <termios.h>

#define UART_PORT "/dev/ttyUSB0"  // Replace with your actual UART port
#define BAUD_RATE B115200

int main() {
    // Open the UART port
    struct termios options;
    int uart_fd = open(UART_PORT, O_RDWR | O_NOCTTY | O_NDELAY);
    if (uart_fd < 0) {
        perror("open");
        exit(1);
    }

    // Configure the UART port
    tcgetattr(uart_fd, &options);
    cfsetispeed(&options, BAUD_RATE);
    cfsetospeed(&options, BAUD_RATE);
    options.c_cflag |= (CLOCAL | CREAD);
    options.c_cflag &= ~PARENB;
    options.c_cflag &= ~CSTOPB;
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;
    tcsetattr(uart_fd, TCSANOW, &options);

    // Define the text to transmit
    char text[] = "This is the text to transmit.";

    // Calculate the CRC checksum
    unsigned long crc = crc32(text, strlen(text));

    // Start the timer
    clock_t start_time = clock();

    // Send the text and CRC checksum to the MCU
    write(uart_fd, text, strlen(text));
    write(uart_fd, &crc, sizeof(crc));

    // Receive the text and CRC checksum from the MCU
    char received_text[strlen(text)];
    unsigned long received_crc;
    read(uart_fd, received_text, strlen(text));
    read(uart_fd, &received_crc, sizeof(received_crc));

    // Calculate the CRC checksum of the received text
    unsigned long received_crc_calculated = crc32(received_text, strlen(received_text));

    // Stop the timer
    clock_t end_time = clock();

    // Calculate the transmission speed
    double transmission_speed = strlen(text) / ((end_time - start_time) / (CLOCKS_PER_SEC));

    // Verify the CRC checksum
    if (received_crc == received_crc_calculated) {
        printf("Data transmission successful.\n");
        printf("Transmission speed: %.2f bytes/second\n", transmission_speed);
    } else {
        printf("Data transmission failed. CRC checksum mismatch.\n");
    }

    close(uart