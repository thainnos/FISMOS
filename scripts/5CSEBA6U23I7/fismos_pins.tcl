# Pin Configuration

# Testing Output with LEDs
set_location_assignment  PIN_W15   -to out_byte[0]
set_location_assignment  PIN_AA24  -to out_byte[1]
set_location_assignment  PIN_V16   -to out_byte[2]
set_location_assignment  PIN_V15   -to out_byte[3]
set_location_assignment  PIN_AF26  -to out_byte[4]
set_location_assignment  PIN_AE26  -to out_byte[5]
set_location_assignment  PIN_Y16   -to out_byte[6]
set_location_assignment  PIN_AA23  -to out_byte[7]

set_location_assignment  PIN_Y13   -to clk
set_location_assignment  PIN_AH17  -to resetn

#set_location_assignment  PIN_AH17  -to PUSH[0]
set_location_assignment  PIN_AH16  -to PUSH[1]
set_location_assignment  PIN_Y24   -to SW[0]
set_location_assignment  PIN_W24   -to SW[1]
set_location_assignment  PIN_W21   -to SW[2]
set_location_assignment  PIN_W20   -to SW[3]
