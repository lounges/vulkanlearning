import os
import subprocess

def build_shader(source, destination):
    print(f"- Compiling Shader: {source} => {destination}")
    args = [
        "glslc",
        source,
        "-o",
        destination
    ]

    if os.path.exists(destination):
        print(f"\tRemoving existing file: \"{destination}\"")
        
    cmd = " ".join(args)
    print(f"\tCMD: \"{cmd}\"")

    proc = subprocess.Popen(cmd, stdout=subprocess.PIPE)
    proc.wait()
    if proc.returncode != None and proc.returncode != 0:
        raise Exception("command failed, see output.")
    

def main():
    input_directory = "resources/shaders/"
    output_directory = "build/resources/shaders/"

    if not os.path.exists(output_directory):
        os.makedirs(output_directory)
    
    shaders_to_compile = [
        {
            "source": "shader.vert",
            "out": "vert.spv"
        },
        {
            "source": "shader.frag",
            "out": "frag.spv"
        },
    ]

    print("Building Shaders...")
    for shader in shaders_to_compile:
        build_shader(f"{input_directory}{shader['source']}", f"{output_directory}{shader['out']}")

if  __name__ =='__main__':
    main()