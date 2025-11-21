/**
 * Raspberry Pi UART Communication Example
 * * This program uses the standard Linux 'termios' interface for UART I/O,
 * but includes the bcm2835 library initialization and cleanup for projects
 * that also require GPIO or other peripheral control via bcm2835.
 * * The program opens the default UART device (/dev/serial0), configures it
 * for 9600 baud, 8 data bits, no parity, and 1 stop bit (8N1), sends a
 * message, and attempts to read a response.
 * * To compile:
 * gcc -o uart_comm uart_comm.c -lrt -lbcm2835
 * * To run:
 * sudo ./uart_comm
 * * NOTE: Ensure the UART is enabled and configured correctly (e.g., using raspi-config
 * to disable the login shell over serial). The default UART pins are GPIO 14 (TX) 
 * and GPIO 15 (RX).
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>     // Used for read(), write(), close()
#include <fcntl.h>      // Used for open()
#include <errno.h>      // Used for errno
#include <termios.h>    // Used for serial port settings
#include <bcm2835.h>    // Used for initialization/cleanup

// Define the serial port device file
#define SERIAL_PORT "/dev/serial0"
#define BAUDRATE B9600 // 9600 baud
#define TX_MESSAGE "Hello, Microcontroller!\n"
#define RX_BUFFER_SIZE 256

// Function to initialize the serial port
int serial_init(const char *device, speed_t baud_rate) {
    // Open the serial port file descriptor
    int fd = open(device, O_RDWR | O_NOCTTY | O_SYNC);
    if (fd < 0) {
        perror("Error opening serial port");
        return -1;
    }

    // Configure port settings
    struct termios tty;
    if (tcgetattr(fd, &tty) != 0) {
        perror("Error from tcgetattr");
        close(fd);
        return -1;
    }

    // Set Baud Rate
    cfsetospeed(&tty, baud_rate);
    cfsetispeed(&tty, baud_rate);

    // Control Modes (CFLAG)
    tty.c_cflag &= ~PARENB;        // No parity
    tty.c_cflag &= ~CSTOPB;        // 1 stop bit
    tty.c_cflag &= ~CSIZE;         // Clear data size bits
    tty.c_cflag |= CS8;            // 8 data bits
    tty.c_cflag &= ~CRTSCTS;       // Disable hardware flow control
    tty.c_cflag |= CREAD | CLOCAL; // Enable receiver, ignore modem control lines

    // Local Modes (LFLAG) - Raw Input
    tty.c_lflag &= ~ICANON; // Disable canonical mode (non-buffered input)
    tty.c_lflag &= ~ECHO;   // Disable echo
    tty.c_lflag &= ~ECHOE;  // Disable erasure
    tty.c_lflag &= ~ECHONL; // Disable new-line echo

    // Input Modes (IFLAG)
    tty.c_iflag &= ~IGNBRK; // Disable break processing
    tty.c_iflag &= ~BRKINT; // Disable break signal
    tty.c_iflag &= ~PARMRK; // Disable parity check
    tty.c_iflag &= ~ISTRIP; // Disable stripping off eighth bit
    tty.c_iflag &= ~INLCR;  // Disable translating NL into CR on input
    tty.c_iflag &= ~IGNCR;  // Disable ignoring CR on input
    tty.c_iflag &= ~ICRNL;  // Disable translating CR into NL on input
    tty.c_iflag &= ~IXON;   // Disable software flow control (XON/XOFF)
    tty.c_iflag &= ~IXOFF;
    tty.c_iflag &= ~IXANY;

    // Output Modes (OFLAG) - Raw Output
    tty.c_oflag &= ~OPOST; // Disable implementation-defined output processing
    tty.c_oflag &= ~ONLCR; // Disable translating NL into CR-NL

    // VMIN and VTIME settings (Timeouts)
    // VMIN = 0, VTIME = 10 (tenths of a second) means read waits for 1.0s total.
    tty.c_cc[VMIN]  = 0;  // Return as soon as any data is available
    tty.c_cc[VTIME] = 10; // Wait up to 1 second

    // Apply the settings
    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        perror("Error from tcsetattr");
        close(fd);
        return -1;
    }

    return fd;
}

int main(void) {
    // 1. Initialize the bcm2835 library
    if (!bcm2835_init()) {
        printf("bcm2835_init failed. Are you running as root?\n");
        return 1;
    }
    printf("bcm2835 library initialized.\n");

    // 2. Initialize and configure the UART device
    int fd = serial_init(SERIAL_PORT, BAUDRATE);
    if (fd < 0) {
        // bcm2835_close will be called in the return block
        bcm2835_close();
        return 1;
    }

    printf("Serial port %s opened successfully at 9600 baud.\n", SERIAL_PORT);
    printf("Waiting for data... (Press Ctrl+C to exit)\n");

    // 4. Receive data continuously
    
    while(1) {
        char rx_buffer[RX_BUFFER_SIZE];
        memset(rx_buffer, 0, RX_BUFFER_SIZE); // Clear the buffer
    
        // Read up to RX_BUFFER_SIZE - 1 bytes
        int n_read = read(fd, rx_buffer, RX_BUFFER_SIZE - 1); 
        
        if (n_read < 0) {
            // Check for specific error (EAGAIN means timeout/no data)
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                continue;
            } else {
                perror("Error reading from serial port");
                break;
            }
        } else if (n_read > 0) {
            // Null-terminate the string
            rx_buffer[n_read] = '\0';
            
            prinnt("%s",rx_buffer);
            if(rx_buffer[n_read-1]=='\n'){
                break;
            }
            
        }
    }

    // 5. Cleanup
    close(fd);
    printf("Serial port closed.\n");

    bcm2835_close();
    printf("bcm2835 library closed. Program finished.\n");
    
    return 0;
}
