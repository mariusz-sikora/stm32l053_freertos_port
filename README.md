This project is FreeRTOS hello world application for STM32L053 microcontroller created in KEIL µVision V5.13.0.0.
Application creates two threads producer and consumer. Each thread maintains own counter.
Producer increments owned counter and pushes updated values to the queue.
Consumer takes counter updates from the queue and assigns to the owned counter.
