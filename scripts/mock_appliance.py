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
    res = bytearray()
    for b in data:
        if (b & 0xFC) == 0xE0: # E0, E1, E2, E3
            res.append(ESC)
            res.append(b)
        else:
            res.append(b)
    return bytes(res)

def unescape_data(data: bytes) -> bytes:
    res = bytearray()
    escape_next = False
    for b in data:
        if escape_next:
            res.append(b)
            escape_next = False
        elif b == 0xE0:
            escape_next = True
        else:
            res.append(b)
    return bytes(res)

def send_packet(ser, dest, src, payload):
    length = 7 + len(payload)
    header = struct.pack('BBB', dest, length, src)
    
    crc_data = header + payload
    crc = crc16_block(crc_data)
    crc_bytes = struct.pack('>H', crc)
    
    packet_inner = header + payload + crc_bytes
    packet_inner_escaped = escape_data(packet_inner)
    
    packet = bytes([STX]) + packet_inner_escaped + bytes([ETX])
    print(f"Sending: {packet.hex()}")
    ser.write(packet)

def handle_packet(ser, packet):
    packet_inner = unescape_data(packet[1:-1])
    if len(packet_inner) < 5:
        return
        
    dest = packet_inner[0]
    length = packet_inner[1]
    src = packet_inner[2]
    payload = packet_inner[3:-2]
    crc_bytes = packet_inner[-2:]
    
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
        last_pub_time = time.time()
        pub_req_id = 0
        
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
                
                # Periodically publish mock ERDs every 5 seconds
                if time.time() - last_pub_time > 5.0:
                    last_pub_time = time.time()
                    pub_req_id = (pub_req_id + 1) % 256
                    
                    # Dummy ERD Data
                    # ERD 0x2000 (Oven Temp, e.g. 350F -> 0x015E)
                    erd = 0x2000
                    erd_bytes = struct.pack('>H', erd)
                    mock_data = struct.pack('>H', 350)
                    data_len = len(mock_data)
                    
                    print(f"Publishing mock ERD 0x{erd:04X} = {mock_data.hex()}")
                    
                    # Publication Header: CMD(A6), Context(00), ReqID, Count(1)
                    header = bytes([CMD_PUB, 0x00, pub_req_id, 0x01])
                    payload = header + erd_bytes + bytes([data_len]) + mock_data
                    
                    # Dest=E4 (ESP32), Src=C0 (Appliance)
                    send_packet(ser, 0xE4, 0xC0, payload)
                    
        except KeyboardInterrupt:
            print("\nExiting")

if __name__ == '__main__':
    main()
