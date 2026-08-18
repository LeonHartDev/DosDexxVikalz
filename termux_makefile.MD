CC = clang++
CFLAGS = -O3 -march=native -std=c++17
LIBS = -lcurl -lncurses -lpthread

dos: dos.cpp
	$(CC) $(CFLAGS) -o dos dos.cpp $(LIBS)

clean:
	rm -f dos

install-deps:
	pkg update && pkg install -y libcurl ncurses-dev clang make
