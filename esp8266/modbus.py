from pyModbusTCP.client import ModbusClient

host = "192.168.0.100"

def servo(address, value):
	c = ModbusClient(host=host, auto_open=True, auto_close=True)
	if c.write_multiple_registers(address, [value]):
	    print("write ok")
	else:
	    print("write error")

while True:
	num = 1
	value = int(input("[*] New value: "))
	servo(4+num, value)