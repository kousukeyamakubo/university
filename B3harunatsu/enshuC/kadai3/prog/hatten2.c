/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * This program demonstrates the use of the merge sort algorithm.  For
 * more information about this and other sorting algorithms, see
 * http://linux.wku.edu/~lamonml/kb.html
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#define NUM_ITEMS 10
#define DEPTH 2

void mergeSort(int numbers[], int temp[], int array_size, int left, int right, int depth);
void m_sort(int numbers[], int temp[], int left, int right);
void merge(int numbers[], int temp[], int left, int mid, int right);

int numbers[NUM_ITEMS];
int temp[NUM_ITEMS];


int main()
{
  int i,left,right;
  left = 0;
  right = NUM_ITEMS-1;

  //seed random number generator
  srand(getpid());

  //fill array with random integers
  for (i = 0; i < NUM_ITEMS; i++)
    numbers[i] = rand();

  //perform merge sort on array
  mergeSort(numbers, temp, NUM_ITEMS, left, right, DEPTH);

  printf("Done with sort.\n");

  for (i = 0; i < NUM_ITEMS; i++)
    printf("%i\n", numbers[i]);

  return 0;
}

void mergeSort(int numbers[], int temp[], int array_size, int left, int right, int depth){
    int left_pid,right_pid;
    int mid = (left+right)/2;
    int size_left = mid - left + 1;
    int size_right = right - mid;
    int fd_left[2],fd_right[2];
    if(right <= left){
      return;
    }
    if (pipe(fd_left) ==-1 || pipe(fd_right)==-1) {
        perror("pipe failed.");
        exit(1);
    }
    if(depth == 0){
      m_sort(numbers,temp,left,right);
    }else{
      if ((left_pid=fork())==-1) {
        perror("fork failed.");
        exit(1);
      }
      if(left_pid == 0){
        close(fd_left[0]);
        mergeSort(numbers,temp,size_left,left,mid,depth-1);
        if (write(fd_left[1], &numbers[left], size_left * sizeof(int)) == -1) {
          perror("pipe write.");
          exit(1);
        }
        close(fd_left[1]);
        exit(0);
      }
      if ((right_pid=fork())==-1) {
        perror("fork failed.");
        exit(1);
      }
      if(right_pid == 0){
        close(fd_right[0]);
        mergeSort(numbers,temp,size_right,mid+1,right,depth-1);
        if (write(fd_right[1], &numbers[mid+1], size_right * sizeof(int)) == -1) {
          perror("pipe write.");
          exit(1);
        }
        close(fd_right[1]);
        exit(0);
      }
      close(fd_left[1]);
      close(fd_right[1]);
      waitpid(left_pid, NULL, 0);
      if (read(fd_left[0], &numbers[left], size_left * sizeof(int)) == -1) {
          perror("pipe read.");
          exit(1);
      }
      close(fd_left[0]);
      waitpid(right_pid, NULL, 0);
      if (read(fd_right[0], &numbers[mid + 1], size_right * sizeof(int)) == -1) {
          perror("pipe read.");
          exit(1);
      }
      close(fd_right[0]);
      merge(numbers, temp, left, mid + 1, right);
    }
}



void m_sort(int numbers[], int temp[], int left, int right)
{
  int mid;

  if (right > left)
  {
    mid = (right + left) / 2;
    m_sort(numbers, temp, left, mid);
    m_sort(numbers, temp, mid+1, right);

    merge(numbers, temp, left, mid+1, right);
  }
}


void merge(int numbers[], int temp[], int left, int mid, int right)
{
  int i, left_end, num_elements, tmp_pos;

  left_end = mid - 1;
  tmp_pos = left;
  num_elements = right - left + 1;

  while ((left <= left_end) && (mid <= right))
  {
    if (numbers[left] <= numbers[mid])
    {
      temp[tmp_pos] = numbers[left];
      tmp_pos = tmp_pos + 1;
      left = left +1;
    }
    else
    {
      temp[tmp_pos] = numbers[mid];
      tmp_pos = tmp_pos + 1;
      mid = mid + 1;
    }
  }

  while (left <= left_end)
  {
    temp[tmp_pos] = numbers[left];
    left = left + 1;
    tmp_pos = tmp_pos + 1;
  }
  while (mid <= right)
  {
    temp[tmp_pos] = numbers[mid];
    mid = mid + 1;
    tmp_pos = tmp_pos + 1;
  }

  for (i=0; i <= num_elements; i++)
  {
    numbers[right] = temp[right];
    right = right - 1;
  }
}
