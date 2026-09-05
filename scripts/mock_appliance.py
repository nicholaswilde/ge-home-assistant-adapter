#!/usr/bin/env python3
import serial
import time
import argparse
import struct

# GEA3 Protocol Constants
STX = 0xE2
ETX = 0xE3
ESC = 0xE0

# Commands
CMD_READ_REQ = 0xA0
CMD_READ_RSP = 0xA1
CMD_WRITE_REQ = 0xA2
CMD_WRITE_RSP = 0xA3
CMD_SUB_ALL_REQ = 0xA4
CMD_SUB_ALL_RSP = 0xA5
CMD_PUB = 0xA6
CMD_PUB_ACK = 0xA7
CMD_SUB_HOST_STARTUP = 0xA8

def crc16_byte(seed: int, byte: int) -> int:
    crc = seed
    byte = ((crc >> 8) ^ byte) & 0xFF
    byte ^= byte >> 4
    return ((crc << 8) ^ (byte << 12) ^ (byte << 5) ^ byte) & 0xFFFF

def crc16_block(data: bytes, seed: int = 0x1021) -> int:
    crc = seed
    for b in data:
        crc = crc16_byte(crc, b)
    return crc

def escape_data(data: bytes) -> bytes:
    # Not fully implementing escape logic for simple mock, 
    # but GEA3 escapes E0, E1, E2, E3 by prefixing E0 and XORing with some value
    # For now, just return data as is unless needed
    res = bytearray()
    for b in data:
        if (b & 0xFC) == 0xE0:
            res.append(ESC)
            res.append(b ^ 0x10) # rough guess for GEA esc XOR
        else:
            res.append(b)
    return bytes(res)

def send_packet(ser, dest, src, payload):
    # packet_without_crc = Dest + Length + Src + Payload
    # Wait, the format from the captured packet:
    # E2 C0 0A E4 A4 0A 00 80 73 E3
    # Dest = C0
    # Length = 0A (10 bytes total)
    # Src = E4
    # Payload = A4 0A 00
    # Length = 2 (STX, ETX) + 1 (Dest) + 1 (Length) + 1 (Src) + len(payload) + 2 (CRC) = 7 + len(payload)
    length = 7 + len(payload)
    
    # Header: Dest, Length, Src
    header = struct.pack('BBB', dest, length, src)
    
    crc_data = header + payload
    crc = crc16_block(crc_data)
    crc_bytes = struct.pack('>H', crc) # MSB first based on 80 73
    
    packet = bytes([STX]) + header + payload + crc_bytes + bytes([ETX])
    print(f"Sending: {packet.hex()}")
    ser.write(packet)

def handle_packet(ser, packet):
    if len(packet) < 7:
        return
        
    dest = packet[1]
    length = packet[2]
    src = packet[3]
    payload = packet[4:-3]
    crc_bytes = packet[-3:-1]
    
    print(f"Received from {hex(src)} to {hex(dest)}: Payload {payload.hex()}")
    
    if len(payload) > 0:
        cmd = payload[0]
        if cmd == CMD_SUB_ALL_REQ:
            req_id = payload[1]
            print(f"-> Subscribe All Request (Req ID: {hex(req_id)})")
            # Send Subscribe All Response (A5)
            # Result 00 = Success
            resp_payload = bytes([CMD_SUB_ALL_RSP, req_id, 0x00])
            send_packet(ser, src, dest, resp_payload)
            
        elif cmd == CMD_READ_REQ:
            if len(payload) >= 4:
                req_id = payload[1]
                erd = struct.unpack('>H', payload[2:4])[0]
                print(f"-> Read Request for ERD 0x{erd:04X} (Req ID: {hex(req_id)})")
                
                # Mock response (A1)
                # Command (A1), ReqID, Result(00=Success), ERD(2 bytes), Data Length(1 byte), Data
                erd_bytes = struct.pack('>H', erd)
                
                # Let's return 1 byte of 0x00 as a default dummy response
                mock_data = bytes([0x00])
                data_len = len(mock_data)
                
                resp_payload = bytes([CMD_READ_RSP, req_id, 0x00]) + erd_bytes + bytes([data_len]) + mock_data
                send_packet(ser, src, dest, resp_payload)
        
        elif cmd == CMD_WRITE_REQ:
            if len(payload) >= 4:
                req_id = payload[1]
                erd = struct.unpack('>H', payload[2:4])[0]
                print(f"-> Write Request for ERD 0x{erd:04X} (Req ID: {hex(req_id)})")
                # Command (A3), ReqID, Result(00=Success)
                resp_payload = bytes([CMD_WRITE_RSP, req_id, 0x00])
                send_packet(ser, src, dest, resp_payload)

def main():
    parser = argparse.ArgumentParser(description="Mock GEA3 Appliance")
    parser.add_argument("--port", default="/dev/ttyUSB0", help="Serial port")
    parser.add_argument("--baud", type=int, default=230400, help="Baud rate")
    args = parser.parse_args()
    
    print(f"Mock Appliance listening on {args.port} at {args.baud} baud...")
    
    with serial.Serial(args.port, args.baud, timeout=0.1) as ser:
        buffer = bytearray()
        try:
            while True:
                data = ser.read(100)
                if data:
                    buffer.extend(data)
                    
                    # Parse E2 ... E3 frames
                    while True:
                        if STX in buffer:
                            start_idx = buffer.index(STX)
                            buffer = buffer[start_idx:] # trim before STX
                            
                            if ETX in buffer:
                                end_idx = buffer.index(ETX)
                                packet = buffer[:end_idx+1]
                                buffer = buffer[end_idx+1:]
                                
                                handle_packet(ser, packet)
                            else:
                                break
                        else:
                            buffer.clear()
                            break
        except KeyboardInterrupt:
            print("\nExiting")

if __name__ == '__main__':
    main()
