import matplotlib.pyplot as plt
from tqdm import tqdm
import random
import time


class HeapSort:
    @staticmethod
    def heapify(arr, n, i):
        j = 2 * i if i != 0 else 1
        x = arr[i]
        while j < n:
            if j + 1 < n and arr[j + 1] > arr[j]:
                j += 1
            if x >= arr[j]:
                break
            arr[j // 2] = arr[j]
            j *= 2
        arr[j // 2] = x

    @staticmethod
    def sort(arr):
        n = len(arr)
        # Build Heap
        for i in range(n // 2 - 1, -1, -1):
            HeapSort.heapify(arr, n, i)
        # Sort
        for i in range(n - 1, 0, -1):
            arr[i], arr[0] = arr[0], arr[i]
            HeapSort.heapify(arr, i, 0)


class MergeSort:
    @staticmethod
    def sort(arr):
        if len(arr) > 1:
            # Divide
            mid = len(arr) // 2
            left = arr[:mid]
            right = arr[mid:]
            # Sort
            MergeSort.sort(left)
            MergeSort.sort(right)
            # Merge
            i = j = k = 0
            while i < len(left) and j < len(right):
                if left[i] < right[j]:
                    arr[k] = left[i]
                    i += 1
                else:
                    arr[k] = right[j]
                    j += 1
                k += 1
            while i < len(left):
                arr[k] = left[i]
                i += 1
                k += 1
            while j < len(right):
                arr[k] = right[j]
                j += 1
                k += 1


class QuickSort:
    @staticmethod
    def partition(arr, low, high):
        pivot = arr[high]
        i = low - 1

        for j in range(low, high):
            if arr[j] < pivot:
                i += 1
                arr[i], arr[j] = arr[j], arr[i]

        arr[i + 1], arr[high] = arr[high], arr[i + 1]
        return i + 1

    @staticmethod
    def quick_sort(arr, low, high):
        if low < high:
            pi = QuickSort.partition(arr, low, high)
            QuickSort.quick_sort(arr, low, pi - 1)
            QuickSort.quick_sort(arr, pi + 1, high)

    @staticmethod
    def sort(arr):
        QuickSort.quick_sort(arr, 0, len(arr) - 1)


class ShellSort:
    @staticmethod
    def sort(arr):
        n = len(arr)
        gap = n // 2

        while gap > 0:
            for i in range(gap, n):
                temp = arr[i]
                j = i
                while j >= gap and arr[j - gap] > temp:
                    arr[j] = arr[j - gap]
                    j -= gap
                arr[j] = temp
            gap //= 2


def measure_sorting_time(sorting_func, arr):
    start_time = time.time()
    sorting_func(arr)
    end_time = time.time()
    return end_time - start_time


if __name__ == '__main__':
    random.seed(time.time())
    results = [[], [], [], [], []]
    data_sizes = [0, 10_000, 50_000, 100_000, 200_000, 300_000, 400_000, 500_000, 600_000, 700_000, 800_000, 900_000, 1_000_000]
    # Measure different sorting algorithm through different data sizes
    for size in tqdm(data_sizes):
        data = random.sample(range(1, size * 10), size)
        results[0].append(measure_sorting_time(HeapSort.sort, data.copy()))
        results[1].append(measure_sorting_time(MergeSort.sort, data.copy()))
        results[2].append(measure_sorting_time(QuickSort.sort, data.copy()))
        results[3].append(measure_sorting_time(ShellSort.sort, data))
        results[4].append(size // 2)
    # Show results on Plot
    fig, ax1 = plt.subplots(figsize=(10, 6))
    ax1.set_xlabel('Data Size')
    ax1.set_ylabel('Time (s)')
    ax1.plot(data_sizes, results[0], label='Heap Sort', marker='o', color='r')
    ax1.plot(data_sizes, results[1], label='Merge Sort', marker='o', color='g')
    ax1.plot(data_sizes, results[2], label='Quick Sort', marker='o', color='dodgerblue')
    ax1.plot(data_sizes, results[3], label='Shell Sort', marker='o', color='darkorange')

    ax2 = ax1.twinx()
    ax2.set_ylabel('Shell Sort Gap')
    ax2.plot(data_sizes, results[4], label='Shell Sort Gap', marker='x', color='orange')
    ax2.tick_params(axis='y')

    fig.legend(loc='upper left')
    plt.title('Sorting Algorithm Performance')
    plt.grid(True)
    plt.show()
