all: test
CXX = g++-4.9
CFLAGS=-fPIC -g -Wall `pkg-config --cflags opencv`
#LUSB = -L/usr/lib/arm 
LIBS = `pkg-config --libs opencv`
#LIBS = -L/usr/local/lib/arm/ -lopencv_highgui -lopencv_core 
INCLUDE = -I/usr/local/include/libfreenect
FREE_LIBS = -L/usr/local/lib -lfreenect
#CUDA_LIBS = -L/usr/local/arm/cuda/lib -lcudart -lnpps -lnppi -lnppc -lcufft
UDP_LIB   = udpSocket.a
CUDA_LIBS = -L/usr/local/cuda/lib64 -lcufft

test:  test.cpp
	$(CXX) $(INCLUDE) $(CFLAGS) $? -o $@  $(UDP_LIB) $(LIBS) $(FREE_LIBS) $(CUDA_LIBS) $(LUSB)

test.o: test.cpp
	$(CXX) -c $(CFLAGS) $< -o $@

clean:
	rm -rf *.o test
