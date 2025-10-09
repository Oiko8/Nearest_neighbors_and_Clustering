import struct

# Convert the official MNIST image file to input.dat
def convert_mist_dataset(input_path, output_path):
    
    with open(input_path, "rb") as f:
        magic, num_images, rows, cols = struct.unpack(">IIII", f.read(16))
        data = f.read()

    with open(output_path, "wb") as f_out:
        # Write same header + image data in big endian
        f_out.write(struct.pack(">IIII", magic, num_images, rows, cols))
        f_out.write(data)

    print(f"Created {output_path} with {num_images} images of size {rows}x{cols}")


def main():
    input_train_path = "MNIST_data/train-images.idx3-ubyte"
    output_train_path = "MNIST_data/input.dat"

    input_query_path = "MNIST_data/t10k-images.idx3-ubyte"
    output_query_path = "MNIST_data/query.dat"
    convert_mist_dataset(input_path=input_query_path, output_path=output_query_path)

if __name__ == "__main__" :
    main()