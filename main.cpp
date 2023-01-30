#include <iostream>
#include "Xeryon.h"

#include <stdio.h>

int main() {

    Xeryon * controller = new Xeryon("/dev/ttyACM0", 115200);
    Axis * axisX = controller->addAxis('X', &XRTU_30_109);


    controller->start();

    axisX->findIndex();

    printf("axis DPOS %d\n", axisX->getData("DPOS"));
    printf("axis EPOS %d\n", axisX->getData("EPOS"));
    printf("axis STAT %d\n", axisX->getData("STAT"));


    Distance dist;
    dist.move_motor(120, "mm");

    axisX->setDPOS(dist);

//    axisX->setDPOS(100_mu);


    printf("2axis DPOS %d\n", axisX->getData("DPOS"));
    printf("2axis EPOS %d\n", axisX->getData("EPOS"));
    printf("2axis STAT %d\n", axisX->getData("STAT"));


    controller->stop();
    return 0;
}
