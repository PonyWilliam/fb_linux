
LDFLAGS = -L. -liconv -lfont -lpthread 
CPPFLAGS = -I../include 


client:audio_client.c 
	arm-linux-gcc audio_client.c lcd.c ts.c utf8_gb2312.c libbase64.o -o client $(LDFLAGS)

clean:
	rm client