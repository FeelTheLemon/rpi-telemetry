OBJDIR=obj
OBJS=common/graphics.o common/camera.o common/cameracontrol.o common/gfx.o \
	 common/text.o common/draw.o \
	 common/i2c/i2c.o common/i2c/mpu6050.o common/i2c/hmc5983.o \
	 common/uart/uart.o common/uart/gps.o \
	 main.o overlay.o data.o
BIN=build/telemetry

CFLAGS+=-std=c++11

LDFLAGS+=-L/opt/vc/lib/ -lbrcmGLESv2 -lbrcmEGL -lbcm_host -lvcos -lvchiq_arm -lpthread -lrt -lm -lmmal_core -lmmal_util -lmmal_vc_client -lfreetype -lbcm2835

INCLUDES+=-I/opt/vc/include/ -I/opt/vc/include/interface/vcos/pthreads -I/opt/vc/include/interface/vmcs_host/linux -Idependencies/freetype/include/ 

all: $(BIN) $(LIB)

%.o: %.c
	@rm -f $@ 
	$(CC) $(CFLAGS) $(INCLUDES) -g -c $< -o $@ 

%.o: %.cpp
	@rm -f $@ 
	$(CXX) $(CFLAGS) $(INCLUDES) -g -c $< -o $@ 

$(BIN): $(OBJS)
	$(CXX) -o $@ $(OBJS) $(LDFLAGS)

%.a: $(OBJS)
	$(AR) r $@ $^

clean:
	for i in $(OBJS); do (if test -e "$$i"; then ( rm $$i ); fi ); done
	@rm -f $(BIN) $(LIB)

run: all
	sudo ./$(BIN)
