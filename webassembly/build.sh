clear
em++ \
-std=c++11 \
-O3 \
-s ALLOW_MEMORY_GROWTH=1 \
-o loop_subdivision.wasm \
-s EXPORTED_RUNTIME_METHODS="['cwrap']" \
-s WASM=1 \
-s ERROR_ON_UNDEFINED_SYMBOLS=0 \
wasm.cpp ../src/Mesh.cpp ../src/Edge.cpp ../src/LOOP.cpp ../src/FormTrait.cpp ../src/HalfEdge.cpp ../src/javascript.cpp ../src/Vertex.cpp  ../src/Face.cpp

# -s INITIAL_MEMORY=67108864 \
# -g2 \
