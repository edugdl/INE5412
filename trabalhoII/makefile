# Makefile para compilação no Windows com MinGW

# Comando do compilador
CXX = g++

# Opções do compilador
CXXFLAGS = -std=c++11 -Wall

# Diretório dos arquivos de origem (.cpp)
SRC_DIR = src

# Diretório de saída dos arquivos objetos (.o)
OBJ_DIR = obj

# Diretório de saída do arquivo executável (.exe)
BIN_DIR = bin

# Nome do executável
TARGET = simulador

# Lista de arquivos de origem (.cpp)
SOURCES = $(wildcard $(SRC_DIR)/*.cpp)

# Lista de arquivos objetos (.o)
OBJECTS = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SOURCES))

# Cria o diretório de objetos e binários se eles não existirem
$(shell mkdir -p $(OBJ_DIR))

$(shell mkdir -p $(BIN_DIR))

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(BIN_DIR)/$@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJ_DIR)/*.o $(BIN_DIR)/$(TARGET)
	rmdir $(OBJ_DIR) $(BIN_DIR)