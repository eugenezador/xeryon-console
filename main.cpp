#include <iostream>
#include "Xeryon.h"

#include <stdio.h>

int main() {

// int main(int argc, char** argv) {

    Xeryon * controller = new Xeryon("/dev/ttyACM0", 9600);
    Axis * axisX = controller->addAxis('X', &/*XLS_312*/XRTU_30_109);


    controller->start();
//    std::this_thread::sleep_for(std::chrono::milliseconds(4000));

    axisX->setSetting("FREQ", 171500);
    axisX->setSetting("HFRQ", 180000);
    axisX->setSetting("LFRQ", 160000);

    axisX->setSetting("PROP", 15);
    axisX->setSetting("MPRO", 300);
//    axisX->setSetting("MASS", 155);
    axisX->setSetting("CFRQ", 30000);
    axisX->setSetting("MSPD", 720);
    axisX->setSetting("SSPD", 125);
    axisX->setSetting("ELIM", 3000);
    axisX->setSetting("DUCO", 1);
    axisX->setSetting("ACTD", 0);
    axisX->setSetting("PTOL", 30);
    axisX->setSetting("PTO2", 100);
    axisX->setSetting("TOUT", 3000);
    axisX->setSetting("POLI", 97);
//    axisX->setSetting("DLAY", 100);
    axisX->setSetting("TOU2", 3000);
    axisX->setSetting("TOUT", 3000);
        axisX->setSetting("ENCO", 0);
        axisX->setSetting("PHAC", -300);




//    axisX->setSpeed(125_mm);
//    axisX->setSetting("TOUT", 3000);
//    axisX->setSetting("POLI", 97);
//    axisX->setSetting("DLAY", 100);
//    axisX->setSetting("TOU2", 3000);
//    axisX->setSetting("PTOL", 30);
//    axisX->setSetting("PTO2", 100);
    axisX->findIndex();
//    axisX->calibrate();


        printf("axis DPOS %d\n", axisX->getData("DPOS"));
        printf("axis EPOS %d\n", axisX->getData("EPOS"));
        printf("axis STAT %d\n", axisX->getData("STAT"));
    axisX->setDPOS(10_mu);

    axisX->setDPOS(20_mu);

    axisX->setDPOS(30_mu);
//       axisX->setDPOS(60_mu);
//axisX->getSetting("TOUT");
printf("axis TOUT %d\n", axisX->getSetting("TOUT"));
printf("axis POLI %d\n", axisX->getSetting("POLI"));
    printf("axis DPOS %d\n", axisX->getData("DPOS"));
    printf("axis EPOS %d\n", axisX->getData("EPOS"));
    printf("axis STAT %d\n", axisX->getData("STAT"));

//    std::

//    axisX->findIndex();
//    printf("axis DPOS %d\n", axisX->getData("DPOS"));
//    printf("axis EPOS %d\n", axisX->getData("EPOS"));
//    printf("axis STAT %d\n", axisX->getData("STAT"));

//    axisX->setSpeed(20_mu);
//    std::this_thread::sleep_for(std::chrono::milliseconds(4000));
//    axisX->setDPOS(50_mu);
//    axisX->setDPOS(60_mu);
//    axisX->setDPOS(0_mu);
//    printf("DPOS: %lf\n", (double)axisX->getDPOS());
//    printf("EPOS: %lf\n", (double)axisX->getEPOS());

//    axisX->setDPOS(10_mm);
//    printf("DPOS: %lf\n", (double)axisX->getDPOS());
//    printf("EPOS: %lf\n", (double)axisX->getEPOS());

//    axisX->setDPOS(0_mm);
//    printf("DPOS: %lf\n", (double)axisX->getDPOS());
//    printf("EPOS: %lf\n", (double)axisX->getEPOS());

//    axisX->setDPOS(5_mm);
//    printf("DPOS: %lf\n", (double)axisX->getDPOS());
//    printf("EPOS: %lf\n", (double)axisX->getEPOS());

//    for (int i = 0; i < 5; i++)
//        axisX->step(1_mu);

    controller->stop();
    return 0;
}
