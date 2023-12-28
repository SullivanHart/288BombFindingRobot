/*
 * movement functions for CyBot
 *
 * 12\2\2023
 * @author Daniel, Laith, Logan, Sullivan
 */

#include <stdio.h>
#include "open_interface.h"
#include "movement.h"
#include "uart.h"

//  Move the bot x centimeters backward
void move_backward(oi_t *sensor, int centimeters, int speed, int cliff){
    int cliff_detect = 0;

    double sum = 0;
    int distance = centimeters * 10;
    oi_setWheels(speed * -1, speed * -1); // move forward; full speed
    oi_update(sensor);

    while (sum < distance) {
        oi_update(sensor);
        sum += sensor->distance * -1;

        if ((sensor->cliffFrontLeftSignal > 2500 || sensor->cliffFrontRightSignal > 2500) && cliff){
            move_forward(sensor, 10, 100, 0);
            cliff_detect = 1;
            break;
        }

        if ((sensor->cliffLeftSignal > 2700 || sensor->cliffRightSignal > 2700) && cliff){
            move_forward(sensor, 15, 100, 0);
            cliff_detect = 1;
            break;
        }


        if ((sensor->cliffFrontLeftSignal < 1000 ||  sensor->cliffLeftSignal < 1000 ) && cliff){
            move_forward(sensor, 10, 100, 0);
            cliff_detect = 1;
            break;
        }

        if (( sensor->cliffFrontRightSignal < 1000|| sensor->cliffRightSignal < 1000) && cliff){
            move_forward(sensor, 15, 100, 0);
            cliff_detect = 1;
            break;
        }
    }

    if(cliff_detect && cliff){
        uart_sendStr("Backed into a cliff! Bot repositioned itself");
    }
    else if (cliff){
        uart_sendStr("Moved backward 5cm");
    }


    oi_setWheels(0, 0); // stop
}

//  Move the bot x centimeters forward
void move_forward(oi_t *sensor, int centimeters, int speed, int cliff){

    double sum = 0;
    int distance = centimeters * 10;
    oi_setWheels(speed, speed); // move forward; full speed
    oi_update(sensor);
    int cliff_detect = 0;
    int hole_right = 0;
    int hole_left = 0;

    while (sum < distance) {
        oi_update(sensor);
        sum += sensor->distance;

        if (sensor->bumpLeft == true || sensor->bumpRight == true){
            bump(sensor);
            cliff = 0;
            break;
        }

        if ((sensor->cliffFrontLeftSignal > 2500 || sensor->cliffFrontRightSignal > 2500) && cliff){
            move_backward(sensor, 10, 100, 0);
            cliff_detect = 1;
            break;
        }

        if ((sensor->cliffLeftSignal > 2700 || sensor->cliffRightSignal > 2700) && cliff){
            move_backward(sensor, 15, 100, 0);
            cliff_detect = 1;
            break;
        }

        if ((sensor->cliffFrontLeftSignal < 1000 || sensor->cliffFrontRightSignal < 1000 ) && cliff){
            move_backward(sensor, 10, 100, 0);
            hole_left = 1;
            hole_right = 1;
            break;
        }

        if ((sensor->cliffFrontLeftSignal < 1000 ||sensor->cliffLeftSignal < 1000) && cliff){
            move_backward(sensor, 10, 100, 0);
            hole_left = 1;
            break;
        }

        if ((sensor->cliffFrontRightSignal < 1000 || sensor->cliffRightSignal < 1000) && cliff){
            move_backward(sensor, 15, 100, 0);
            hole_right = 1;
            break;
        }
    }

    if(cliff_detect && cliff){
        uart_sendStr("Ran into a cliff! Bot repositioned itself");
    } else if (hole_left && hole_right && cliff){
        uart_sendStr("Hole front");
    } else if (hole_left && cliff){
        uart_sendStr("Hole left");
    } else if (hole_right && cliff){
        uart_sendStr("Hole right");
    }else if (cliff){
        uart_sendStr("Moved forward 5cm");
    }

    oi_setWheels(0, 0); // stop
}


void bump(oi_t *sensor_data){

    int left = 0;
    int right = 0;

    if (sensor_data->bumpLeft == true){

        oi_setWheels(-100, -100);
        left = 1;
        double back_up = 0;

        while (back_up < 100){
            oi_update(sensor_data);
            back_up += sensor_data->distance * -1;
        }
    }

    else if (sensor_data->bumpRight == true){

        oi_setWheels(-100, -100);
        right = 1;
        double back_up = 0;

        while (back_up < 100){
            oi_update(sensor_data);
            back_up += sensor_data->distance * -1;
        }
    }

    if (right){
        uart_sendStr("right");
    } else if (left){
        uart_sendStr("left");
    }

    oi_setWheels(0, 0); // stop
}

// Turn the bot x degrees clockwise
void turn_clockwise(oi_t *sensor_data, int degrees){

    double sum = 0;
    degrees = degrees - (degrees * 0.10);

    oi_setWheels(-100, 100); // trun the bot clockwise
    oi_update(sensor_data);

    while (sum < (degrees)){
        oi_update(sensor_data);
        sum += sensor_data->angle * -1;
    }

    oi_setWheels(0, 0); // stop
}

// Turn the bot x degrees counterclockwise
void turn_counterclockwise(oi_t *sensor_data, int degrees){

    double sum = 0;
    degrees = degrees - (degrees * 0.10);

    oi_setWheels(100, -100); // trun the bot counterclockwise
    oi_update(sensor_data);

    while (sum < (degrees)){
        oi_update(sensor_data);
        sum += sensor_data->angle;
    }

    oi_setWheels(0, 0); // stop
}
