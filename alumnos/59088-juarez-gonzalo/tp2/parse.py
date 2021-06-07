import getopt
import os
import sys

H_MAXSIZE = 512
INFONL = 3

def btoi(b_arr):
    ret = 0
    for i in range(len(b_arr)):
        ret = ret * 10 + (b_arr[i] - ord('0'))
    return ret

def usagendie():
    h = "usage: %s [-h] (-s|--size) SIZE (-f|--file) FILE\n\n" % __file__
    h += "TP1 - procesa ppm\n\n"
    h += "\t-h, --help\tMuestra esta ayuda\n"
    h += "\t-s, --size\tTamaño del bloque de lectura\n"
    h += "\t-f, --file\tArchivo a procesar\n"
    h += "\t-s, --signed\tModifica la rotacion default (sentido de agujas del reloj) a contrarreloj\n"
    sys.stdout.write(h)
    sys.exit(1)

CCW = 1
CW = 2
WALSH = 3

def parse_args(argv):
    opt, args = getopt.getopt(argv, "s:f:h", ["size=", "file=", "sentido", "walsh", "help"])
    filename = ""
    filepath = ""
    rsize = 0
    rotopt = CCW

    for o in opt:
        oname = o[0].replace("-","")

        if oname == "sentido":
            rotopt = CW
            continue

        if oname[0] == "w":
            rotopt = WALSH
            continue

        if oname[0] == "s":
            rsize = int(o[1])
            continue

        if oname[0] == "f":
            filename = o[1]
            continue

            usagendie()

    if not filename or not rsize:
        usagendie()

    if not os.path.isfile(filename):
        raise ValueError("%s no es un archivo" % filename)

    filepath = os.path.realpath(filename)

    # super mega validacion de formato de archivo
    if ".ppm" != filename[-4:]:
        raise ValueError("Archivo no tiene extensión ppm")

    return {
        "filename": filename,
        "filepath": filepath,
        "rsize": rsize,
        "rotopt": rotopt
    }

def search_fileheader(filepath):
    fd = os.open(filepath, os.O_RDONLY)
    rb = os.read(fd, H_MAXSIZE)
    hdrfields = parseheader(rb)
    os.close(fd)
    return hdrfields

def parseheader(b_arr):
    hdr_uncmmnt = bytearray()
    nl_count = 0
    f_idx = 0
    in_cmmnt = 0
    c = b""

    for i in range(len(b_arr)):

        if b_arr[i] == ord("#"):
            in_cmmnt |= 1
            continue
        if b_arr[i] == ord("\n") and in_cmmnt:
            in_cmmnt ^= in_cmmnt
            continue
        if b_arr[i] == ord("\n"):
            nl_count += 1
        if not in_cmmnt:
            hdr_uncmmnt += b_arr[i].to_bytes(1, byteorder="big")
        if nl_count == INFONL:
            f_idx = i + 1
            break

    if nl_count != INFONL:
        raise ValueError("Header superior a %d. Seguro que es un ppm?" % H_MAXSIZE)

    hdr_fields = hdr_uncmmnt.split(b"\n")
    cols, rows = hdr_fields[1].split(b" ")

    return {
        "content": b_arr[:f_idx],
        "magic": hdr_fields[0],
        "cols": btoi(cols),
        "rows": btoi(rows),
        "maxcolor": btoi(hdr_fields[2]),
    }
