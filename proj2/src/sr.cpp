#include "../include/simulator.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <ctype.h>
#include <vector>
#include <string.h>
#include <limits>

using namespace std;
///* ******************************************************************
// ALTERNATING BIT AND GO-BACK-N NETWORK EMULATOR: VERSION 1.1  J.F.Kurose
//
//   This code should be used for PA2, unidirectional data transfer
//   protocols (from A to B). Network properties:
//   - one way network delay averages five time units (longer if there
//     are other messages in the channel for GBN), but can be larger
//   - packets can be corrupted (either the header or the data portion)
//     or lost, according to user-defined probabilities
//   - packets will be delivered in the order in which they were sent
//     (although some can be lost).
//**********************************************************************/

///********* STUDENTS WRITE THE NEXT SEVEN ROUTINES *********/

int windowSize;
int nextSeqnum = 0;

float increment = 20.0;
int sendBase = 0;
int recvBase = 0;

int earlistSendUnackedIndex = 0;


struct storePkt {
    pkt packet;
    int ackOrNot; //0 - not acked, 1 - acked
    float sendTime;
};

vector<storePkt> buffer_A;

storePkt buffer_B[1000] = {};


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
//

void printMsg(char* array){
    for (int i = 0; i < sizeof(array) ; i++){
        cout << array[i];
    }
    cout << endl;
}

//int checkMaxinBuffer(){
//    for(int i = sendBase)
//}

void findEarlistSentUnackedPkt(){
//    float sendTimeTemp = numeric_limits<float>::infinity();
    float sendTimeTemp = 3.402823e+38;
    /***************New ADDED******************/
//    int length = 0;
//    if(windowSize + sendBase <= 1000){
//        length = windowSize + sendBase;
//    } else {
//        length = 1000;
//    }


    /***************New ADDED******************/


    for(int i = sendBase; i < windowSize + sendBase; i++ ){
        if(buffer_A[i].ackOrNot == 0 && buffer_A[i].sendTime != 0){
            if(buffer_A[i].sendTime < sendTimeTemp){
                sendTimeTemp = buffer_A[i].sendTime;
                earlistSendUnackedIndex = i;
            }
        }
    }

//    for(int i = sendBase; i < windowSize + sendBase; i++){
//
//    }
}


///* called from layer 5, passed the data to be sent to other side */
void A_output(struct msg message)
{
    cout << "Send Message from Layer 5 : ";
    printMsg(message.data);

    struct storePkt newStorePkt;
    struct pkt newPacket;
    for(int i = 0; i < 20; i++){
        newPacket.payload[i] = message.data[i];
    }
    newStorePkt.packet = newPacket;
    buffer_A.push_back(newStorePkt);
    if(nextSeqnum < sendBase + windowSize){
        buffer_A[nextSeqnum].packet.seqnum = nextSeqnum;
        buffer_A[nextSeqnum].packet.acknum = 0;
        buffer_A[nextSeqnum].packet.checksum = checkSumFunc(buffer_A[nextSeqnum].packet);
        cout << "A_output Send data : ";
        printMsg(buffer_A[nextSeqnum].packet.payload);
        tolayer3(0, buffer_A[nextSeqnum].packet);
        buffer_A[nextSeqnum].ackOrNot = 0; //Unacked message
        buffer_A[nextSeqnum].sendTime = get_sim_time();
        if(sendBase == nextSeqnum){
            starttimer(0, increment);
        }
        nextSeqnum++;
    }
}


//
///* called from layer 3, when a packet arrives for layer 4 */
void A_input(struct pkt packet)
{
    cout << "Acked Message Received." << endl;
    int checkResult = checkSumFunc(packet);
    if(checkResult == packet.checksum){
        buffer_A[packet.acknum].ackOrNot = 1;

        while(buffer_A[sendBase].ackOrNot == 1){
            sendBase++;
        }
        cout << "Send base : " << sendBase << endl;
        cout << "Next Sequence : " << nextSeqnum << endl;

        if(sendBase == nextSeqnum){
            cout << "Stop timer ." << endl;
            stoptimer(0);
        } else {
            stoptimer(0);
            float loaclTime = get_sim_time();
            starttimer(0, buffer_A[sendBase].sendTime + increment - loaclTime);
        }
    } else {
        cout << "Received Corrupt Message." << endl;
    }
}
//
///* called when A's timer goes off */
void A_timerinterrupt()
{
//    cout << "Time out, resend Mesasge." << endl;
//    cout << "Earlist sent unacked Message Index : " << earlistSendUnackedIndex << endl;
//    cout << "Send Message : ";
    //cout << earlistSendUnackedIndex << endl;
    //printMsg(buffer_A[earlistSendUnackedIndex].packet.payload);
    tolayer3(0, buffer_A[earlistSendUnackedIndex].packet); //Only send the unacked one, is different from gbn
    buffer_A[earlistSendUnackedIndex].sendTime = get_sim_time(); //Resend Message will change the sendTime in buffer_A
//    cout << "earlist sendTime : " << buffer_A[earlistSendUnackedIndex].sendTime << endl;
    findEarlistSentUnackedPkt();
    cout << earlistSendUnackedIndex << endl;
//    cout << "Current buffer_A size : " << buffer_A.size() << endl;
//    cout << "New Earlist unacked Message Index : " << earlistSendUnackedIndex << endl;
//    for(int i = 0; i < buffer_A.size(); i ++){
//        cout << "Element in Buferer A sendTme : " << buffer_A[i].sendTime;
//        printMsg(buffer_A[i].packet.payload);
//        cout << endl;
//    }
    cout << "earlist sendTime : " << buffer_A[earlistSendUnackedIndex].sendTime << endl;
    float timeTemp =get_sim_time();
    starttimer(0, buffer_A[earlistSendUnackedIndex].sendTime + increment - timeTemp);

}
//
///* the following routine will be called once (only) before any other */
///* entity A routines are called. You can use it to do any initialization */
void A_init()
{
    windowSize = getwinsize();
    cout << windowSize << endl;


}
//
///* Note that with simplex transfer from a-to-B, there is no B_output() */
//
///* called from layer 3, when a packet arrives for layer 4 at B*/
void B_input(struct pkt packet)
{
    cout << "B_input received packet : ";
    printMsg(packet.payload);
    cout << packet.seqnum  << "," << packet.acknum << endl;
    struct pkt newAckPacket;
    int checksumResult = checkSumFunc(packet);
    if(checksumResult == packet.checksum){
        if(buffer_B[packet.seqnum].ackOrNot != 1){
            cout << "Send Ack Message. " << endl;
            newAckPacket.acknum = packet.seqnum;
            newAckPacket.seqnum = 0;
            memset(newAckPacket.payload, 0, 20);
            newAckPacket.checksum = checkSumFunc(newAckPacket);
            tolayer3(1, newAckPacket);

            if(recvBase == packet.seqnum){
                buffer_B[recvBase].packet = packet;
                buffer_B[recvBase].ackOrNot = 1;

                while(buffer_B[recvBase].ackOrNot == 1){
                    cout << "deliver Message : ";
                    printMsg(buffer_B[recvBase].packet.payload);
                    tolayer5(1, buffer_B[recvBase].packet.payload);
                    recvBase++;
                }

            } else {
                buffer_B[packet.seqnum].packet = packet;
                buffer_B[packet.seqnum].ackOrNot = 1;
            }
        } else {
            cout << "Duplicated message." << endl;
            cout << "Send Ack Message. " << endl;
            newAckPacket.acknum = packet.seqnum;
            newAckPacket.seqnum = 0;
            memset(newAckPacket.payload, 0, 20);
            newAckPacket.checksum = checkSumFunc(newAckPacket);
            tolayer3(1, newAckPacket);
        }

    } else {
        cout << " Corrupted Message, discard." << endl;
    }

}
//
///* the following rouytine will be called once (only) before any other */
///* entity B routines are called. You can use it to do any initialization */
void B_init()
{

}
