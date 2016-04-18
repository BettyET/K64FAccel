/*
 * Queue.h
 *
 *  Created on: 18.04.2016
 *      Author: User
 */

#ifndef SOURCES_QUEUE_H_
#define SOURCES_QUEUE_H_

/*!
 * \brief Sends a string to the queue. Note that if the queue is full, it will block.
 * \param str Pointer to the string.
 */
void DATAQUEUE_SaveValue(const int16_t value);

/*!
 * \brief Returns the number of elements (characters) in the queue.
 * \return Number of characters in the queue.
 */
unsigned short DATAQUEUE_NofElements(void);

/*!
 * \brief Receives an a message character from the queue, and returns immediately if the queue is empty.
 * \return Message character, or '\0' if there was no message.
 */
int16_t DATAQUEUE_ReadValue(void);

/*! \brief Initializes the queue module */
void DATAQUEUE_Init(void);

/*! \brief Deinitializes the queue module */
void DATAQUEUE_Deinit(void);



#endif /* SOURCES_QUEUE_H_ */
