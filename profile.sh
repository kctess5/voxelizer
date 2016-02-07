echo "Size 64x64x64"
echo "---"

echo "Sphere"
echo "GPU algorithm:"
./build/bin/voxelizer -r 64 ./data/sphere/sphere.obj ./data/sphere/sphere_voxelized_gpu.obj | grep -E "seconds|Triangles"

echo "CPU algorithm:"
./sample/voxelizer ./data/sphere/sphere.obj ./data/sphere/sphere_voxelized_cpu.obj | grep -E "seconds"


echo "\nBunny"
echo "GPU algorithm:"
./build/bin/voxelizer -r 64 ./data/bunny/bunny.obj ./data/bunny/bunny_voxelized_gpu.obj | grep -E "seconds|Triangles"

echo "CPU algorithm:"
./sample/voxelizer ./data/bunny/bunny.obj ./data/bunny/bunny_voxelized_cpu.obj | grep -E "seconds"


echo "\nTeapot"
echo "GPU algorithm:"
./build/bin/voxelizer -r 64 ./data/teapot/teapot.obj ./data/teapot/teapot_voxelized_gpu.obj | grep -E "seconds|Triangles"

echo "CPU algorithm:"
./sample/voxelizer ./data/teapot/teapot.obj ./data/teapot/teapot_voxelized_cpu.obj | grep -E "seconds"