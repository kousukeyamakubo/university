#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#define NUM_ITEMS 10
#define NUM_PROCS 5

void mergeSort(int numbers[], int temp[], int left, int right, int p);
void m_sort(int numbers[], int temp[], int left, int right);
void merge(int numbers[], int temp[], int left, int mid, int right);

int numbers[NUM_ITEMS];
int temp[NUM_ITEMS];


int main(int argc,char *argv[])
{
  if (argc != 2) {
    printf("Usage: %s <number>\n", argv[0]);
    return 1;
  }
  int number = atoi(argv[1]);
  int i,left,right,result;
  left = 0;
  right = NUM_ITEMS-1;
  result = number;
  //seed random number generator
  srand(getpid());

  //fill array with random integers
  for (i = 0; i < NUM_ITEMS; i++)
    numbers[i] = rand();

  //perform merge sort on array
  mergeSort(numbers, temp, left, right, number);

  return 0;
}

void mergeSort(int numbers[], int temp[],int left, int right, int p) {
    int left_pid, right_pid;
    int mid = (left + right) / 2;
    int size_left = mid - left + 1;
    int size_right = right - mid;
    int fd_left[2], fd_right[2];

    if (right <= left) {
        return;
        
    }

    if (p <= 1) {
        m_sort(numbers, temp, left, right);
    } else {
        if (pipe(fd_left) == -1 || pipe(fd_right) == -1) {
          perror("pipe failed.");
          exit(1);
        }
        int p_left = p / 2;
        int p_right = p - p_left;

        if ((left_pid = fork()) == -1) {
            perror("fork failed.");
            exit(1);
        }

        if (left_pid == 0) {
            close(fd_left[0]);
            mergeSort(numbers, temp, left, mid, p_left);
            if (write(fd_left[1], &numbers[left], size_left * sizeof(int)) == -1) {
                perror("pipe write.");
                exit(1);
            }
            close(fd_left[1]);
            exit(0);
        }

        if ((right_pid = fork()) == -1) {
            perror("fork failed.");
            exit(1);
        }

        if (right_pid == 0) {
            close(fd_right[0]);
            mergeSort(numbers, temp, mid + 1, right, p_right);
            if (write(fd_right[1], &numbers[mid + 1], size_right * sizeof(int)) == -1) {
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
