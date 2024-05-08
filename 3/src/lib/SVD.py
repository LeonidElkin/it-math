import math
from abc import ABC, abstractmethod

import numpy as np


class AbstractSVD(ABC):
    name: bytes
    __num_of_bytes = 4

    def __init__(self, compression_degree: int = 2):
        self.u = None
        self.v = None
        self.s = None
        self.compression_degree = compression_degree
        self.matrices_sizes = None
        self._accuracy = None

    def __setter(self, u, s, v, sizes):
        self.u = u
        self.s = s
        self.v = v
        self.matrices_sizes = sizes
        return self

    def __truncate(self, u, s, v):
        accuracy = min(self._accuracy, len(u), len(s), len(v))
        u_trunc = u[:, :accuracy]
        s_trunc = s[:accuracy]
        v_trunc = v[:accuracy, :]
        return u_trunc, s_trunc, v_trunc

    @abstractmethod
    def _algorithm(self, matrix) -> (np.ndarray, np.ndarray, np.ndarray):
        pass

    def encode(self, matrix):
        h, w = matrix.shape
        self._accuracy = math.ceil(h * w // (self.__num_of_bytes * self.compression_degree * (h + w + 1)))
        self.u, self.s, self.v = list(map(lambda x: x.astype(np.float32), self.__truncate(*self._algorithm(matrix))))
        self.matrices_sizes = (*self.u.shape, *self.s.shape, *self.v.shape)
        return self

    def decode(self):
        if self.u is None or self.v is None or self.s is None:
            ValueError('U or V or S are not initialized')
        return np.clip(self.u @ np.diag(self.s) @ self.v, 0, 255).astype(np.uint8)


class NpSVD(AbstractSVD):
    name = b"Np"

    def _algorithm(self, matrix):
        u, s, v = np.linalg.svd(matrix)
        return u, s, v


class AbstractPowSVD(AbstractSVD, ABC):
    def __init__(self, compression_degree: int = 2, epsilon: float=1e-1, random_unit_vector_seed=0xebac0c):
        super().__init__(compression_degree)
        self.epsilon = epsilon
        self.rng = np.random.default_rng(seed=random_unit_vector_seed)

    def random_unit_vector(self, size):
        unnormalized = self.rng.uniform(0, 1, size=size)
        return unnormalized / np.linalg.norm(unnormalized)


class PowerSVD(AbstractPowSVD):
    name = b"Pow"

    def __svd_1d(self, matrix):

        current_v = self.random_unit_vector(matrix.shape[1])
        additional_matrix = matrix.T @ matrix

        while True:

            last_v = current_v
            current_v = additional_matrix @ last_v
            current_v /= np.linalg.norm(current_v)

            if abs(current_v @ last_v) > 1 - self.epsilon:
                break

        return current_v

    def _algorithm(self, matrix):
        current_svd = []

        for i in range(matrix.shape[1]):

            matrix_1d = matrix.copy().astype(np.float32)

            for s_value, u, v in current_svd[:i]:
                matrix_1d -= s_value * np.outer(u, v)

            v = self.__svd_1d(matrix_1d)
            u_unnormalized = matrix @ v
            sigma = np.linalg.norm(u_unnormalized)
            u = u_unnormalized / sigma

            current_svd.append((sigma, u, v))

        singular_vs, us, vs = [np.array(x) for x in zip(*current_svd)]

        return us.T, singular_vs, vs


class BlockSVD(AbstractPowSVD):
    name = b"Block"

    def _algorithm(self, matrix):

        matrix_t = matrix.T
        width = matrix.shape[1]
        v = np.array([self.random_unit_vector(self._accuracy) for _ in range(width)])

        while True:
            q, r = np.linalg.qr(matrix @ v)
            u = q[:, :self._accuracy]
            q, r = np.linalg.qr(matrix_t @ u)
            v = q[:, :self._accuracy]
            sigma = r[:self._accuracy, :self._accuracy]
            err = np.linalg.norm(matrix @ v - u @ sigma)
            if err <= self.epsilon:
                break

        return u, np.diag(sigma), v.T
