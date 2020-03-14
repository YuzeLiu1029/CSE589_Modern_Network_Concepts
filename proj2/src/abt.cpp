#include "../include/simulator.h"
#include <iostream>
#include <queue>
#include <string.h>
#include <stdio.h>
#include <cstring>
//#include <string.h>
//#include <stdio.h>

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

/********* STUDENTS WRITE THE NEXT SEVEN ROUTINES *********/

using namespace std;




queue<msg> bufferQueue;
struct msg backUp;



int sequenceTemp = 0;
int fsm = 0; //Finite State Machine Status : 0 - wait for call from above; 1 - wait for ACK;
int BexpectedSequence = 0;
int resendMsg = 0; //0 - is not a resend message; 1 - is resend message

int increment = 15.0;




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





/**********************************************************************************************************************/
/* called from layer 5, passed the data to be sent to other side */
void A_output(struct msg message)
{
    struct pkt newPacket;
    newPacket.seqnum = sequenceTemp % 2;
    newPacket.acknum = 0; //doesn't matter what is the actual value

    switch(fsm){
        case 0: //message can be sent at this point
            if(bufferQueue.size() > 0) {//buffer is not empty, send the message in buffer as the payload of newpacket
                cout << "buffer is not empty" << endl;
                struct msg currentMsg;
                currentMsg = bufferQueue.front();

                for (int i=0; i<20; i++)
                {
                    newPacket.payload[i]=currentMsg.data[i];
                    backUp.data[i]=currentMsg.data[i];
                }

                bufferQueue.pop();
                cout << "Put the message in the buffer case 0." << endl;
                printMsg(message.data);
                bufferQueue.push(message); //Store the new Message to the buffer;
                newPacket.checksum = checkSumFunc(newPacket); //Packet is ready to send;
                starttimer(0, increment);
                tolayer3(0, newPacket);
                fsm = 1;
                cout << "Send the message : ";
                printMsg(newPacket.payload);
            } else { //buffer is empty, send the message from above
                cout << "buffer is empty" << endl;
                printMsg(message.data);
                for (int i=0; i<sizeof(message.data); i++)
                {
                    newPacket.payload[i]=message.data[i];
                    backUp.data[i]=message.data[i];
                }
                printMsg(message.data);
                newPacket.checksum = checkSumFunc(newPacket); //Packet is ready to send;
//                starttimer(0, increment);
                cout << "Data in Packet " << newPacket.seqnum << " , " << newPacket.acknum << endl;
                tolayer3(0, newPacket);
                starttimer(0, increment);
                printMsg(newPacket.payload);
                fsm = 1;
            }
            break;

        case 1:
            bufferQueue.push(message);
            printMsg(message.data);
            cout << "Put the message in the buffer case." << endl;
            break;
    }



}
/**********************************************************************************************************************/




/**********************************************************************************************************************/
/* called from layer 3, when a packet arrives for layer 4 */
void A_input(struct pkt packet)
{
    struct msg newMsg;
    //stoptimer(0);


    if(packet.acknum == 1 && sequenceTemp % 2 == 1){
        cout << "get ack message successsfully" << endl;
        sequenceTemp  = sequenceTemp + 1;
        fsm = 0;
        stoptimer(0);
    } else if(packet.acknum == 0 && sequenceTemp % 2 == 0){
        cout << "get ack message successsfully" << endl;
        sequenceTemp = sequenceTemp + 1;
        fsm = 0;
        stoptimer(0);
    } else {
        cout << "Discard Message." << endl;
//        //resend the message;
//        struct pkt rePacket;
//        rePacket.seqnum = sequenceTemp % 2;
//        rePacket.acknum = 0;
//        for(int i = 0; i < 20; i++){
//            rePacket.payload[i] == backUp.data[i];
//        }
//        rePacket.checksum = checkSumFunc(rePacket);
//        tolayer3(0, rePacket);
    }
//    stoptimer(0);

}
/**********************************************************************************************************************/




/**********************************************************************************************************************/
/* called when A's timer goes off */
void A_timerinterrupt()
{
    //stoptimer(0);
    cout << "Time Out! Resend the last message!" << endl;
    printMsg(backUp.data);
    cout << "Sequence Number : " << sequenceTemp << endl;
    struct pkt resendPacket;
    resendPacket.seqnum = sequenceTemp % 2;
    resendPacket.acknum = 0;
    for(int i = 0; i < 20; i++){
        resendPacket.payload[i] = backUp.data[i];
    }
    resendPacket.checksum = checkSumFunc(resendPacket);
    //starttimer(0, increment);
    tolayer3(0, resendPacket);
    starttimer(0, increment);

}
/**********************************************************************************************************************/



/**********************************************************************************************************************/
/* the following routine will be called once (only) before any other */
/* entity A routines are called. You can use it to do any initialization */
void A_init()
{

}
/**********************************************************************************************************************/

/* Note that with simplex transfer from a-to-B, there is no B_output() */

/**********************************************************************************************************************/
/* called from layer 3, when a packet arrives for layer 4 at B*/
void B_input(struct pkt packet)
{
    int BSidecheckSum;
    struct msg newMsg;
    struct pkt ackPacket;

    BSidecheckSum = checkSumFunc(packet);

    if(BSidecheckSum != packet.checksum){
        cout << "Checksum for this message : " ;
        printMsg(packet.payload);
        cout << "Checksum returns false. " << endl;

    } else {
        if( (BexpectedSequence % 2) == packet.seqnum){
            for (int i=0; i<sizeof(packet.payload); i++)
            {
                newMsg.data[i]=packet.payload[i];
            }
            cout << "Try to deliver the message." << endl;
            tolayer5(1, newMsg.data);
            BexpectedSequence++;
            printMsg(newMsg.data);
            cout << "Deliver the message successfully. " << endl;
            cout << "Sequence Number : " << packet.seqnum << endl;
            ackPacket.acknum = packet.seqnum;
            ackPacket.seqnum = packet.seqnum;
            cout << "Send Back the ack : " << ackPacket.acknum << endl;
            memset(ackPacket.payload, 0, 20);
            ackPacket.checksum = checkSumFunc(ackPacket);
            tolayer3(1, ackPacket);
        } else {
            cout << "Dupliacted Detected." << endl;
            cout << BexpectedSequence << endl;
            cout << packet.seqnum << endl;
            cout << "Duplicated Message, ignore." << endl;
            ackPacket.acknum = packet.seqnum;
            ackPacket.seqnum = packet.seqnum;
            memset(ackPacket.payload, 0, 20);
            ackPacket.checksum = checkSumFunc(ackPacket);
            tolayer3(1,ackPacket);
        }

    }
}
/**********************************************************************************************************************/


/**********************************************************************************************************************/
/* the following rouytine will be called once (only) before any other */
/* entity B routines are called. You can use it to do any initialization */
void B_init()
{

}
/**********************************************************************************************************************/