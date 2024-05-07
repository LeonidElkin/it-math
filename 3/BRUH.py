from copy import deepcopy
from typing import NamedTuple, BinaryIO

from PIL import Image
from struct import pack, unpack, calcsize
from Packing import *

from SVD import *


class BRUH:
    """Basic Representation Unified Helper"""

    class __Meta(NamedTuple):
        img_width: int
        img_height: int
        algorithm: bytes

    __max_algo_name_length = 5
    __supported_algorithms = (NpSVD.name, b"Block", b"Power")
    __magic_value_pack = Packing(b'BRUH', "<4s")
    __meta_pack = Packing(None, f"<2I{__max_algo_name_length}s")
    __matrix_pack_format = "<5I"

    def __init__(self, file_path: str, encoder: AbstractSVD):
        self.__g_channel = None
        self.__r_channel = None
        self.__b_channel = None
        with (open(file_path, "rb") as f):
            if unpack(self.__magic_value_pack.format, f.read(calcsize(self.__magic_value_pack.format)))[0] \
                    == self.__magic_value_pack.value:
                self.__load_from_bruh(f, encoder)
            else:
                self.__load_from_image(file_path, encoder)

    def __load_channel_from_bruh(self, file: BinaryIO, encoder: AbstractSVD):
        sizes = unpack(self.__matrix_pack_format, file.read(calcsize(self.__matrix_pack_format)))
        u, s, v = [np.fromfile(file, dtype=np.float32, count=sizes[0] * sizes[1]).reshape(sizes[0], sizes[1]),
                   np.fromfile(file, dtype=np.float32, count=sizes[2]),
                   np.fromfile(file, dtype=np.float32, count=sizes[3] * sizes[4]).reshape(sizes[3], sizes[4])]
        return deepcopy(encoder)._AbstractSVD__setter(u, s, v, sizes)

    def __load_from_bruh(self, file: BinaryIO, encoder: AbstractSVD):
        self.__meta_pack.value = self.__Meta(
            *unpack(self.__meta_pack.format, file.read(calcsize(self.__meta_pack.format))))
        if self.__meta_pack.value.algorithm not in self.__supported_algorithms:
            ValueError(f"Invalid algorithm given. Must be one of {NpSVD.name} , Block, Power")
        self.__r_channel, self.__g_channel, self.__b_channel = [self.__load_channel_from_bruh(file, encoder) for _ in
                                                                range(3)]

    def __load_from_image(self, image_path: str, encoder: AbstractSVD):
        image = Image.open(image_path)
        if image.format != "BMP":
            ValueError("Image format is not BMP")
        self.__meta_pack.value = self.__Meta(image.width, image.height, encoder.name)
        image_matrix = np.array(image)
        self.__r_channel, self.__g_channel, self.__b_channel = list(
            map(lambda x, y: y.encode(x), [image_matrix[:, :, i] for i in range(3)],
                [deepcopy(encoder) for _ in range(3)]))

    def get_image(self):
        img = np.zeros((self.__meta_pack.value.img_height, self.__meta_pack.value.img_width, 3), dtype=np.uint8)
        img[:, :, 0], img[:, :, 1], img[:, :, 2] = list(
            map(lambda x: x.decode(), [self.__r_channel, self.__g_channel, self.__b_channel]))
        return Image.fromarray(img)

    def __pack_matrices(self):
        packing = b""
        for i in [self.__r_channel, self.__g_channel, self.__b_channel]:
            packing += pack(self.__matrix_pack_format,
                            *i.matrices_sizes) + i.u.tobytes() + i.s.tobytes() + i.v.tobytes()
        return packing

    def save_bruh(self, output_path: str):
        with open(output_path, 'wb') as f:
            f.write(pack(self.__magic_value_pack.format, self.__magic_value_pack.value))
            f.write(pack(self.__meta_pack.format, *self.__meta_pack.value))
            f.write(self.__pack_matrices())
