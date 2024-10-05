#include <stdio.h>
#include <stdlib.h>

/**
 * @brief Structure representing an item in the queue.
 */
struct queue_item {
    int value;               /**< Value of the queue item */
    struct queue_item *next; /**< Pointer to the next item in the queue */
};

/**
 * @brief Structure representing the queue.
 */
struct queue {
    struct queue_item *head; /**< Pointer to the head of the queue */
    struct queue_item *tail; /**< Pointer to the tail of the queue */
};

/**
 * @brief Pushes a value onto the back of the queue.
 *
 * @param q Pointer to the queue.
 * @param value Value to be pushed onto the queue.
 * @return int 1 if the operation was successful, 0 otherwise.
 */
int queue_push(struct queue *const q, const int value) {
    struct queue_item *item = malloc(sizeof(struct queue_item));
    if (item == NULL)
        return 0;

    *item = (struct queue_item){.value = value, .next = NULL};

    if (q->head == NULL) {
        q->head = q->tail = item;
    } else {
        q->tail->next = item;
        q->tail = item;
    }
    return 1;
}

/**
 * @brief Pops a value from the front of the queue.
 *
 * @param q Pointer to the queue.
 * @param result Pointer to an integer where the popped value will be stored.
 * @return int 1 if the operation was successful, 0 otherwise.
 */
int queue_pop(struct queue *const q, int *const result) {
    if (q->head == NULL)
        return 0;
    struct queue_item *item = q->head;
    if (item->next == NULL)
        q->head = q->tail = NULL;
    else
        q->head = item->next;

    *result = item->value;
    free(item);
    return 1;
}

#define TEST_TIMES 10

/**
 * @brief Main function to test the queue operations.
 *
 * @return int Exit status of the program.
 */
int main() {
    struct queue q = {.head = NULL, .tail = NULL};

    for (int i = 0; i < TEST_TIMES; i++)
        if (queue_push(&q, i))
            printf("push to back: %d\n", i);
        else
            printf("push to back fail\n");

    for (int i = 0; i < TEST_TIMES + 1; i++) {
        int value;
        if (queue_pop(&q, &value))
            printf("pop from front: %d\n", value);
        else
            printf("pop from front fail\n");
    }

    return 0;
}
