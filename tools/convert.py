import numpy as np
import matplotlib.pyplot as plt
from PIL import Image


def write_chunk_file(bit0, bit1):
    with open("sand_title", 'w') as out:
        out.write("sand ")
        out.write(str(8 * 8 * 8 * 2) + ' ')
        for i in range(8):
            for j in range(8):
                for p in range(8):
                    tem = bit0[i][j][p] | bit1[i][j][p]
                    for p in range(8):
                        if (tem >> p) & 1 == 0:
                            print i, j, p
                    out.write(str(bit0[i][j][p]))
                    out.write(" ")
                    out.write(str(bit1[i][j][p]))
                    out.write(" ")

    return


if __name__ == '__main__':
    img = np.array(Image.open('resize_sand.png'))
    plt.figure("dog")
    plt.imshow(img)
    plt.axis('off')
    plt.show()
    rows, cols, dims = img.shape
    print rows, cols

    palette = [(0, 0, 0, 0), (222, 201, 155, 250), (243, 221, 170, 250), (234, 212, 163, 250)]

    def closest(c):
        dist, ans = float('inf'), 0
        p = -1
        for i, s in enumerate(palette):
            d = sum([(s[x] - c[x]) ** 2 for x in range(len(s))])
            if d < dist:
                dist = d
                ans = s
                p = i
        return p, ans

    for i in range(rows):
        for j in range(cols):
            c = img[i][j]
            _, img[i][j] = closest(c)

    plt.figure("dog")
    plt.imshow(img)
    plt.axis('off')
    plt.show()

    bit0 = np.empty((8, 8, 8), dtype=int)
    bit1 = np.empty((8, 8, 8), dtype=int)

    for i in range(8):
        for j in range(8):
            for p in range(8):
                bit0[i][j][-p - 1] = 0
                bit1[i][j][-p - 1] = 0
                for q in range(8):
                    c = img[i * 8 + p][j * 8 + q]
                    ind, _ = closest(c)
                    bit0[i][j][-p - 1] |= (ind & 1) << q
                    bit1[i][j][-p - 1] |= ((ind >> 1) & 1) << q
                    if (ind & 1) << q == 0 and ((ind >> 1) & 1) << q == 0:
                        print ind, q, 0
                if bit0[i][j][-p - 1] == 0 and bit1[i][j][-p - 1] == 0:
                    print 1

    write_chunk_file(bit0, bit1)

