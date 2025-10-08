import struct

# Convert the official MNIST image file to input.dat
input_path = "MNIST_data/train-images.idx3-ubyte"
output_path = "MNIST_data/input.dat"

with open(input_path, "rb") as f:
    magic, num_images, rows, cols = struct.unpack(">IIII", f.read(16))
    data = f.read()

with open(output_path, "wb") as f_out:
    # Write same header + image data in big endian
    f_out.write(struct.pack(">IIII", magic, num_images, rows, cols))
    f_out.write(data)

print(f"✅ Created {output_path} with {num_images} images of size {rows}x{cols}")
