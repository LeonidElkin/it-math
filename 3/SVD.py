from abc import ABC, abstractmethod
from PIL import Image
import numpy as np


class AbstractSVD(ABC):
    def __init__(self, accuracy: int = 5):
        self.u = None
        self.v = None
        self.s = None
        self.accuracy = accuracy

    def truncate(self, is_compressed):
        accuracy = min(len(self.u), len(self.s), len(self.v))
        if is_compressed:
            accuracy = min(self.accuracy, accuracy)
        u_trunc = self.u[:, :accuracy]
        s_trunc = self.s[:accuracy]
        v_trunc = self.v[:accuracy, :]
        return u_trunc, s_trunc, v_trunc

    @abstractmethod
    def algorithm(self, imageMatrix) -> (np.ndarray, np.ndarray, np.ndarray):
        pass

    def encode(self, matrix):
        self.u, self.s, self.v = self.algorithm(matrix)
        return self

    def decode(self, is_compressed):
        if self.u is None or self.v is None or self.s is None:
            ValueError('U or V or S are not initialized')
        u, s, v = self.truncate(is_compressed)
        return np.clip(u @ np.diag(s) @ v, 0, 255).astype(np.uint8)


class NpSVD(AbstractSVD):
    def algorithm(self, matrix):
        u, s, v = np.linalg.svd(matrix)
        return u, s, v
