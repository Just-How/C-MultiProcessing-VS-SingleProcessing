// HW2 Practical Assignment
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <time.h>

  // Insertion sort algorithm
void insertionSort(int arr[], int n)
{
  int i, key, j;
  for (i = 1; i < n; i++)
  {
    key = arr[i];
    j = i - 1;
    while (j >= 0 && arr[j] > key)
    {
      arr[j + 1] = arr[j];
      j = j - 1;
    }
    arr[j + 1] = key;
  }
}

int main()
{
  int n = 10;
  int arr[] = {339,360,725,567,797,90,852,954,510,824};

    // Pipe Creation
  int pipe1[2], pipe2[2];
  if (pipe(pipe1) == -1 || pipe(pipe2) == -1)
  {
    perror("Pipe creation failed");
    return -1;
  }

  // Start time for multi-threading sorting
  clock_t start_time_multi = clock();

  // Fork two child processes
  pid_t pid1, pid2;
  pid1 = fork();

  if (pid1 < 0)
  {
    perror("Fork failed");
    return -1;
  }

  if (pid1 == 0)
  {
    // First child process sorts the first half of the array
    close(pipe1[0]);
    insertionSort(arr, n / 2);
    write(pipe1[1], arr, (n / 2) * sizeof(int));
    close(pipe1[1]);
    exit(0);
  } 
  else
  {
    pid2 = fork();

    if (pid2 < 0)
    {
      perror("Fork failed");
      return -1;
    }

    if (pid2 == 0)
    {
      // Second child process sorts the second half of the array
      close(pipe2[0]);
      insertionSort(arr + n / 2, n - n / 2);
      write(pipe2[1], arr + n / 2, (n - n / 2) * sizeof(int));
      close(pipe2[1]);
      exit(0);
    }
    else
    {
      wait(NULL);
      wait(NULL);
      
      // Read sorted halves from the pipes
      int sorted_half1[n / 2], sorted_half2[n - n / 2];
      close(pipe1[1]); 
      close(pipe2[1]); 
      read(pipe1[0], sorted_half1, (n / 2) * sizeof(int));
      read(pipe2[0], sorted_half2, (n - n / 2) * sizeof(int));
      close(pipe1[0]);
      close(pipe2[0]);

      // Merge the sorted halves
      int merged_arr[n];
      int i = 0, j = 0, k = 0;
      
      while (i < n / 2 && j < n - n / 2)
      {
        if (sorted_half1[i] < sorted_half2[j])
        {
          merged_arr[k++] = sorted_half1[i++];
        } 
        else 
        {
          merged_arr[k++] = sorted_half2[j++];
        }
      }

      while (i < n / 2) 
      {
        merged_arr[k++] = sorted_half1[i++];
      }
      
      while (j < n - n / 2)
      {
        merged_arr[k++] = sorted_half2[j++];
      }
      
      // Measure end time for multi-processing sorting
      clock_t end_time_multi = clock();
      double time_taken_multi = ((double)end_time_multi - start_time_multi) / CLOCKS_PER_SEC;

      // Non-multi-threaded sorting (insertion sort)
      int arr_copy[n];
      memcpy(arr_copy, arr, n * sizeof(int));
      clock_t start_time_single = clock();
      insertionSort(arr_copy, n);
      clock_t end_time_single = clock();
      double time_taken_single = ((double)end_time_single - start_time_single) / CLOCKS_PER_SEC;

      // Write results to a text file
      FILE *file = fopen("output.txt", "a");
      if (file == NULL)
      {
        perror("Error opening file");
        return -1;
      }
/*
      fprintf(file, "Sorted Array (Multi-Processing): ");
      for (int i = 0; i < n; i++)
      {
        fprintf(file, "%d ", merged_arr[i]);
      }
      fprintf(file, "\n");
*/
      fprintf(file, "Number of Elements: %d\n", n);

      fprintf(file, "Time for  Multi-Processing Sorting: %f seconds\n", time_taken_multi);
            fprintf(file, "Time for Single-Processing Sorting: %f seconds\n", time_taken_single);

          fclose(file);
      
          return 0;
     }
   }
}