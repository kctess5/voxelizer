# CUDA C mesh voxelizer

This is a simple voxelization engine. Given a .obj mesh, this will output a voxel grid in the form of another .obj file. This voxelizer relies on CUDA for acceleration of the inherently parallel voxelization process.

WARNING: this can generate some BIG files and make your computer very sad. Keep the resolution below around 128 for safety. Higher can be achieved, but a more efficient storage method would probably be wise first.

Using resolution of 256 I generated a 4.7 GB voxel file containing over 67 million vertices. This just about crashed my computer as I obliterated my poor swap space. On the bright side, it only took 2.8 seconds to generate...

This voxelizer is built on:

- [Möller–Trumbore intersection algorithm](https://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm)
- CUDA

### Usage:

./voxelizer [options] [input path] [ouput path]

Options: 

    -s, --samples     : number of sample rays per vertex    

    -v, --verbose     : Verbosity level. Multiple flags for more verbosity.    

    -z, --depth       : voxelization depth    

    -y, --height      : voxelization height    

    -x, --width       : voxelization width    

    -r, --resolution  : voxelization resolution    

    -h, --help        : Displays usage information and exits.

Arguments:

   [input path] : path to .obj mesh

   [ouput path] : path to save voxel grid

### Build instructions:

You will need NVIDIA CUDA for this to compile properly.

```
mkdir build
cd build
cmake ..
make
```

### References

- This was very useful for implementing the GPU ray-triangle intersection [Möller–Trumbore intersection algorithm](https://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm)
- Last semester's graphics assignment in which I implemented ray-triangle intersection
- The CUDA documentation
- The given skeleton code

### Known Problems

There are no known problems with the code. It gets a bit squirly if given very large resolutions, but that's not a surprise.

### Extra Credit

I implemented the voxelization algorithm on the GPU with CUDA. This gives a massive speedup - on the order of 300 times faster than the sample executable.

Additionally, I implemented multiple sampling for incomplete blocks. Using the -s or --samples argument allows you to specify how many random directions are tested.