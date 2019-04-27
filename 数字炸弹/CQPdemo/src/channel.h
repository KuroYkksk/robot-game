#pragma once

#include <mutex>
#include <queue>

/*
�ܵ��࣬����һ���߳�������Ŷ�������һ���̴߳�����ȡ
*/
template <typename T>
class Channel {
public:
	Channel(int maxSize = 100) : m_maxSize(maxSize) {}

	bool put(T&& element) {
		std::lock_guard<std::mutex> lock(m_mutex);

		if (m_buffer.size() >= m_maxSize) {
			return false;
		}
		else {
			m_buffer.push(std::move(element));
			return true;
		}
	}

	bool get(T& element) {
		std::lock_guard<std::mutex> lock(m_mutex);
		if (m_buffer.size() != 0) {
			element = m_buffer.front();
			m_buffer.pop();
			return true;
		}
		else {
			return false;
		}
	}

private:
	const unsigned int m_maxSize;
	std::queue<T> m_buffer;
	std::mutex m_mutex;
};