/*
 * circular_buffer.c
 *
 *      Author: Arnav Mehta
 */

#include "circular_buffer.h"

//*****************************************************************************
// Initializes a circular buffer.
//
// Parameters
//    max_size:   Number of entries in the circular buffer.
//*****************************************************************************
Circular_Buffer * circular_buffer_init(uint16_t max_size)
{
    Circular_Buffer *buffer = malloc(sizeof(Circular_Buffer));
   buffer->data = malloc(max_size);
   buffer->max_size = max_size;
   buffer->produce_count = 0;
   buffer->consume_count = 0;
   return buffer;
}

//*****************************************************************************
// Returns a circular buffer to the heap
//
// Parameters
//    buffer  :   The address of the circular buffer.
//*****************************************************************************
void circular_buffer_delete(Circular_Buffer * buffer)
{
   free((void *)buffer->data);
   free(buffer);
}

//*****************************************************************************
// Adds a character to the circular buffer.
//
// Parameters
//    buffer  :   The address of the circular buffer.
//    c       :   Character to add.
//*******************************************************************************
bool circular_buffer_add(Circular_Buffer *buffer, char c)
{
  // If the circular buffer is full, return false.
    if(circular_buffer_full(buffer)) {
        return false;
    }

  // Add the data to the circular buffer
    buffer->data[(buffer->produce_count)  % (buffer->max_size)] = c;
    buffer->produce_count += 1; //(((buffer->produce_count) + 1) % (buffer->max_size));

  // Return true to indicate that the data was added to the
  // circular buffer.
    return true;

}

//*****************************************************************************
// Removes the oldest character from the circular buffer.
//
// Parameters
//    buffer  :   The address of the circular buffer.
//
// Returns
//    The character removed from the circular buffer.  If the circular buffer
//    is empty, the value of the character returned is set to 0.
//*****************************************************************************
char circular_buffer_remove(Circular_Buffer *buffer)
{
  char return_char;

  // If the circular buffer is empty, return 0.
  if (circular_buffer_empty(buffer)) {
          return 0;
  }

  // remove the character from the circular buffer
  return_char = buffer->data[(buffer->consume_count)  % (buffer->max_size)];
  buffer->consume_count += 1;

  // return the character
  return return_char;

}

//*****************************************************************************
// Returns true if the circular buffer is empty.  Returns false if it is not.
//
// Parameters
//    buffer  :   The address of the circular buffer.
//*****************************************************************************
bool circular_buffer_empty(Circular_Buffer *buffer)
{
    if ((buffer->produce_count) == (buffer->consume_count)) {
        return true;
    } else {
        return false;
    }
}

//*****************************************************************************
// Returns true if the circular buffer is full.  Returns false if it is not.
//
// Parameters
//    buffer  :   The address of the circular buffer.
//*****************************************************************************
bool circular_buffer_full(Circular_Buffer *buffer)
{
    if(((buffer->produce_count) - (buffer->consume_count)) == (buffer->max_size)) {
        return true;
    } else {
        return false;
    }
}


/******************************************************************************
 * Circular Buffer Test 0
 *
 * Description:
 *  1. Create a circular buffer based on the size passed into the test function
 *  2. Add data to the circular buffer until it is full using a for loop.  Each time
 *     the for loop is executed, add loop count to the circular buffer.
 *  3. Verify that adding an additional byte of data fails
 *  4. Remove the data one byte at a time and verify the data removed matches
 *     the expected value
 *  5. Return the circular buffer to the heap.
 *
 ******************************************************************************/
bool circular_buffer_test_0(uint16_t size)
{
    Circular_Buffer *circular_buffer;
    int i ;
    char data;
    //bool return_status;

    // create a new circular buffer
    circular_buffer = circular_buffer_init(size);

    // Using a for loop, Add the value of i to the circular buffer
    // until it is full.
    for(i = 0; i < size; i++)
    {
        // Add the next byte of data
        // If the data was not added, return the circular
        // buffer to the heap and return false
        if (!circular_buffer_add(circular_buffer, i)) {
            circular_buffer_delete(circular_buffer);
            return false;
        }
    }

    // Verify that the buffer is full. If it is not full
    // return the circular buffer to the heap and return false
    if(!circular_buffer_full(circular_buffer)) {
        circular_buffer_delete(circular_buffer);
        return false;
    }

    // validate that you cannot add data to the circular buffer
    // after it is full.  If the return value is is equal to true,
    // return the circular buffer to the heap and return false.

    if (circular_buffer_add(circular_buffer, i)) {
        circular_buffer_delete(circular_buffer);
        return false;
    }

    // remove the data from the circular buffer one byte at a time
    for(i = 0; i < size; i++)
    {
        // Remove the next byte of data
        data = circular_buffer_remove(circular_buffer);
        // Verify that the value returned. If the value returned
        // is not correct, return the circular buffer to the heap
        // and return false
        if(data != i) {
            circular_buffer_delete(circular_buffer);
            return false;
        }
    }

    // Verify that the buffer is empty. If it is not empty
    // return the circular buffer to the heap and return false
    if(!circular_buffer_empty(circular_buffer)) {
        circular_buffer_delete(circular_buffer);
        return false;
    }
    // return the circular buffer to the heap.
    circular_buffer_delete(circular_buffer);

    // success!!
    return true;

}


