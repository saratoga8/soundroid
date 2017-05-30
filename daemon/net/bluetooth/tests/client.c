/**
 * @file
 * The client for testing the blue tooth clien-server library
 **
 * The MIT License (MIT)
 *
 * Copyright (c) 2014 Daniel Haimov
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */


#include <stdio.h>
#include <unistd.h>

#include <sys/socket.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>

#include <string.h>
#include <errno.h>

#define ERR -1

int main(int argc, char **argv)
{
    if(argc != 3)
	{
	    printf("ERROR: invalid parameters number. Should be: client remote_addr sleep_time\n");
	    return 1;
	}
    
    struct sockaddr_rc addr = { 0 };
    
    char *dest = argv[1];
    
    int s = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
    if(s == ERR)
	{
	    printf("ERROR: Can't allocate socket: %s\n", strerror(errno));
	    return 1;
	}

    addr.rc_family = AF_BLUETOOTH;
    addr.rc_channel = (uint8_t) 11;
    str2ba( dest, &addr.rc_bdaddr );

    int status = connect(s, (struct sockaddr *)&addr, sizeof(addr));
    if(status == ERR)
	{
	    printf("ERROR: Can't connect to %s: %s\n", dest, strerror(errno));
	    close(s);
	    return 1;
	}

    int i;
    for(i = 0; i < 8; ++i)
	{
	    if(write(s, "hello!", 6) == ERR)
		{
		    printf("ERROR: Can't write to the socket: %s\n", strerror(errno));
		    break;
		}
	    sleep(atoi(argv[2]));
	    char buf[10] = { 0 };
	    const int bytes_read = read(s, buf, 10);
	    if(bytes_read > 0)
		{
		    printf("Received: %s\n", buf);
		}
	    else
		{
		    printf("ERROR: Can't read from the socket: %s\n", strerror(errno));
		    break;
		}
	}    
    
    close(s);
    
    return 0;
}
