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


    //    Distance dist;
    //    degree_to_Distance(120);
    //    dist.move_motor(120, "mm");

    axisX->setDPOS(Distance::degree_to_Distance(92));
    for (int i = 1;i<30*20;i++)
    {
    axisX->step(Distance::degree_to_Distance(0.05));
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}
    //    axisX->setDPOS(100_mu);


    printf("2axis DPOS %d\n", axisX->getData("DPOS"));
    printf("2axis EPOS %d\n", axisX->getData("EPOS"));
    printf("2axis STAT %d\n", axisX->getData("STAT"));


    controller->stop();
    return 0;
}
