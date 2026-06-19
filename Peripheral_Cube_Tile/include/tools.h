#ifndef TOOLS_H
#define TOOLS_H
#ifndef MAX_ARGUMENT_LENGTH
#define MAX_ARGUMENT_LENGTH 8
#endif

#include "base.h"

/** Functions declarations */

uint8_t get_checksum(uint8_t* bytes_array, uint16_t number_of_bytes);
bool is_checksum_ok(uint8_t* bytes_array, uint16_t number_of_bytes);


/** Functions implementations */

bool is_checksum_ok(uint8_t* bytes_array, uint16_t number_of_bytes)
{
	uint8_t LRC = get_checksum(bytes_array, number_of_bytes);
	
	return(LRC == 0);
}

// Longitudinal Redundancy Check function
// Source: https://en.wikipedia.org/wiki/Longitudinal_redundancy_check
uint8_t get_checksum(uint8_t* bytes_array, uint16_t number_of_bytes)
{
	if(bytes_array == NULL || number_of_bytes == 0)
		return 0;
	uint8_t LRC = bytes_array[0];
	uint8_t i = 1;
	/* while(bytes_array[i] != 0 && i < MAX_ARGUMENT_LENGTH) */
	while(i < number_of_bytes)
	{
		LRC ^= bytes_array[i];
		i++;
	}
	return LRC;
}

/** 

	Base struct to be inherited by elements that need to have an ID and a name.

*/
struct IdBase
{
	char name[7] = "";
	uint8_t id = 0;
};

/** 
	ArrayManipulator class

	Used to reorder, delete or insert array_elements in any type of array.
*/
class ArrayManipulator : public Base
{
public:
	void configure(void* _array_start, uint16_t* _current_size_of_array, uint8_t _size_of_element, int16_t _max_size_of_array);

	void delete_array_element(uint16_t index);
	void insert_array_element(uint16_t index, void* element);

	void initialize_array();
private:
	bool is_configured = false;
	void* array_start = NULL;
	uint16_t* current_size_of_array = 0; // Using a pointer so that we don't need to update it all the time
	uint16_t max_size_of_array = 0;
	uint8_t size_of_element = 0;

	void reorder_array();

	bool is_array_element_empty(uint8_t index);
	void clear_array_element(uint8_t index);
};

void ArrayManipulator::configure(void* _array_start, uint16_t* _current_size_of_array, uint8_t _size_of_element, int16_t _max_size_of_array = 10)
{
	if(!current_size_of_array)
	{
	
	}
		
	is_configured = true;
	array_start = _array_start;
	current_size_of_array = _current_size_of_array;
	size_of_element = _size_of_element;
	max_size_of_array = _max_size_of_array;

	initialize_array();
}	
// Clears an element of the array and reorders the array
void ArrayManipulator::delete_array_element(uint16_t index)
{
	if(!is_configured)
		return;
	debug.print(F("Deleting element "));
	debug.print(index);
	debug.println();

	memset(((uint8_t*)array_start)+index*size_of_element, 0, size_of_element);

	reorder_array();
}

void ArrayManipulator::initialize_array()
{
	debug.print(F("Initializing array. Max "));
	debug.print(max_size_of_array);
	debug.print(F(" elements."));
	debug.println();
	
	for(uint8_t i = 0; i < max_size_of_array; i++)
	{
		clear_array_element(i);
	}
}

void ArrayManipulator::reorder_array()
{
	if(!is_configured)
		return;
			
	debug.print(F("Reordering array, currently "));
	debug.print(*current_size_of_array);
	debug.print(F(" elements."));
	debug.println();

	uint8_t number_of_empty_elements = 0;

	// Since this looks one element ahead, do it for one less time than the size of the array.
	for(uint8_t i = 0; i < *current_size_of_array - 1; i++)
	{
		if(is_array_element_empty(i))
		{
			number_of_empty_elements++;
			// Copy element of next index to current index
			debug.print(F("Moving element "));
			debug.print((uint8_t)(i+1));
			debug.print(F(" to index "));
			debug.print(i);
			debug.println();
			
			memcpy(((uint8_t*)array_start) + i * size_of_element, ((uint8_t*)array_start) + (i + 1) * size_of_element, size_of_element);

			// Delete next element
			clear_array_element(i+1);
		}
	}
}

// Make sure that all bytes of the specified element are zero
bool ArrayManipulator::is_array_element_empty(uint8_t index)
{
	bool is_empty = true;
	for(uint8_t i = 0; i < size_of_element; i++)
	{
		if(*(((uint8_t*)array_start) + (index * size_of_element) + i) != 0)
		{
			debug.print(F("Element at index "));
			debug.print(index);
			debug.print(F(" not empty"));
			debug.println();
			
			is_empty = false;
			break;
		}
	}
	return is_empty;
	// return((*(uint8_t*)(array_start + index * size_of_element)) == 0);
}

// Set specified array element to zero
void ArrayManipulator::clear_array_element(uint8_t index)
{
	if(!is_configured)
		return;

	debug.print(F("Clearing index "));
	debug.print(index);
	debug.print(F(" from array."));
	debug.println();
	memset(((uint8_t*)array_start) + index * size_of_element, 0, size_of_element);
}


#endif







