#pragma once

template <typename T, int size = 256> struct CircularBuffer {
	int index;
	T data[size];

	void Push(T item) {
		data[index] = item;
		index++;
		if (index >= size)
			index = 0;
	}

	struct it {
		T *base;
		int index;

		it operator++() {
			index++;
			if (index >= size)
				index = 0;
			return *this;
		}

		T &operator*() { return base[index]; }

		bool operator!=(const it &b) {
			return index != b.index;
		}
	};

	it begin() { return it{data, index}; }

	it end() {
		auto prev = index - 1;
		if (prev < 0)
			prev = size - 1;
		return it{data, prev};
	}
};
