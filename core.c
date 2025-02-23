#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <sndfile.h>

void convolve(float *signal, int signal_length, float *impulse, int impulse_length, float *output, int channels) {
  for (int ch = 0; ch < channels; ch++){
    // Channel by channel
    for (int n = 0; n < signal_length + impulse_length - 1; n++) {
      // Initialize output channel
      output[n * channels + ch] = 0.0f;
      for (int k = 0; k < impulse_length; k++) {
        if (n - k >= 0 && n - k < signal_length) {
          output[n * channels + ch] += signal[(n - k) * channels + ch] * impulse[k * channels + ch];
        }
      }
    }
  }
}

void HRTF (int prefix, int suffix){
  // Read Vars
  SNDFILE * signal_file, *impulse_file;
  SF_INFO signal_info, impulse_info;
  
  // Write Vars
  SNDFILE *outfile;
  SF_INFO sfinfo;
  
  float * signal_buffer, * impulse_buffer, * output_buffer;
  
  // Constant output name
  char signal_file_path[15] = "noise.wav";
  
  //printf("Trying to open audio File: %s\n",signal_file_path);

  // Concatenate name  
  char impulse_file_path[50];
  snprintf(impulse_file_path, sizeof(impulse_file_path), "impulses/%de%03da.wav", prefix, suffix);

  //printf("%s\n",impulse_file_path);

  // Open read files
  signal_file = sf_open (signal_file_path, SFM_READ, &signal_info);
  impulse_file = sf_open (impulse_file_path, SFM_READ, &impulse_info);
  
  signal_buffer = (float *) malloc (signal_info.frames * signal_info.channels * sizeof(float));
  impulse_buffer = (float *) malloc (impulse_info.frames * impulse_info.channels * sizeof(float)); //used to be signal_info
               
  // Read files
  int signal_frames, impulse_frames, output_frames;
  signal_frames = sf_read_float (signal_file, signal_buffer, signal_info.frames*signal_info.channels);
  impulse_frames = sf_read_float (impulse_file, impulse_buffer, impulse_info.frames*impulse_info.channels);

  // Define channels
  int signal_channels, impulse_channels;
  signal_channels = signal_info.channels;
  impulse_channels = impulse_info.channels;
  
  // Size of output
  output_frames = signal_frames + impulse_frames - 1;
  output_buffer = (float *) malloc (output_frames * signal_channels * sizeof(float));
  
  if (impulse_channels != signal_channels) {
    printf ("Missmatch channels\n");
    sf_close (signal_file);
    sf_close (impulse_file);
    free (signal_buffer);
    free (impulse_buffer);
    free (output_buffer);
    exit (1);
  }

  // Convolve function
  convolve (signal_buffer, signal_frames/signal_channels, impulse_buffer, impulse_frames/impulse_channels, output_buffer, signal_channels);
  
  // Set audio file info
  sfinfo.format = SF_FORMAT_WAV | SF_FORMAT_PCM_24;
  sfinfo.channels = signal_info.channels;
  sfinfo.samplerate = signal_info.samplerate;

  // Open write file
  outfile = sf_open("output.wav", SFM_WRITE, &sfinfo);
  if (!outfile) {
    printf("Error opening write file: %s\n", sf_strerror(NULL));
    free (signal_buffer);
    free (impulse_buffer);
    free (output_buffer);
    exit (1);
  }

  // Write buffer data in audio file
  sf_count_t frames_written = sf_writef_float(outfile, output_buffer, signal_frames);
  if (frames_written < 0) {
    printf("Error writting output file: %s\n", sf_strerror(outfile));
  }

  // Close files
  sf_close (signal_file);
  sf_close (impulse_file);
  sf_close(outfile);
  
  // Free space
  free (signal_buffer);
  free (impulse_buffer);
  free (output_buffer);

}

