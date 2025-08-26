#include <math.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

void cleanup(int sig) {
  printf("\x1b[?25h"); // show cursor
  fflush(stdout);
  _exit(0);
}

int main(int argc, char **argv) {
  // screen properties
  struct winsize w;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &w); // get window size

  int screen_width = w.ws_col;
  int screen_height = w.ws_row;

  // wave properties
  float amplitude = screen_height / 4.0f; // scale with window size
  float frequency = 0.1f;
  float speed = 0.2f;
  float offset = 0.0f;

  // buffer
  char buffer[screen_height * screen_width];

  // default char
  char pixel = '*';

  if (argc >= 2) {
    if (argc >= 2) {
      pixel = argv[1][0]; // set custom char
    }
  }

  printf("\x1b[2J");   // clear screen
  printf("\x1b[?25l"); // hide cursor

  signal(SIGINT, cleanup); // ctrl-c will trigger cleanup

  while (1) {
    memset(buffer, ' ', screen_width * screen_height);

    // iterate through columns
    for (int x = 0; x < screen_width; ++x) {
      float y = amplitude * sinf(frequency * x + offset);
      int screen_y =
          (int)(screen_height / 2.0f + y); // shift pivot to center of screen

      if (screen_y >= 0 && screen_y < screen_height) {
        int idx = x + screen_y * screen_width;
        buffer[idx] = pixel;
      }
    }

    printf("\x1b[H"); // move cursor to top left

    for (int i = 0; i < screen_width * screen_height; i++) {
      // wrap to next line if on edge, else print out char
      putchar(i % screen_width == 0 ? '\n' : buffer[i]);
    }

    offset += speed;
    usleep(50000);
  }

  return 0;
}
