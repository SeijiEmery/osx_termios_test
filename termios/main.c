//
//  main.c
//  termios
//
//  Created by semery on 12/2/16.
//  Copyright © 2016 Seiji Emery. All rights reserved.
//

#include <sys/ioctl.h>
#include <sys/termios.h>

#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>

int main(int argc, const char * argv[]) {
    
    // Print termios struct + offsets
    printf("sizeof(termios) = 0x%lx\n", sizeof(struct termios));
    printf("field offsets:\n");
    struct termios t;
    printf("c_iflag:  0x%lx\n", (size_t)&t.c_iflag - (size_t)&t);
    printf("c_oflag:  0x%lx\n", (size_t)&t.c_oflag - (size_t)&t);
    printf("c_cflag:  0x%lx\n", (size_t)&t.c_cflag - (size_t)&t);
    printf("c_lflag:  0x%lx\n", (size_t)&t.c_lflag - (size_t)&t);
    printf("c_cc:     0x%lx\n", (size_t)&t.c_cc - (size_t)&t);
    printf("c_ispeed: 0x%lx\n", (size_t)&t.c_ispeed - (size_t)&t);
    printf("c_ospeed: 0x%lx\n", (size_t)&t.c_ospeed - (size_t)&t);
    
    // Print BSD TIOCGETA / TIOCSETA constants.
    printf("TIOCGETA = 0x%lx\n", TIOCGETA);
    printf("TIOCSETA = 0x%lx\n", TIOCSETA);
    
    // Print ICANNON and ECHO flags
    printf("ICANON   = 0x%x\n", ICANON);
    printf("ECHO     = 0x%x\n", ECHO);
    
    
    // Using https://opensource.apple.com/source/Libc/Libc-167/gen.subproj/termios.c
    // as a reference, check that these work as expected (should be able to implement
    // tcsetattr / tcgetattr using ioctl + TIOCGETA / TIOCSETA, respectively).
    struct termios t1, t2, t3;
    
    // Get termios info for STDIN (file handle 0), and assert that
    // the tcgetattr() + ioctl(TIOCGETA) calls are identical.
    assert(tcgetattr(0, &t1) == 0);
    assert(ioctl(0, TIOCGETA, &t2) == 0);
    assert(memcmp(&t1, &t2, sizeof(t1)) == 0);
    
    // Switch the ICANON and ECHO flags on t1, and set using tcsetattr().
    t1.c_lflag ^= (ICANON | ECHO);
    assert(tcsetattr(0, TCSANOW, &t1) == 0);
    
    // Fetch into t3 using ioctl(TIOCGETA), and assert that t3 == t1, and t3 != t2.
    assert(ioctl(0, TIOCGETA, &t3) == 0);
    assert(memcmp(&t3, &t1, sizeof(t1)) == 0);
    assert(memcmp(&t3, &t2, sizeof(t1)) != 0);
    
    // Set back using the original values in t2 using ioctl(TIOCSETA)
    assert(ioctl(0, TIOCSETA, &t2) == 0);
    
    // Fetch into t3 using tcgetattr, and assert t3 == t2, t3 != t1.
    assert(tcgetattr(0, &t3) == 0);
    assert(memcmp(&t3, &t1, sizeof(t1)) != 0);
    assert(memcmp(&t3, &t2, sizeof(t1)) != 0);
    
    printf("tests ok.\n");
    return 0;
}
