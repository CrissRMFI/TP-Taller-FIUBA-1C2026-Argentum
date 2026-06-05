.PHONY: all test clean editor client common server build install uninstall

compile-debug:
	mkdir -p build/
	cmake -S . -B ./build -DCMAKE_BUILD_TYPE=Debug $(EXTRA_GENERATE)
	cmake --build  build/ $(EXTRA_COMPILE)

run-tests: compile-debug
	./build/taller_tests

all: clean run-tests

# Instalador: deps + build + tests + copia a ~/.local y ~/.config.
# Pasar args al script con ARGS, p.ej.: make install ARGS=--no-deps
install:
	./install.sh $(ARGS)

uninstall:
	./uninstall.sh $(ARGS)

clean:
	rm -Rf build/
