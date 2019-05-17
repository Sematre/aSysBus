/*
  aSysBus CAN definitions

  @copyright 2019 Sematre

  This interface depends on the SparkFun_CAN-Bus_Arduino_Library library fork.
    Download: https://github.com/Sematre/CAN-Bus-Library
*/

#ifndef ASB_CAN2__H
#define ASB_CAN2__H
    #include "asb.h"
    #include <Canbus.h>
    #include <defaults.h>
    #include <global.h>
    #include <mcp2515.h>
    #include <mcp2515_defs.h>

    /**
     * CAN Communication Interface
     * @see ASB_COMM
     */
    class ASB_CAN2 : public ASB_COMM {
        private:
            /**
             * CAN Bus Speed
             */
            byte _speed = 0;

        public:
            /**
             * Constructor
             * @param speed CAN bus speed definition
             */
            ASB_CAN2(unsigned char speed);

            /**
             * Initialize CAN controller
             * @return byte error code
             * @see https://github.com/Seeed-Studio/CAN_BUS_Shield/blob/master/mcp_can_dfs.h
             */
            byte begin(void);

            /**
             * Parse CAN-address into our metadata format
             * @param canAddr CAN-address
             * @return asbMeta object containing decoded metadata, targst/source==0x00 on errors
             */
            asbMeta asbCanAddrParse(unsigned long canAddr);

            /**
             * Assemble a CAN-address based on our adressing format
             * @param meta asbMeta object
             * @return unsigned long CAN-address
             */
            unsigned long asbCanAddrAssemble(asbMeta meta);

            /**
             * Assemble a CAN-address based on our adressing format
             * @param type 2 bit message type (ASB_PKGTYPE_*)
             * @param target address between 0x0001 and 0x07FF/0xFFFF
             * @param source source address between 0x0001 and 0x07FF
             * @return unsigned long CAN-address
             */
            unsigned long asbCanAddrAssemble(byte type, unsigned int target, unsigned int source);
            
            /**
             * Assemble a CAN-address based on our adressing format
             * @param type 2 bit message type (ASB_PKGTYPE_*)
             * @param target address between 0x0001 and 0x07FF/0xFFFF
             * @param source source address between 0x0001 and 0x07FF
             * @param port port address between 0x00 and 0x1F, Unicast only
             * @return unsigned long CAN-address
             */
            unsigned long asbCanAddrAssemble(byte type, unsigned int target, unsigned int source, char port);

            /**
             * Send message to CAN-bus
             * @param type 2 bit message type (ASB_PKGTYPE_*)
             * @param target address between 0x0001 and 0x07FF/0xFFFF
             * @param source source address between 0x0001 and 0x07FF
             * @param port port address between 0x00 and 0x1F, Unicast only
             * @param len number of bytes to send (0-8)
             * @param data array of bytes to send
             */
            bool asbSend(byte type, unsigned int target, unsigned int source, char port, byte len, const byte *data);

            /**
             * Receive a message from the CAN-bus
             *
             * This polls the CAN-Controller for new messages.
             * The received message will be passed to &pkg, if no message
             * is available the function will return false.
             *
             * @param pkg asbPacket-Reference to store received packet
             * @return true if a message was received
             */
            bool asbReceive(asbPacket &pkg);
    };

#endif /* ASB_CAN2__H */