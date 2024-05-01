from copy import deepcopy

import numpy as np
from PIL import Image
from struct import pack, unpack, calcsize

from SVD import AbstractSVD


class BRUH:
    """Basic Representation Unified Helper"""

    class ImgMetaData:
        height: int
        width: int

    def open(self):
        image = Image.open(self.img_path)
        self.ImgMetaData.width, self.ImgMetaData.height = image.size
        return np.array(image)

    def __init__(self, img_path: str, encoder: AbstractSVD):
        self.img_path = img_path
        self.matrix = self.open()
        self.r_channel, self.g_channel, self.b_channel = list(
            map(lambda x, y: y.encode(x), [self.matrix[:, :, i] for i in range(3)],
                [deepcopy(encoder) for _ in range(3)]))

    def decode(self, is_compressed=True):
        img = np.zeros((self.ImgMetaData.height, self.ImgMetaData.width, 3), dtype=np.uint8)
        img[:, :, 0] = self.r_channel.decode(is_compressed)
        img[:, :, 1] = self.g_channel.decode(is_compressed)
        img[:, :, 2] = self.b_channel.decode(is_compressed)
        return Image.fromarray(img)
