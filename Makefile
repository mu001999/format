test: test.out
	./test.out

test.out: test.cpp
	g++ -W -std=c++2a -O2 test.cpp -lgtest -lpthread -o test.out

.PHONY: clean

clean:
	rm test.out
