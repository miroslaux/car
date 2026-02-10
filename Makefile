# Variable Definitions
CC      = gcc
CFLAGS  = -Wall -O2
LIBS    = -lncurses
TARGET  = car
SRC     = car.c
PREFIX  = /usr/local/bin

# Default target: compile the program
all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LIBS)

# Install target: moves the binary to /usr/local/bin
# Requires sudo for permissions
install: $(TARGET)
	@echo "Installing $(TARGET) to $(PREFIX)..."
	sudo cp $(TARGET) $(PREFIX)/$(TARGET)
	sudo chmod 755 $(PREFIX)/$(TARGET)
	@echo "Installation complete. You can now run '$(TARGET)' from your terminal."

# Uninstall target: removes the binary from the system path
uninstall:
	@echo "Removing $(TARGET) from $(PREFIX)..."
	sudo rm -f $(PREFIX)/$(TARGET)
	@echo "Uninstalled."

# Clean target: removes local build files
clean:
	rm -f $(TARGET)

.PHONY: all install uninstall clean
