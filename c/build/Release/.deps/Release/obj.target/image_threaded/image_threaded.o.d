cmd_Release/obj.target/image_threaded/image_threaded.o := cc '-DNODE_GYP_MODULE_NAME=image_threaded' '-DUSING_UV_SHARED=1' '-DUSING_V8_SHARED=1' '-DV8_DEPRECATION_WARNINGS=1' '-D_LARGEFILE_SOURCE' '-D_FILE_OFFSET_BITS=64' -I/home/marty/.node-gyp/6.9.1/include/node -I/home/marty/.node-gyp/6.9.1/src -I/home/marty/.node-gyp/6.9.1/deps/uv/include -I/home/marty/.node-gyp/6.9.1/deps/v8/include  -fPIC -pthread -Wall -Wextra -Wno-unused-parameter -m64 -fopenmp -O3 -fno-omit-frame-pointer  -MMD -MF ./Release/.deps/Release/obj.target/image_threaded/image_threaded.o.d.raw   -c -o Release/obj.target/image_threaded/image_threaded.o ../image_threaded.c
Release/obj.target/image_threaded/image_threaded.o: ../image_threaded.c \
 ../image_threaded.h ../stb_image.h ../stb_image_write.h
../image_threaded.c:
../image_threaded.h:
../stb_image.h:
../stb_image_write.h:
