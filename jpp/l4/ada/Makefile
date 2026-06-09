FC := gnatmake
FFLAGS := -gnat2012 -Wall

SOURCES := main.adb philosopher.ads philosopher.adb
TARGET := main

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(FC) $(FFLAGS) -o $(TARGET) main.adb

clean:
	rm -f $(TARGET)
	rm -f *.o *.ali

.PHONY: all clean
