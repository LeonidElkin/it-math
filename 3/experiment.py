import subprocess
from os import path, listdir

cur_dir = path.dirname(path.abspath(__file__))
images_dir = path.join(cur_dir, 'images')
images_dirs = list(map(lambda x: path.join(images_dir, x, x), listdir(images_dir)))

app_dir = path.join(cur_dir, 'src', 'app.py')

for i in range(len(images_dirs)):
    subprocess.run(
        ['python', app_dir, 'compress', f'-i={images_dirs[i] + ".bmp"}', f'-o={images_dirs[i] + "_np" + ".bruh"}',
         f"-a=Np"])
    subprocess.run(
        ['python', app_dir, 'compress', f'-i={images_dirs[i] + ".bmp"}', f'-o={images_dirs[i] + "_pow" + ".bruh"}',
         f"-a=Pow"])
    subprocess.run(
        ['python', app_dir, 'compress', f'-i={images_dirs[i] + ".bmp"}', f'-o={images_dirs[i] + "_block" + ".bruh"}',
         f"-a=Block"])
    subprocess.run(['python', app_dir, 'decompress', f'-i={images_dirs[i] + "_np" + ".bruh"}',
                    f'-o={images_dirs[i] + "_np_decompressed" + ".bmp"}'])
    subprocess.run(['python', app_dir, 'decompress', f'-i={images_dirs[i] + "_pow" + ".bruh"}',
                    f'-o={images_dirs[i] + "_pow_decompressed" + ".bmp"}'])
    subprocess.run(['python', app_dir, 'decompress', f'-i={images_dirs[i] + "_block" + ".bruh"}',
                    f'-o={images_dirs[i] + "_block_decompressed" + ".bmp"}'])
