from headerutils import *

# counter-clockwise rot
def ccw_rc_rot(out_header, in_r, in_c):
    out_r = out_header["rows"] - in_c - 1
    out_c = in_r
    return out_r, out_c

# clockwise rot
def cw_rc_rot(out_header, in_r, in_c):
    out_r = in_c
    out_c = out_header["cols"] - in_r - 1
    return out_r, out_c

# walsh rot: maria elena walsh - reino del reves
def walsh_rc_rot(out_header, in_r, in_c):
    out_r = out_header["rows"] - in_r - 1
    out_c = out_header["cols"] - in_c - 1
    return out_r, out_c

# recibe offset en el cuerpo del archivo correspondiente a in_header
# produce offset en archivo (header + cuerpo) de output
def byte_rot(rc_rot, in_header, out_header, in_offset):
    in_pixel = BODYPX_OFFSET(in_header, in_offset)

    in_r, in_c = PIXEL2RC(in_header, in_pixel)
    out_r, out_c = rc_rot(out_header, in_r, in_c)

    out_pixel = RC2PIXEL(out_header, out_r, out_c)
    out_pos = HEADERSIZE(out_header) + BODYBYTE_OFFSET(out_header, out_pixel, in_offset)
    return out_pos

if __name__ == "__main__":
    in_hdr = {
        "content": "P6\n# Imagen ppm\n200 298\n255\n",
        "magic": "P6",
        "rows": 298,
        "cols": 200,
        "maxcolor": 255,
    }
    out_hdr = {
        "content": "P6\n# Imagen ppm\n298 200\n255\n",
        "magic": "P6",
        "rows": 200,
        "cols": 298,
        "maxcolor": 255,
    }
    print(byte_rot(ccw_rc_rot, in_hdr, out_hdr, 1))
    print(byte_rot(cw_rc_rot, in_hdr, out_hdr, 1))
