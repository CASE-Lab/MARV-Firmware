
### Notes on NCU program

* Motor driver only responds when 12V auto is active.
* There is a sense line for 12V auto on PA_1. Need to check this and perform configuration when 12V auto turns on.

### Control Loop with Angle Sensor Polling
* Idea is to try and get rid of interrupt and instead use polling inside the control loop to check state for the ADC.
* For the filter implementation it is important that the sensor values are sampled with low jitter and an even period.
* Goal is to try and fit as many samples as possible in a single control loop iteration.
* Timings assume motor driver baudrate of 115200
* Sensor is polled with an even rate every 3.4 ms (approximately 300 Hz)
* Full loop takes 17 ms giving a loop rate of around 60 Hz

|      | Time:   | Action:          | Duration:  | Comments                        |
| ---- | ------- | ---------------- | ---------- | ------------------------------- |
| 0    | 0 ms    | Poll Sensor      | fast       |                                 |
|      |         | Poll State       | fast       | Checks 12V auto state           |
|      |         | Motor Set Speed  | ~2.6 ms    |                                 |
| 2.6  | 2.6 ms  | Above done       |            |                                 |
|      |         | Idle time        | ~0.8 ms    |                                 |
| 3.7  | 3.5 ms  | Poll Sensor      | fast       |                                 |
|      |         | Idle time        | ~3.4 ms    | Debug print?                    |
| 7.4  | 7 ms    | Poll Sensor      | fast       |                                 |
|      | 7.4     | Motor ready      |            | Only for info, no action needed |
| 7.4  | 7.4 ms  | Motor Read Speed | ~2.6 ms    | > 4.8 ms after Motor Set Speed  |
|      | 10 ms   | Above done       |            |                                 |
|      |         | Idle time        | ~0.2 ms    |                                 |
| 11.1 | 10.5 ms | Poll Sensor      | fast       |                                 |
|      |         | Idle time        | ~3.4 ms    |                                 |
|      | 14      | Poll Sensor      |            |                                 |
|      |         | Idle time        | /          |                                 |
|      | 14.8 ms | Motor ready      | \| ~3.4 ms | Only for info, no action needed |
|      |         | Idle time        | \          |                                 |
| 14.8 | 17 ms   | Start next loop  |            |                                 |

