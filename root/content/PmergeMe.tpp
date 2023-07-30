#pragma once

template <class T>
double bench_container(T &c)
{
	std::clock_t start = std::clock();
	PmergeMe::process(c, PmergeMe::kDefaultThreshold);
	std::clock_t end = std::clock();
	double elapsed = 1000000.0 * (end - start) / CLOCKS_PER_SEC;
	return elapsed;
}
template <class T>
void print_container(const std::string &mesg, const T &c)
{
	typedef typename T::const_iterator const_iterator;

	std::cout << mesg;
	for (const_iterator it = c.begin(); it != c.end(); ++it)
		std::cout << *it << " ";
	std::cout << std::endl;
}


template <class T> void PmergeMe::process(T &c, int threshold){
    mergeInsertSort(c, 0, c.size() - 1, threshold);
}

template <class T> void PmergeMe::mergeInsertSort(T& c, int start, int end, int threshold){
    if (start < end) {
        if (end - start < threshold) {
            insertSort(c, start, end);
        } else {
            int mid = start + (end - start) / 2;
            mergeInsertSort(c, start, mid, threshold);
            mergeInsertSort(c, mid + 1, end, threshold);
            merge(c, start, mid, end);
        }
    }
}

template <class T> void PmergeMe::insertSort(T &c, int start, int end){
    for (int i = start + 1; i <= end; ++i) {
        int key = c[i];
        int j = i - 1;
        for (; j >= start && c[j] > key; --j)
            c[j + 1] = c[j];
        c[j + 1] = key;
    }
}

template <class T> void PmergeMe::merge(T &c, int start, int mid, int end){
    int i, j, k;
    int n1 = mid - start + 1;
    int n2 = end - mid;

    T left(n1), right(n2);

    for (i = 0; i < n1; ++i)
        left[i] = c[start + i];
    for (j = 0; j < n2; ++j)
        right[j] = c[mid + 1 + j];

    i = 0;
    j = 0;
    k = start;
    while (i < n1 && j < n2) {
        if (left[i] <= right[j])
            c[k++] = left[i++];
        else
            c[k++] = right[j++];
    }

    while (i < n1)
        c[k++] = left[i++];
    while (j < n2)
        c[k++] = right[j++];
}
