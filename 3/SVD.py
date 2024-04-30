from abc import ABC, abstractmethod
from PIL import Image
import numpy as np


class AbstractSVD(ABC):
    def __init__(self, accuracy: int = 5):
        self.u = None
        self.v = None
        self.s = None
        self.accuracy = accuracy

    def truncate(self, u, s, v: int):
        u_trunc = u[:, :self.accuracy]
        s_trunc = s[:self.accuracy]
        v_trunc = v[:self.accuracy, :]
        return u_trunc, s_trunc, v_trunc

    @abstractmethod
    def algorithm(self, imageMatrix) -> (np.ndarray, np.ndarray, np.ndarray):
        pass

    def encode(self, matrix):
        u, s, v = self.algorithm(matrix)
        self.u, self.s, self.v = self.truncate(u, s, v)
        return self

    def decode(self):
        if self.u is None or self.v is None or self.s is None:
            ValueError('U or V or S are not initialized')
        return np.clip(self.u @ np.diag(self.s) @ self.v, 0, 255).astype(np.uint8)


class NpSVD(AbstractSVD):
    def algorithm(self, matrix):
        u, s, v = np.linalg.svd(matrix)
        return u, s, v