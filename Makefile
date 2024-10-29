TARGET = stegobmp
# flags
CC = gcc
CFLAGS = -Wall -Wextra -O2
LDFLAGS = -lssl -lcrypto  # Librerías de OpenSSL para cifrado

# Carpetas
SRC_DIR = src
OBJ_DIR = obj
INC_DIR = include
SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

all: $(TARGET) compile_commands.json

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -I $(INC_DIR) -o $@ $^ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -I $(INC_DIR) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

compile_commands.json: Makefile
	@echo "[" > $@
	@for src in $(SRCS); do \
		echo "  {" >> $@ ; \
		echo "    \"directory\": \"$$PWD\"," >> $@ ; \
		echo "    \"command\": \"$(CC) $(CFLAGS) -I$(INC_DIR) $$src -o $$src.o\"," >> $@ ; \
		echo "    \"file\": \"$$PWD/$$src\"" >> $@ ; \
		if [ "$$src" != "$$(echo $(SRCS) | awk '{print $$NF}')" ]; then \
			echo "  }," >> $@ ; \
		else \
			echo "  }" >> $@ ; \
		fi \
	done
	@echo "]" >> $@

clean:
	rm -rf $(OBJ_DIR) $(TARGET) tests compile_commands.json

.PHONY: all clean
