# Detect host environment
UNAME := $(shell uname)
ISMINGW = $(shell uname | grep -E -c "MINGW32")

# If the AR environment variable is not set, simply set it to ar
ifeq ($(AR),)
	AR = ar
endif

# If the CXX environment variable is not set, simply set it to g++
ifeq ($(CXX),)
	CXX = g++
endif

# We can use the BUILD_DIR environment variable to set the location of the output files
ifeq ($(BUILD_DIR),)
	BUILD_DIR = ./build
endif

# We can use the PREFIX environment variable to control the installation directory
ifeq ($(PREFIX),)
	PREFIX = /usr/local
endif

# EFC is split into a resusable library and several driver tools
# These flags allow the tools to link against the library without installation
TOOL_CXX_FLAGS = -I$(BUILD_DIR)/include
TOOL_LD_FLAGS = -L$(BUILD_DIR)/lib -lefc -lcryptopp -lsimple-base -lz

# Under MinGW, we want to use GCC and statically link with the standard libraries
EXE_EXT =
CXXFLAGS += -Wall -g
CREATELIB = $(AR) rcs $(BUILD_DIR)/lib/libefc.a $(LIB_OBJECT_FILES)
ifeq ($(ISMINGW),1)
	CXX = g++
	EXE_EXT = .exe
	LDFLAGS += -static-libgcc -static-libstdc++
endif

# Under OSX, we use clang++ as the compiler and ensure we link against libstdc++
ifeq ($(UNAME), Darwin)
	CXX = clang++
	CXXFLAGS += -std=c++11 -stdlib=libc++
	LDFLAGS += -lstdc++
	CREATELIB = libtool -static -o $(BUILD_DIR)/lib/libefc.a $(LIB_OBJECT_FILES)
endif

# Object files in libefc
LIB_OBJECT_FILES = $(BUILD_DIR)/obj/CompressionFactory.o $(BUILD_DIR)/obj/CompressionStrategy.o $(BUILD_DIR)/obj/NoCompression.o $(BUILD_DIR)/obj/ZlibCompression.o $(BUILD_DIR)/obj/ZlibCompressor.o $(BUILD_DIR)/obj/ZlibDecompressor.o $(BUILD_DIR)/obj/EFCDefaultHeader.o $(BUILD_DIR)/obj/EFCHeader.o $(BUILD_DIR)/obj/EFCHeaderFactory.o $(BUILD_DIR)/obj/AESDecrypter.o $(BUILD_DIR)/obj/AESEncrypter.o $(BUILD_DIR)/obj/AESEncryption.o $(BUILD_DIR)/obj/EncryptionFactory.o $(BUILD_DIR)/obj/EncryptionStrategy.o $(BUILD_DIR)/obj/ApplicationConfig.o $(BUILD_DIR)/obj/ChecksumUtility.o $(BUILD_DIR)/obj/MeteredIfstream.o $(BUILD_DIR)/obj/MeteredOfstream.o

all: dirs $(BUILD_DIR)/bin/efcencode$(EXE_EXT) $(BUILD_DIR)/bin/efcdecode$(EXE_EXT) $(BUILD_DIR)/bin/efcinfo$(EXE_EXT)
	@echo Done!

# Driver tools
$(BUILD_DIR)/bin/efcencode$(EXE_EXT): $(BUILD_DIR)/obj/efcencode.o $(BUILD_DIR)/lib/libefc.a
	$(CXX) -o $@ $(BUILD_DIR)/obj/efcencode.o $(CXXFLAGS) $(TOOL_LD_FLAGS) $(LDFLAGS)

$(BUILD_DIR)/bin/efcdecode$(EXE_EXT): $(BUILD_DIR)/obj/efcdecode.o $(BUILD_DIR)/lib/libefc.a
	$(CXX) -o $@ $(BUILD_DIR)/obj/efcdecode.o $(CXXFLAGS) $(TOOL_LD_FLAGS) $(LDFLAGS)

$(BUILD_DIR)/bin/efcinfo$(EXE_EXT): $(BUILD_DIR)/obj/efcinfo.o $(BUILD_DIR)/lib/libefc.a
	$(CXX) -o $@ $(BUILD_DIR)/obj/efcinfo.o $(CXXFLAGS) $(TOOL_LD_FLAGS) $(LDFLAGS)

$(BUILD_DIR)/obj/efcencode.o: ./source/efcencode.cpp $(HEADER_FILES)
	$(CXX) -c $< -o $@ $(TOOL_CXX_FLAGS) $(CXXFLAGS)

$(BUILD_DIR)/obj/efcdecode.o: ./source/efcdecode.cpp $(HEADER_FILES)
	$(CXX) -c $< -o $@ $(TOOL_CXX_FLAGS) $(CXXFLAGS)

$(BUILD_DIR)/obj/efcinfo.o: ./source/efcinfo.cpp $(HEADER_FILES)
	$(CXX) -c $< -o $@ $(TOOL_CXX_FLAGS) $(CXXFLAGS)


# libefc
$(BUILD_DIR)/lib/libefc.a: $(LIB_OBJECT_FILES)
	$(CREATELIB)

$(BUILD_DIR)/obj/CompressionFactory.o: ./source/compression/CompressionFactory.cpp ./source/compression/CompressionFactory.h ./source/compression/CompressionStrategy.h ./source/compression/NoCompression.h ./source/compression/ZlibCompressor.h ./source/compression/ZlibCompression.h ./source/compression/ZlibDecompressor.h
	$(CXX) -c $< -o $@ $(CXXFLAGS)

$(BUILD_DIR)/obj/CompressionStrategy.o: ./source/compression/CompressionStrategy.cpp ./source/compression/CompressionStrategy.h
	$(CXX) -c $< -o $@ $(CXXFLAGS)

$(BUILD_DIR)/obj/NoCompression.o: ./source/compression/NoCompression.cpp ./source/compression/NoCompression.h ./source/compression/CompressionStrategy.h
	$(CXX) -c $< -o $@ $(CXXFLAGS)

$(BUILD_DIR)/obj/ZlibCompression.o: ./source/compression/ZlibCompression.cpp ./source/compression/ZlibCompression.h ./source/compression/CompressionStrategy.h
	$(CXX) -c $< -o $@ $(CXXFLAGS)

$(BUILD_DIR)/obj/ZlibCompressor.o: ./source/compression/ZlibCompressor.cpp ./source/compression/ZlibCompressor.h ./source/compression/ZlibCompression.h
	$(CXX) -c $< -o $@ $(CXXFLAGS)

$(BUILD_DIR)/obj/ZlibDecompressor.o: ./source/compression/ZlibDecompressor.cpp ./source/compression/ZlibDecompressor.h ./source/compression/ZlibCompression.h
	$(CXX) -c $< -o $@ $(CXXFLAGS)

$(BUILD_DIR)/obj/EFCDefaultHeader.o: ./source/efc/EFCDefaultHeader.cpp ./source/efc/EFCDefaultHeader.h ./source/efc/EFCHeader.h ./source/utility/MeteredFilestream.h ./source/utility/MeteredIfstream.h ./source/utility/MeteredOfstream.h ./source/compression/CompressionFactory.h ./source/compression/CompressionStrategy.h ./source/encryption/EncryptionFactory.h ./source/encryption/EncryptionStrategy.h ./source/compression/CompressionStrategy.h ./source/utility/MeteredFilestream.h
	$(CXX) -c $< -o $@ $(CXXFLAGS)

$(BUILD_DIR)/obj/EFCHeader.o: ./source/efc/EFCHeader.cpp ./source/efc/EFCHeader.h ./source/utility/MeteredFilestream.h ./source/utility/MeteredIfstream.h ./source/utility/MeteredOfstream.h ./source/compression/CompressionFactory.h ./source/compression/CompressionStrategy.h ./source/encryption/EncryptionFactory.h ./source/encryption/EncryptionStrategy.h
	$(CXX) -c $< -o $@ $(CXXFLAGS)

$(BUILD_DIR)/obj/EFCHeaderFactory.o: ./source/efc/EFCHeaderFactory.cpp ./source/efc/EFCHeaderFactory.h ./source/efc/EFCHeader.h ./source/utility/MeteredFilestream.h ./source/utility/MeteredIfstream.h ./source/utility/MeteredOfstream.h ./source/compression/CompressionFactory.h ./source/compression/CompressionStrategy.h ./source/encryption/EncryptionFactory.h ./source/encryption/EncryptionStrategy.h ./source/compression/CompressionStrategy.h ./source/efc/EFCDefaultHeader.h
	$(CXX) -c $< -o $@ $(CXXFLAGS)

$(BUILD_DIR)/obj/AESDecrypter.o: ./source/encryption/AESDecrypter.cpp ./source/encryption/AESDecrypter.h ./source/encryption/AESEncryption.h ./source/encryption/EncryptionStrategy.h ./source/compression/CompressionStrategy.h ./source/utility/MeteredFilestream.h
	$(CXX) -c $< -o $@ $(CXXFLAGS)

$(BUILD_DIR)/obj/AESEncrypter.o: ./source/encryption/AESEncrypter.cpp ./source/encryption/AESEncrypter.h ./source/encryption/AESEncryption.h ./source/encryption/EncryptionStrategy.h ./source/compression/CompressionStrategy.h ./source/utility/MeteredFilestream.h
	$(CXX) -c $< -o $@ $(CXXFLAGS)

$(BUILD_DIR)/obj/AESEncryption.o: ./source/encryption/AESEncryption.cpp ./source/encryption/AESEncryption.h ./source/encryption/EncryptionStrategy.h ./source/compression/CompressionStrategy.h ./source/utility/MeteredFilestream.h
	$(CXX) -c $< -o $@ $(CXXFLAGS)

$(BUILD_DIR)/obj/EncryptionFactory.o: ./source/encryption/EncryptionFactory.cpp ./source/encryption/EncryptionFactory.h ./source/encryption/EncryptionStrategy.h ./source/compression/CompressionStrategy.h ./source/utility/MeteredFilestream.h ./source/encryption/AESEncrypter.h ./source/encryption/AESEncryption.h ./source/encryption/AESDecrypter.h
	$(CXX) -c $< -o $@ $(CXXFLAGS)

$(BUILD_DIR)/obj/EncryptionStrategy.o: ./source/encryption/EncryptionStrategy.cpp ./source/encryption/EncryptionStrategy.h ./source/compression/CompressionStrategy.h ./source/utility/MeteredFilestream.h ./source/utility/MeteredIfstream.h ./source/utility/MeteredOfstream.h
	$(CXX) -c $< -o $@ $(CXXFLAGS)

$(BUILD_DIR)/obj/ApplicationConfig.o: ./source/utility/ApplicationConfig.cpp ./source/utility/ApplicationConfig.h ./source/compression/CompressionFactory.h ./source/compression/CompressionStrategy.h ./source/encryption/EncryptionFactory.h ./source/encryption/EncryptionStrategy.h ./source/compression/CompressionStrategy.h ./source/efc/EFCHeaderFactory.h ./source/efc/EFCHeader.h ./source/utility/MeteredFilestream.h ./source/utility/MeteredIfstream.h ./source/utility/MeteredOfstream.h
	$(CXX) -c $< -o $@ $(CXXFLAGS)

$(BUILD_DIR)/obj/ChecksumUtility.o: ./source/utility/ChecksumUtility.cpp ./source/utility/ChecksumUtility.h ./source/utility/MeteredFilestream.h ./source/utility/MeteredIfstream.h ./source/utility/MeteredOfstream.h
	$(CXX) -c $< -o $@ $(CXXFLAGS)

$(BUILD_DIR)/obj/MeteredIfstream.o: ./source/utility/MeteredIfstream.cpp ./source/utility/MeteredIfstream.h
	$(CXX) -c $< -o $@ $(CXXFLAGS)

$(BUILD_DIR)/obj/MeteredOfstream.o: ./source/utility/MeteredOfstream.cpp ./source/utility/MeteredOfstream.h
	$(CXX) -c $< -o $@ $(CXXFLAGS)


dirs:
	@test -d $(BUILD_DIR) || mkdir $(BUILD_DIR)
	@test -d $(BUILD_DIR)/obj || mkdir $(BUILD_DIR)/obj
	@test -d $(BUILD_DIR)/bin || mkdir $(BUILD_DIR)/bin
	@test -d $(BUILD_DIR)/lib || mkdir $(BUILD_DIR)/lib

install_dirs:
	@test -d $(PREFIX) || mkdir $(PREFIX)
	@test -d $(PREFIX)/bin || mkdir $(PREFIX)/bin

install: install_dirs
	cp -r $(BUILD_DIR)/bin/* $(PREFIX)/bin/
	chmod 777 $(PREFIX)/bin/efcencode$(EXE_EXT)
	chmod 777 $(PREFIX)/bin/efcdecode$(EXE_EXT)
	chmod 777 $(PREFIX)/bin/efcinfo$(EXE_EXT)

clean:
	rm $(BUILD_DIR)/obj/*.o
