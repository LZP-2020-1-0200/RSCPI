/**
 * @file dataBuffer.h
 * This file contains the DataBuffer class, which is used to store data in a statically allocated buffer.
 * The DataBuffer class can be used to store data of any type, as long as the type is not larger than the buffer.
 * 
 * This header file is microcontroller independent, so it can be used in a native environment.
*/

#include <utility>
#include <cstdint>
#include <type_traits>
#include <memory>



#ifndef DATA_BUFFER_H
#define DATA_BUFFER_H

#ifndef DATA_BUFFER_SIZE
#define DATA_BUFFER_SIZE 40
#endif

/**
 * @brief The DataBuffer class is used to store data in a statically allocated buffer.
 * The DataBuffer class can be used to store data of any type, as long as the type is not larger than the buffer.
 * The buffer size is defined by the DATA_BUFFER_SIZE macro.
 * When setting the data, a tearDown function will be automatically generated.
 * This function will call the destructor of the data.
 * This function will be called when the buffer is cleared, overriden or deleted.
 * 
*/
class DataBuffer {
public:

	/**
	 * @brief The Error enum is used to indicate what error has occurred.
	 * The error flags are static, so they can be checked
	 * after the buffer has been deleted.
	 * This does mean that all buffers share the same
	 * errFlags, but this is more of a diagnostic tool,
	 * helping to detect memory leaks.
	*/
	enum Error {
		NO_ERR = 0,
		DOUBLE_SET_ERR = 1,
		DOUBLE_CLEAR_ERR = 2,
		DESTRUCTOR_WITHOUT_CLEAR_ERR = 4,
		UNSET_GET_ERR = 8,
		ALLIGNMENT_ERR = 16

	};
private:
	/**
	 * @brief Designates the static error flags.
	 */
	static uint8_t errFlags;

	/// @brief The function to call when the buffer is deleted.
	void (*tearDown)(DataBuffer& data);

	/// @brief Indicates if the buffer is set.
	bool isSet = false;

	/// @brief The buffer to store the data in.
	alignas(64) uint8_t data[DATA_BUFFER_SIZE];

	/// @brief The pointer to the data in the buffer.
	/// @warning This pointer is not initialized until the buffer is set.
	/// If the data buffer has the same alignment as the data type,
	/// this pointer will be equal to the data pointer.
	/// Otherwise, this pointer will be larger than the data pointer,
	/// but within the bounds of the data buffer.
	void* aligned;
public:
	/**
	 * @brief Function to get the error flags.
	 * @return The error flags, according to DataBuffer::Error
	*/
	static uint8_t getErrFlags() {
		return errFlags;
	}

	/**
	 * @brief Function to clear the error flags.
	*/
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

	/**
	 * @brief Function to check if the buffer is set.
	 * @return True if the buffer is set, false otherwise.
	*/
	bool isDataSet() const {
		return isSet;
	}

	/**
	 * @brief Function to get the data in the buffer.
	 * @tparam T The type of the data to get.
	 * @return A reference to the data in the buffer.
	 * @warning This function does not check if the buffer is set.
	 * @warning This function does not check if the type of the data is correct.
	*/
	template <typename T>
	T& get() {
		static_assert(sizeof(T) <= DATA_BUFFER_SIZE, "DataBuffer size is too small");
		if(!isSet) {
			errFlags |= UNSET_GET_ERR;
		}
		return *reinterpret_cast<T*>(aligned);
	}

	/**
	 * @brief Function to set the data in the buffer.
	 * @tparam T The type of the data to set.
	 * @return A reference to the data in the buffer.
	 * @warning This function does not check if the buffer is set.
	 * @warning This function does not check if the type of the data is correct.
	*/
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

	/**
	 * @brief Function to set the data in the buffer.
	 * @tparam T The type of the data to set.
	 * @return A reference to the data in the buffer.
	 * @warning This function does not check if the buffer is set.
	 * @warning This function does not check if the type of the data is correct.
	*/
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

	/**
	 * @brief Function to clear the buffer.
	 * @warning This function does not check if the buffer is set.
	*/
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

	/**
	 * @brief Function to unset the data in the buffer.
	 * @tparam T The type of the data to unset.
	 * @warning This function does not check if the buffer is set.
	 * @warning This function does not check if the type of the data is correct.
	 * This function will call the destructor of the data.
	 * It is used by the auto-generated tearDown function.
	*/
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