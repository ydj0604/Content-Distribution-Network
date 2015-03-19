CC=g++
CC_FLAGS=-std=c++11 -Wall -Wextra -Wno-unused-parameter -Wno-sign-compare
LD_FLAGS=-I$(HOME)/casablanca/Release/include/ -L$(HOME)/casablanca/Release/build.release/Binaries -lcpprest -lboost_system
LD_CRYPTO_FLAGS=-lcryptopp -lpthread
OUTDIR=build

all: client fss meta cdn

CLIENT_SRC=$(wildcard Client/*.cpp) $(wildcard ipToLatLng/*.cpp)
CLIENT_OBJ=$(CLIENT_SRC:.cpp=.o)
client: $(CLIENT_OBJ)
	mkdir -p $(OUTDIR)
	$(CC) $(CC_FLAGS) $(CLIENT_OBJ) -o $(OUTDIR)/client $(LD_FLAGS) $(LD_CRYPTO_FLAGS)

FSS_SRC=$(wildcard FSS/*.cpp) $(wildcard ipToLatLng/*.cpp)
FSS_OBJ=$(FSS_SRC:.cpp=.o)
fss: $(FSS_OBJ)
	mkdir -p $(OUTDIR)
	$(CC) $(CC_FLAGS) $(FSS_OBJ) -o $(OUTDIR)/fss $(LD_FLAGS)

META_SRC=$(wildcard Meta/*.cpp) $(wildcard Origin/*.cpp)
META_OBJ=$(META_SRC:.cpp=.o)
meta: $(META_OBJ)
	mkdir -p $(OUTDIR)
	$(CC) $(CC_FLAGS) $(META_OBJ) -o $(OUTDIR)/meta $(LD_FLAGS)

CDN_SRC=$(wildcard CDN/*.cpp) $(wildcard ipToLatLng/*.cpp)
CDN_OBJ=$(CDN_SRC:.cpp=.o)
cdn: $(CDN_OBJ)
	mkdir -p $(OUTDIR)
	$(CC) $(CC_FLAGS) $(CDN_OBJ) -o $(OUTDIR)/cdn $(LD_FLAGS)

%.o: %.cpp
	$(CC) -c $(CC_FLAGS) $< -o $@ $(LD_FLAGS)

clean:
	rm -f build/client build/fss build/cdn build/meta $(CLIENT_OBJ) $(FSS_OBJ) $(META_OBJ) $(CDN_OBJ)

# DONT FORGET TO
# export LD_LIBRARY_PATH=$(HOME)/casablanca//Release/build.release/Binaries/
