# Example to access FISMOS with Python3

def raw(string: str, replace: bool = False) -> str:
    r = repr(string)[1:-1]  # Strip the quotes from representation
    if replace:
        r = r.replace('\\\\', '\\')
    return r

if __name__ == "__main__":
    f_instr_reg = open("/dev/fismos_instruction_register", "rb")
    print("Value of fismos_instruction_registeris: ", f_instr_reg.read())
    # print(repr(open(f_instr_reg, 'rb').read()))
    f_instr_reg.close


    f_instr_reg = open("/dev/fismos_instruction_register", "w")
    f_instr_reg.write("12345678")
    f_instr_reg.close


    f_instr_reg = open("/dev/fismos_instruction_register", "rb")
    print("Value of fismos_instruction_registeris: ", f_instr_reg.read())
    f_instr_reg.close


    f_data_buf = open("/dev/fismos_data_buffer", "rb")
    print("Value of fismos_data_buffer: ", f_data_buf.read())
    f_data_buf.close