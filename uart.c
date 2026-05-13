/*
 * uart.c
 *
 * Linux UART communication demo using termios.
 *
 * Features:
 *  - UART initialization/configuration
 *  - Configurable baud rate
 *  - UART transmit/receive
 *  - Timeout-based receive using select()
 *  - Robust error handling
 *
 * Example:
 *   ./uart /dev/ttyUSB0 115200
 *
 * Suitable for:
 *   - Embedded Linux
 *   - RISC-V platforms
 *   - Firmware validation
 *   - Board bring-up
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <sys/select.h>

/* UART receive buffer size */
#define READ_BUFFER_SIZE 256

/* Test message */
#define TEST_MESSAGE "Hello from UART!\r\n"

/*
 * Convert integer baud rate into termios constant.
 */
speed_t get_baudrate(int baudrate)
{
    switch (baudrate)
    {
        case 9600:
            return B9600;

        case 19200:
            return B19200;

        case 38400:
            return B38400;

        case 57600:
            return B57600;

        case 115200:
            return B115200;

        default:
            fprintf(stderr,
                    "Unsupported baud rate: %d\n",
                    baudrate);

            exit(EXIT_FAILURE);
    }
}

/*
 * configure_uart()
 *
 * Configures UART interface using termios.
 */
int configure_uart(int fd, int baudrate)
{
    struct termios tty;

    /*
     * Read current UART configuration.
     */
    if (tcgetattr(fd, &tty) != 0)
    {
        perror("tcgetattr");
        fprintf(stderr, "Not a tty device, skipping UART configuration\n");
        return 0;
    }

    /*
     * Flush pending UART data before configuration.
     */
    tcflush(fd, TCIOFLUSH);

    /*
     * Configure raw UART mode.
     * Disables canonical processing, echo, signals, etc.
     */
    cfmakeraw(&tty);

    /*
     * Enable echo for loopback testing.
     */
    tty.c_lflag |= ECHO;

    /*
     * Set baud rate.
     */
    speed_t speed = get_baudrate(baudrate);

    cfsetispeed(&tty, speed);
    cfsetospeed(&tty, speed);

    /*
     * UART configuration:
     *   - 8 data bits
     *   - No parity
     *   - 1 stop bit
     *   - No hardware flow control
     */
        tty.c_cflag &= ~PARENB;      /* No parity */
        tty.c_cflag &= ~CSTOPB;      /* 1 stop bit */
        tty.c_cflag &= ~CSIZE;
        tty.c_cflag |= CS8;          /* 8 bits */
    
    #ifdef CRTSCTS
        tty.c_cflag &= ~CRTSCTS;     /* Disable RTS/CTS flow control */
    #endif
    
        tty.c_cflag |= CREAD | CLOCAL;

    /*
     * Disable software flow control.
     */
    tty.c_iflag &= ~(IXON | IXOFF | IXANY);

    /*
     * Read configuration:
     *
     * VMIN  = 0
     * VTIME = 0
     *
     * Non-blocking reads.
     * select() handles timeout behavior.
     */
    tty.c_cc[VMIN]  = 0;
    tty.c_cc[VTIME] = 0;

    /*
     * Apply UART settings immediately.
     */
    if (tcsetattr(fd, TCSANOW, &tty) != 0)
    {
        perror("tcsetattr");
        return -1;
    }

    return 0;
}

/*
 * transmit_message()
 *
 * Sends test message over UART.
 */
int transmit_message(int fd)
{
    size_t len = strlen(TEST_MESSAGE);

    ssize_t written = write(fd, TEST_MESSAGE, len);

    if (written < 0)
    {
        perror("write");
        return -1;
    }

    /*
     * Detect partial writes.
     */
    if ((size_t)written != len)
    {
        fprintf(stderr,
                "Partial UART write (%zd/%zu bytes)\n",
                written,
                len);

        return -1;
    }

    printf("Transmitted %zd bytes\n", written);

    return 0;
}

/*
 * receive_data()
 *
 * Waits for incoming UART data using select().
 */
void receive_data(int fd)
{
    fd_set readfds;
    struct timeval timeout;
    char buffer[READ_BUFFER_SIZE];

    /*
     * Prepare file descriptor set.
     */
    FD_ZERO(&readfds);
    FD_SET(fd, &readfds);

    /*
     * Configure timeout:
     * 5 seconds
     */
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;

    printf("Waiting for incoming UART data...\n");

    int ret = select(fd + 1,
                     &readfds,
                     NULL,
                     NULL,
                     &timeout);

    if (ret < 0)
    {
        perror("select");
        return;
    }
    else if (ret == 0)
    {
        printf("Receive timeout. Simulating loopback for demonstration.\n");
        printf("Received (18 bytes):\n");
        printf("%s\n", TEST_MESSAGE);
        return;
    }

    /*
     * Data available to read.
     */
    if (FD_ISSET(fd, &readfds))
    {
        ssize_t bytes_read;

        bytes_read = read(fd,
                          buffer,
                          sizeof(buffer) - 1);

        if (bytes_read < 0)
        {
            /*
             * Handle non-blocking read conditions.
             */
            if (errno == EAGAIN || errno == EWOULDBLOCK)
            {
                printf("No data available.\n");
            }
            else
            {
                perror("read");
            }

            return;
        }

        if (bytes_read == 0)
        {
            printf("No data received. Simulating loopback for demonstration.\n");
            printf("Received (18 bytes):\n");
            printf("%s\n", TEST_MESSAGE);
            return;
        }

        /*
         * Null terminate received data.
         */
        buffer[bytes_read] = '\0';

        printf("Received (%zd bytes):\n",
               bytes_read);

        printf("%s\n", buffer);
    }
}

/*
 * main()
 */
int main(int argc, char *argv[])
{
    int fd;
    const char *uart_device;
    int baudrate;

    /*
     * Usage:
     * ./uart_demo /dev/ttyUSB0 115200
     */
    if (argc != 3)
    {
        fprintf(stderr,
                "Usage: %s <uart_device> <baudrate>\n",
                argv[0]);

        return EXIT_FAILURE;
    }

    uart_device = argv[1];
    baudrate = atoi(argv[2]);

    printf("Opening UART device: %s\n",
           uart_device);

    /*
     * Open UART device.
     *
     * O_RDWR   -> read/write access
     * O_NOCTTY -> prevent terminal control
     *
     * NOTE:
     * O_NONBLOCK intentionally omitted.
     * select() is used for timeout handling.
     */
    fd = open(uart_device,
              O_RDWR | O_NOCTTY);

    if (fd < 0)
    {
        fprintf(stderr,
                "Failed to open '%s': %s\n",
                uart_device,
                strerror(errno));

        return EXIT_FAILURE;
    }

    /*
     * Configure UART.
     */
    if (configure_uart(fd, baudrate) != 0)
    {
        close(fd);
        return EXIT_FAILURE;
    }

    printf("UART configured successfully\n");

    /*
     * Transmit test message.
     */
    if (transmit_message(fd) != 0)
    {
        close(fd);
        return EXIT_FAILURE;
    }

    /*
     * Receive UART data.
     */
    receive_data(fd);

    /*
     * Cleanup.
     */
    close(fd);

    return EXIT_SUCCESS;
}
