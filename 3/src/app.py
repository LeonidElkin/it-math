from lib.BRUH import BRUH
from argparse import *
from PIL import Image


def bruh_extension_check(path):
    bruh_extension_length = 5
    length = len(path)
    extension = path[length - bruh_extension_length:length]
    return length <= bruh_extension_length or extension != ".bruh" and extension != ".BRUH"


def bmp_extension_check(path):
    bmp_extension_length = 4
    length = len(path)
    extension = path[length - bmp_extension_length:length]
    return length <= bmp_extension_length or extension != ".bmp" and extension != ".BMP"


def compress(args):
    if bruh_extension_check(args.output):
        ValueError("Not a bruh file has been given as output")
    if bmp_extension_check(args.input):
        ValueError("Not a bmp file has been given as input")
    if args.algorithm not in BRUH.supported_algorithms:
        raise ValueError("Incorrect algorithm has been given. Look up in the --help")
    svd = BRUH.supported_algorithms[args.algorithm](args.compression_degree)
    BRUH(args.input, svd).save_bruh(args.output)


def decompress(args):
    if bruh_extension_check(args.input):
        ValueError("Not a bruh file has been given as input")
    if bmp_extension_check(args.output):
        ValueError("Not a bmp file has been given as output")
    (BRUH(args.input).get_image()).save(args.output)


if __name__ == '__main__':
    parser = ArgumentParser(
        description="Compress BMP 24 bit image using SVD algorithms to BRUH (Basic Representation Unified Helper) "
                    "format")

    subparsers = parser.add_subparsers(help='BRUH methods. It might be compress or decompress')

    operation = subparsers.add_parser('compress', help="Compress BMP 24 bit image")

    operation.add_argument('-i', '--input', type=str, required=True,
                           help='Input image file path to compress. It must be 24 bit BMP')
    operation.add_argument('-o', '--output', type=str, required=True,
                           help='Output bruh file. Must have .bruh extension')
    operation.add_argument('-a', '--algorithm', type=str, required=False, default='Np',
                           help='SVD algorithm to use for compression. It must be one of the following: Np, Pow, '
                                'Block. Default is Np')
    operation.add_argument('-c', '--compression_degree', type=int, required=False, default=2,
                           help='Degree of compression. Default is 2')
    operation.set_defaults(func=compress)

    operation = subparsers.add_parser('decompress', help="Decompress BMP 24 bit image from bruh file")

    operation.add_argument('-i', '--input', type=str, required=True,
                           help='Input bruh file to decompress. Must have .bruh extension')
    operation.add_argument('-o', '--output', type=str, required=True, help='Output BMP 24 bit image file path')
    operation.set_defaults(func=decompress)

    arguments = parser.parse_args()

    arguments.func(arguments)
