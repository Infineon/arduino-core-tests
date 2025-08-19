import serial
import sys

JOB_START_TOKEN = "[@START_TEST@]"
JOB_START_TOKEN_RESPONSE = JOB_START_TOKEN

def send_start_token(port_list, baudrate=115200, timeout=2):
    if not port_list:
        print("No ports provided.")
        return 1

    print("\n\nINFO: Sending job start tokens ...")
    return_code = 0

    for port in port_list:
        try:
            ser = serial.Serial(port, baudrate=baudrate, timeout=timeout)
            ser.reset_input_buffer()
            ser.write((JOB_START_TOKEN + "\n").encode('utf-8'))
            response = ser.readline().decode().strip()
            print(f"INFO: [{port}] response : {response}")
            count = 0

            while (JOB_START_TOKEN_RESPONSE not in response) and (count < 300):
                response = ser.readline().decode().strip()
                print(f"INFO: [{port}] response : {response}")
                count += 1

            if JOB_START_TOKEN_RESPONSE not in response:
                print(f"ERROR: Did not get proper response from job after sending job start token to port {port}. Expected was {JOB_START_TOKEN_RESPONSE}, received '{response}' !\n")
                return_code = 1
                ser.close()
                break
            ser.close()
        except Exception as e:
            print(f"ERROR: Exception for port {port}: {e}")
            return_code = 1
            break

    return return_code

if __name__ == "__main__":
    """
    Example usage:
        python sync_ports.py /dev/ttyUSB0 /dev/ttyUSB1
    This will send the JOB_START_TOKEN to each listed serial port and wait for the response.
    """
    ports = sys.argv[1:]
    if not ports:
        print("Usage: python sync_ports.py <port1> <port2> ...")
        sys.exit(1)
    ret = send_start_token(ports)
    sys.exit(ret)