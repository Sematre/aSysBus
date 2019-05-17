/**
  aSysBus CAN interface

  @copyright 2019 Sematre

  This interface depends on the SparkFun_CAN-Bus_Arduino_Library library fork.
    Download: https://github.com/Sematre/CAN-Bus-Library
*/

#ifndef ASB_CAN2__C
#define ASB_CAN2__C
    #include "asb_can2.h"
    #include "../../utils/asb_proto.h"
    #include <Canbus.h>
    #include <defaults.h>
    #include <global.h>
    #include <mcp2515.h>
    #include <mcp2515_defs.h>
    #include <SPI.h>

    ASB_CAN2::ASB_CAN2(unsigned char speed) {
        _speed = speed;
    }

    byte ASB_CAN2::begin() {
        lastErr = Canbus.init(_speed) ? 0 : 1;
        return lastErr;
    }

    asbMeta ASB_CAN2::asbCanAddrParse(unsigned long canAddr) {
        asbMeta temp;

        temp.target = 0;
        temp.source = 0;
        temp.port = -1;

        temp.type = ((canAddr >> 28) & 0x03);
        temp.source = (canAddr & 0x7FF);
        temp.target = ((canAddr >> 11) & 0xFFFF);

        if (temp.type == 0x03) { // Unicast
            temp.port = ((canAddr >> 23) & 0x1F);
            temp.target &= 0x7FF;
        }

        return temp;
    }

    unsigned long ASB_CAN2::asbCanAddrAssemble(asbMeta meta) {
        return asbCanAddrAssemble(meta.type, meta.target, meta.source, meta.port);
    }

    unsigned long ASB_CAN2::asbCanAddrAssemble(byte type, unsigned int target, unsigned int source) {
        return asbCanAddrAssemble(type, target, source, -1);
    }

    unsigned long ASB_CAN2::asbCanAddrAssemble(byte type, unsigned int target, unsigned int source, char port) {
          unsigned long addr = 0x80000000;

          if (type > 0x03) return 0;
          addr |= ((unsigned long) type << 28);

          if (type == ASB_PKGTYPE_UNICAST) {
            if (target > 0x7FF) return 0;
            if (port < 0 || port > 0x1F) return 0;

            addr |= ((unsigned long) port << 23);
          } else {
            if (target > 0xFFFF) return 0;
          }

          addr |= ((unsigned long) target << 11);

          if (source > 0x7FF) return 0;
          addr |= source;

          return addr;
    }

    bool ASB_CAN2::asbSend(byte type, unsigned int target, unsigned int source, char port, byte len, const byte *data) {
        unsigned long addr = asbCanAddrAssemble(type, target, source, port);
        if (addr == 0) return false;

        tCAN message;
        message.id = addr;
        message.extended = true;
        message.header.rtr = 0;
        message.header.length = len;
        for (byte i = 0; i < len; i++) message.data[i] = data[i];

        mcp2515_bit_modify(CANCTRL, (1<<REQOP2)|(1<<REQOP1)|(1<<REQOP0), 0);
        mcp2515_send_message(&message);

        lastErr = 0;
        return true;
    }

    bool ASB_CAN2::asbReceive(asbPacket &pkg) {
        tCAN message;
        if (!mcp2515_check_message()) return false;
        if (!mcp2515_get_message(&message)) return false;
        if (!message.extended) return false;

        pkg.meta = asbCanAddrParse(message.id);
        pkg.len = message.header.length;

        for(byte i = 0; i < message.header.length; i++) pkg.data[i] = message.data[i];
        return true;
    }


#endif /* ASB_CAN2__C */