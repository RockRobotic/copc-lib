import os

with open("test_data.h", mode="w") as out_file:
    for x in ["first_20_pts", "next_12_pts", "last_60_pts"]:
        with open(os.path.join("las", x + ".laz"), mode="rb") as f:
            compressed_bytes = f.read()
            start_pt_offset = int.from_bytes(
                compressed_bytes[96:100], byteorder="little"
            )
            first_pt_offset = start_pt_offset + 8
            last_pt_offset = int.from_bytes(
                compressed_bytes[start_pt_offset:first_pt_offset], byteorder="little"
            )
            print(start_pt_offset, first_pt_offset, last_pt_offset)
            compressed_bytes = compressed_bytes[first_pt_offset:last_pt_offset]

            out_file.write(
                "unsigned char %s_compressed[%d] = { " % (x, len(compressed_bytes))
            )

            byte_str = compressed_bytes.hex(" ")
            byte_str = ", ".join(["0x" + b.upper() for b in byte_str.split(" ")])
            out_file.write(byte_str)
            out_file.write("};")
            out_file.write("\n\n")

        with open(os.path.join("las", x + ".las"), mode="rb") as f:
            uncompressed_bytes = f.read()
            first_pt_offset = int.from_bytes(
                uncompressed_bytes[96:100], byteorder="little"
            )
            print(first_pt_offset)
            uncompressed_bytes = uncompressed_bytes[first_pt_offset:]

            out_file.write("unsigned char %s[%d] = { " % (x, len(uncompressed_bytes)))

            byte_str = uncompressed_bytes.hex(" ")
            byte_str = ", ".join(["0x" + b.upper() for b in byte_str.split(" ")])
            out_file.write(byte_str)
            out_file.write("};")
            out_file.write("\n\n")

        out_file.write("\n")
