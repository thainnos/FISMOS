if __name__ == "__main__":
    import socket
    import time
    import board
    import adafruit_dht

    TCP_IP = '127.0.0.1'
    TCP_PORT = 5005
    BUFFER_SIZE = 1024

    mydht22 = adafruit_dht.DHT22(board.d6)
    temperature = mydht22.temperature

    f_data_buf = open("/dev/fismos_data_buffer", "w")
    f_data_buf.write(temperature)
    f_data_buf.close

    f_instr_reg = open("/dev/fismos_instruction_register", "rb")
    print("Value of fismos_instruction_registeris: ", f_instr_reg.read())
    f_instr_reg.close


    f_instr_reg = open("/dev/fismos_instruction_register", "w")
    f_instr_reg.write("df000003")
    f_instr_reg.close

    time.sleep(2.0)

    f_data_buf = open("/dev/fismos_data_buffer", "rb")
    print("Value of fismos_data_buffer: ", f_data_buf.read())
    signature = f_data_buf.read()
    f_data_buf.close

    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.connect((TCP_IP, TCP_PORT))
    s.send(temperature + signature)
    data = s.recv(BUFFER_SIZE)
    s.close()
    