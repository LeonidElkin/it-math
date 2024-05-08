#Basic Representation Unified Helper

`BRUH` хранит в себе сжатое 24 битное `BMP` изображение, которое он может получсить из файла с `.bruh` расширением или из самого изображения. 
Сжатие производится посредством одного из трех SVD алгоритмов, которые можно выбрать вручную: `NumPy(Np)`, `Power Method(Pow)`, `Block Power Method(Block)`

## Структура BRUH

| Name               | Format     | Size in bytes | Value                                                         |
|--------------------|------------|---------------|---------------------------------------------------------------|
| magic              | <4s        | 4             | b'BRUH'                                                       |
| img_width          | <I         | 4             | Ширина исходного изображения в пикселях                       |
| img_height         | <I         | 4             | Высота исходного изображения в пикселях                       |
| compression_degree | <I         | 4             | Степень сжатие изображения                                    |
| algorithm          | <5s        | 5             | Название алгоритма использованного для сжатия(Np, Pow, Block) |
| red channel        | svd format | svd data size | SVD запакованное в байт стрим(смотрите ниже)                  |
| green channel      | svd format | svd data size | SVD запакованное в байт стрим(смотрите ниже)                  |
| blue channel       | svd format | svd data size | SVD запакованное в байт стрим(смотрите ниже)                  |

##Структура SVD

| Name     | Format | Size in bytes      | Value                     |
|----------|--------|--------------------|---------------------------|
| height   | <I     | 4                  | Высота матрицы u          |
| s_length | <I     | 4                  | Длина s вектора           |
| width    | <I     | 4                  | Ширина матрицы v          |
| u        | -      | height * s_length  | u матрица                 |
| s        | -      | s_length           | s вектор                  |
| v        | -      | s_length * width   | v матрица                 |

# Начало работы

Классу `BRUH` необходимо передать путь к `BMP` изображению и SVD алгоритм или путь к `bruh` файлу. При инициализации автоматически произойдет сжатие изображение.
Далее вы можете либо сохранить промежуточное представление в `bruh` файл с помощью метода `save_bruh` либо восстановить изображение с помощью метода `get_image`. 
Возвращаемое значение метода `get_image` - это `PIL.Image`

## Пример использования

```python
from src.lib.BRUH import *

svd_pow = PowerSVD(compression_degree=8, epsilon=1e-5, random_unit_vector_seed=0x3ae6ca)

bruh = BRUH("your_image.bmp", svd_pow)
bruh.save_bruh("your_bruh.bruh")
bruh_from_bruh = BRUH("your_bruh.bruh") #При получение BRUH из файла .bruh svd алгоритм указывать не обязательно
bruh_from_bruh.get_image().save("your_image_from_bruh.bmp")
```

