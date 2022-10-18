import os
import argparse
import shutil

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument('glslc')
    parser.add_argument('output_path')
    parser.add_argument('shader_files', nargs='+')
    args = vars(parser.parse_args())

    p = os.path.abspath(args['output_path'])

    temp_path = os.path.join(p, "shaders/temp/")
    out_path = os.path.join(p, "shaders/")

    os.makedirs(out_path, exist_ok=True)
    os.makedirs(temp_path, exist_ok=True)

    shaders = args['shader_files']
    shaders = [{
        'in': os.path.join(os.getcwd(), s),
        'temp_out': os.path.join(temp_path, f"{os.path.basename(s)}.sprv"),
        'out': os.path.join(out_path, f"{os.path.basename(s)}.sprv.h"),
        'field_name': f"{os.path.basename(s).replace('.', '_')}"
    } for s in shaders]

    p = os.path.join(p, 'test.txt')
    for s in shaders:
        header = f"SD3D_SHADER{s['field_name'].upper()}"
        print(f"Compiling {s['in']}")
        if not os.system(f"{args['glslc']} {s['in']} -o {s['temp_out']}"):
            with open(s['temp_out'], 'rb') as sprv:
                data = sprv.read()
                h = [f"0x{d:02x}" for d in data]
                arr_len = len(h)
                arr = ', '.join(h)

                f = f"#ifndef {header}\n" \
                    f"#define {header}\n" \
                    f"\n" \
                    f"#include <array>\n" \
                    f"\n" \
                    f"using {s['field_name']}_t = std::array<uint8_t, {arr_len}>;\n" \
                    f"\n" \
                    f"constexpr {s['field_name']}_t {s['field_name']}_data = {{\n" \
                    f"\t{arr}\n" \
                    f"}};\n" \
                    f"\n" \
                    f"#endif // {header}\n"

                with open(s['out'], 'wt') as out_file:
                    out_file.write(f)

    shutil.rmtree(temp_path)
