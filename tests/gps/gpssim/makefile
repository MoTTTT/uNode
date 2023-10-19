test.bin : test.hex

test.hex : test.abs

test.abs : test.obj
	bl51 test.obj,node.lib to test.abs ${LFLAGS} XDATA(4200H)

test.obj : test.c;

