import serial
import argparse

def main():
    parser = argparse.ArgumentParser(description="Test GEA3 serial connection on FTDI adapter")
    parser.add_argument("--port", default="/dev/ttyUSB0", help="Serial port (default: /dev/ttyUSB0)")
    parser.add_argument("--baud", type=int, default=230400, help="Baud rate (default: 230400)")
    args = parser.parse_args()

    print(f"Listening on {args.port} at {args.baud} baud... (Press Ctrl+C to stop)")
    try:
        with serial.Serial(args.port, args.baud, timeout=1) as ser:
            while True:
                data = ser.read(100)
                if data:
                    print(f"Received: {data.hex()}")
    except KeyboardInterrupt:
        print("\nExiting.")
    except Exception as e:
        print(f"Error: {e}")

if __name__ == '__main__':
    main()
