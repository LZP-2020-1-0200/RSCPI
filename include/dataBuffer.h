

#include <utility>
#include <cstdint>
#include <type_traits>
#include <memory>



#ifndef DATA_BUFFER_H
#define DATA_BUFFER_H

#ifndef DATA_BUFFER_SIZE
#define DATA_BUFFER_SIZE 40
#endif
class DataBuffer {
	// Error flags. These are set when an error occurs.
	// These should help detect potential memory leaks.
	// The errFlags are static, so they can be checked
	// after the buffer has been deleted.
	// This does mean that all buffers share the same
	// errFlags, but this is more of a diagnostic tool,
	// helping to detect memory leaks.
public:
	enum Error {
		NO_ERR = 0,
		DOUBLE_SET_ERR = 1,
		DOUBLE_CLEAR_ERR = 2,
		DESTRUCTOR_WITHOUT_CLEAR_ERR = 4,
		UNSET_GET_ERR = 8,
		ALLIGNMENT_ERR = 16

	};
private:
	static uint8_t errFlags;
	void (*tearDown)(DataBuffer& data);
	bool isSet = false;
	alignas(64) uint8_t data[DATA_BUFFER_SIZE];
	void* aligned;
public:
	static uint8_t getErrFlags() {
		return errFlags;
	}
	static void clearErrFlags() {
		errFlags = 0;
	}
	~DataBuffer() {
		if(isSet) {
			if(tearDown != nullptr) {
				tearDown(*this);
			}
			errFlags |= DESTRUCTOR_WITHOUT_CLEAR_ERR;
		}
	}
	bool isDataSet() const {
		return isSet;
	}
	template <typename T>
	T& get() {
		static_assert(sizeof(T) <= DATA_BUFFER_SIZE, "DataBuffer size is too small");
		if(!isSet) {
			errFlags |= UNSET_GET_ERR;
		}
		return *reinterpret_cast<T*>(aligned);
	}
	template <typename T>
	void set(const typename std::remove_reference<T>::type& value) {
		typedef typename std::remove_reference<T>::type TRaw;
		//Serial.println("set with copied data");
		if(isSet) {
			clear();
			errFlags |= DOUBLE_SET_ERR;
		}
		
		tearDown = [](DataBuffer& data) {data.unset<T>();};
		
		{
			void* tmp_ptr = data;
			unsigned int remaining_size = DATA_BUFFER_SIZE;
			aligned = std::align(alignof(TRaw), sizeof(TRaw), tmp_ptr, remaining_size);
			if(aligned == nullptr) {
				errFlags |= ALLIGNMENT_ERR;
				return;
			}
			new (aligned) TRaw(value);
		}
		isSet = true;
	}

	template <typename T>
	void set(typename std::remove_reference<T>::type&& value) {
		//Serial.println("set with moved data");
		if(isSet) {
			clear();
			errFlags |= DOUBLE_SET_ERR;
		}
		tearDown = [](DataBuffer& data) {data.unset<T>();};
		//Serial.println("set with moved data after tearDown");
		typedef typename std::remove_reference<T>::type TRaw;
		static_assert(sizeof(T) <= DATA_BUFFER_SIZE, "DataBuffer size is too small");
		
		//Serial.println("set with moved data before init");
		{
			void* tmp_ptr = data;
			size_t remaining_size = DATA_BUFFER_SIZE;
			aligned = std::align(alignof(TRaw), sizeof(TRaw), tmp_ptr, remaining_size);
			if(aligned == nullptr) {
				errFlags |= ALLIGNMENT_ERR;
				return;
			}
			new (aligned) TRaw(std::move(value));
		}
		
		//Serial.println("set with moved data after init");
		isSet = true;
		//Serial.println("set with moved data end");
	}
	void clear() {
		if(!isSet) {
			errFlags |= DOUBLE_CLEAR_ERR;
		}
		if(tearDown != nullptr) {
			tearDown(*this);
		}
		isSet = false;
	}
private:
	template <typename T>
	void unset() {
		if(!isSet) {
			errFlags |= DOUBLE_CLEAR_ERR;
		}
		static_assert(sizeof(T) <= DATA_BUFFER_SIZE, "DataBuffer size is too small");
		reinterpret_cast<T*>(aligned)->~T();
		isSet = false;
	}
};

inline uint8_t DataBuffer::errFlags = DataBuffer::NO_ERR;

#endif