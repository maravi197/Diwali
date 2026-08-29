CXX = g++
CXXFLAGS = -O2 -Wall
LDLIBS = -lncurses
TARGET = diwali

$(TARGET): diwali.cpp
	$(CXX) $(CXXFLAGS) diwali.cpp -o $(TARGET) $(LDLIBS)

install: $(TARGET)
	sudo cp $(TARGET) /usr/local/bin/$(TARGET)

clean:
	rm -f $(TARGET)
