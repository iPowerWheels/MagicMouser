/* main.c
 *
 * Copyright 2023 Daniel Mendoza
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <GLFW/glfw3.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <pthread.h>
#include <time.h>

double mouserx, mousery;
int mouserstate;
int width, height, widths, heights;

void position(){
  // Print cursor position in terminal
  printf("\rCursor Position at (%0.2f, %0.2f)", mouserx, mousery);
}

void cat(){
  // Play convolved sound
  system("pw-play output.wav");
}

void window_size_callback(GLFWwindow *window, int width, int height){
  // Stay alert for window size
  widths = width;
  heights = height;
}

int pixelstoHdeg(){
  int hdegree, i, j;
  
  // Takes your window in 9 pieces because HRTF impulses were taken in that portion
  int horizontal_steps = widths/37;
  int horizontal_sizes[37][2];
  int flag;
  
  // Window pixels in matrix array
  flag = 0;
  for (i = 0; i < 37; i++)
    {
      for (j = 0; j < 2; j+=2)
        {
          horizontal_sizes [i] [j] = flag;
          flag += horizontal_steps;
          horizontal_sizes [i] [j+1] = flag;
        }
      flag += 1;
    }
                    
  // Assign matrix index to angles
  for (i = 0; i < 37; i++){
    if (i < 18){
      // Beggining at 270 deg
      if (horizontal_sizes[i][0] <= mouserx && mouserx <= horizontal_sizes[i][1]){
        hdegree = 270 + 5*i;
      }
    }
    else if (i > 17){
      if (horizontal_sizes[i][0] <= mouserx && mouserx <= horizontal_sizes[i][1]){
        // 5 by 5 starting with 0 but your most right should be 90 deg
        hdegree = 5 * (i - 18);
      }
    }
    else {
      hdegree = 0;
    }
  }
 
  return hdegree;
}

int pixelstoVdeg(){
  int vdegree, i, j;
  
  // Takes your window in 9 pieces because HRTF impulses are assigned in that portion
  int vertical_steps = heights/9;
  int vertical_sizes[9][2];
  int flag;
  
  // Fills your matrix for recognizing angles
  flag = 0;
  for (i = 0; i < 9; i++)
    {
      for (j = 0; j < 2; j+=2)
        {
          vertical_sizes [i] [j] = flag;
          flag += vertical_steps;
          vertical_sizes [i] [j+1] = flag;
        }
      flag += 1;
    }
  
  // It takes your matrix for making angles
  if (vertical_sizes[0][0] <= mousery && mousery <= vertical_sizes[0][1]){
    vdegree = 40;
  }
  else if  (vertical_sizes[1][0] <= mousery && mousery <= vertical_sizes[1][1]){
    vdegree = 30;
  }
  else if  (vertical_sizes[2][0] <= mousery && mousery <= vertical_sizes[2][1]){
    vdegree = 20;
  }
  else if  (vertical_sizes[3][0] <= mousery && mousery <= vertical_sizes[3][1]){
    vdegree = 10;
  }
  else if  (vertical_sizes[4][0] <= mousery && mousery <= vertical_sizes[4][1]){
    vdegree = 0;
  }
  else if  (vertical_sizes[5][0] <= mousery && mousery <= vertical_sizes[5][1]){
    vdegree = -10;
  }
  else if  (vertical_sizes[6][0] <= mousery && mousery <= vertical_sizes[6][1]){
    vdegree = -20;
  }
  else if  (vertical_sizes[7][0] <= mousery && mousery <= vertical_sizes[7][1]){
    vdegree = -30;
  }
  else if  (vertical_sizes[8][0] <= mousery && mousery <= vertical_sizes[8][1]){
    vdegree = -40;
  }
  else {
    vdegree = 0;
  }

  return vdegree;
}

// Usage vertical-horizontal angles 
void HRTF (int prefix, int suffix);

// Here is mouser function
void *mouser(void *vargp){
  GLFWwindow* window;
  GLFWvidmode* mode;
  char title[]="MagicMouser";
  
  /* Initialize the library */
  if (!glfwInit())
      return NULL;

  // Create a windowed mode window and its OpenGL context
  // This is for window size
  mode = (GLFWvidmode*)glfwGetVideoMode(glfwGetPrimaryMonitor());
  
  // Needed for initialize
  widths = mode -> width;
  heights = mode -> height;
  
  // Creates window
  window = glfwCreateWindow(mode->width, mode->height, title, NULL, NULL);
  
  if (!window)
  {
    glfwDestroyWindow(window);
    glfwTerminate();
    return NULL;
  }

  // Make the window's context current
  glfwMakeContextCurrent(window);

  // Position rendering window, needed maximize
  glfwSetWindowPos(window, 0, 0);

  // Stay alert when user changes size
  glfwSetWindowSizeCallback (window, window_size_callback);
  
  // Make a single click
  int last_state = GLFW_RELEASE;

  // Loop until the user closes the window
  while (!glfwWindowShouldClose(window))
  {
    // Swap front and back buffers
    glfwSwapBuffers(window);

    // Poll for and process events
    glfwPollEvents();
    
    int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
    double xpos, ypos;
    
    // Pointer position
    glfwGetCursorPos(window, &xpos, &ypos);
    mouserx = xpos;
    mousery = ypos;
    
    // Stay alert when user changes size
    //glfwSetWindowSizeCallback (window, window_size_callback);
    
    // Here's for printing in terminal
    position ();
      
    // On click it sounds
    if (state == GLFW_PRESS && last_state == GLFW_RELEASE)
    {
      int horizontal = pixelstoHdeg ();
      int vertical = pixelstoVdeg ();
      
      // For HRTF
      //printf ("\nYour cursor is: %d° horizontal %d° vertical\n", horizontal, vertical);
      HRTF (vertical, horizontal);
      
      // For playing sound
      cat ();
    }  
    
    // Save last click
    last_state = state;
  }
    
  // Window closed
  printf("\nGoodbye! \n");

  glfwDestroyWindow(window);
  glfwTerminate();
  return NULL;
}

int
main (void)
{
  pthread_t thread_id;
  
  pthread_create(&thread_id, NULL, mouser , NULL);
  pthread_join(thread_id, NULL);

  exit (0);
  return EXIT_SUCCESS;
}
