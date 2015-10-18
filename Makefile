all: build
default: build

EXAMPLE = $(shell pwd)/examples/web

build:
	@mkdir -p build && \
		cd build && \
		cmake .. && \
		make

clean:
	@rm -rf build

stop:
	@ps | grep brent | awk 'NR==1{print $1}' | xargs kill -9

serve: build
	@echo "Serving example on localhost:8000"
	@./build/brent -d $(EXAMPLE)

.PHONY: build clean serve stop
