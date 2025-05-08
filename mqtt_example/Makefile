all: mqtt_test.c
	/home/alen/VisonFive2_SDK/VisionFive2/work/buildroot_rootfs/host/bin/riscv64-buildroot-linux-gnu-gcc -o mqtt_test mqtt_test.c  -I include/test/  -I include/common/ -I include/mqttclient  -I include/ -I include/common/log -I include/mqtt  -I ./include/platform/linux  -I ./include/network -L . -lmqttclient -lpthread

clean:
	rm mqtt_test