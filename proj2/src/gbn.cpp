#include "../include/simulator.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
//#include <getopt.h>
#include <ctype.h>
#include <vector>
#include <string.h>

using namespace std;
/* ******************************************************************
 ALTERNATING BIT AND GO-BACK-N NETWORK EMULATOR: VERSION 1.1  J.F.Kurose

   This code should be used for PA2, unidirectional data transfer 
   protocols (from A to B). Network properties:
   - one way network delay averages five time units (longer if there
     are other messages in the channel for GBN), but can be larger
   - packets can be corrupted (either the header or the data portion)
     or lost, according to user-defined probabilities
   - packets will be delivered in the order in which they were sent
     (although some can be lost).
**********************************************************************/



int windowSize;
int nextSeqnum = 0;
int base = 0;

int expectedseqnum;

float increment = 25.0;

vector<pkt> bufferVec;



int checkSumFunc(struct pkt packet){
    int returnSum = 0;
    returnSum = returnSum + packet.seqnum;
    returnSum = returnSum + packet.acknum;
    cout << sizeof(packet.payload) << endl;
    for(int i = 0; i < sizeof(packet.payload);i++){
        returnSum = returnSum + packet.payload[i];
    }
    return returnSum;

}

void printMsg(char* array){
    for (int i = 0; i < sizeof(array) ; i++){
        cout << array[i];
    }
    cout << endl;
}



/* called from layer 5, passed the data to be sent to other side */
void A_output(struct msg message)
{
    cout << "A_output Send Data from layer5 : ";
    printMsg(message.data);
    struct pkt newPacket;
    for(int i = 0; i < 20; i++){
        newPacket.payload[i] = message.data[i];
    }
    bufferVec.push_back(newPacket);
    cout << "A_output Next Seqnum : " << nextSeqnum << endl;
    cout << "A_output Base : " << base << endl;
    cout << "A_output window size : " << windowSize << endl;
    if(nextSeqnum < base + windowSize){
        bufferVec[nextSeqnum].seqnum = nextSeqnum;
        bufferVec[nextSeqnum].acknum = 0;
        bufferVec[nextSeqnum].checksum = checkSumFunc(bufferVec[nextSeqnum]);
        cout << "A_output Send Data : ";
        printMsg(bufferVec[nextSeqnum].payload);
        tolayer3(0, bufferVec[nextSeqnum]);
        if(base == nextSeqnum){
            starttimer(0, increment);
        }
        nextSeqnum++;
    }else{
        cout << "A_output Refuse Data." << endl;
    }
}

/* called from layer 3, when a packet arrives for layer 4 */
void A_input(struct pkt packet)
{
    int checkResult = checkSumFunc(packet);
    if(checkResult == packet.checksum){
        base = packet.acknum + 1;
        if(base == nextSeqnum){
            stoptimer(0);

        }else{
            stoptimer(0);
            starttimer(0, increment);
        }
    } else {
        cout << "Corrupted Packet, wait." << endl;
    }

}

/* called when A's timer goes off */
void A_timerinterrupt()
{
    starttimer(0, increment);
    for(int i = base;i<nextSeqnum;i++){
        tolayer3(0, bufferVec[i]);
    }

}

/* the following routine will be called once (only) before any other */
/* entity A routines are called. You can use it to do any initialization */
void A_init()
{
    windowSize = getwinsize();
    cout << windowSize << endl;

}

/* Note that with simplex transfer from a-to-B, there is no B_output() */

/* called from layer 3, when a packet arrives for layer 4 at B*/
void B_input(struct pkt packet)
{
    cout << "B_input received packet : ";
    printMsg(packet.payload);
    struct pkt newAckPacket;
    int checksumResult = checkSumFunc(packet);
    if(checksumResult == packet.checksum && packet.seqnum == expectedseqnum){
        char message[20];
        for(int i = 0; i < 20; i++){
            message[i] = packet.payload[i];
        }
        tolayer5(1, message);
        newAckPacket.acknum = expectedseqnum;
        newAckPacket.seqnum = expectedseqnum;
        memset(newAckPacket.payload, 0, 20);
        newAckPacket.checksum = checkSumFunc(newAckPacket);
        tolayer3(1, newAckPacket);
        expectedseqnum++;
    }

}

/* the following rouytine will be called once (only) before any other */
/* entity B routines are called. You can use it to do any initialization */
void B_init()
{
    expectedseqnum = 0;

}



