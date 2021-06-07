import sys
import os
import mmap
import stat
import threading

from parse import *
from rot import *
from llist import *

NCHILD = NCOLORS
NCONSUM = NCOLORS

out_mmap = None
rc_rot = None

mm_queues = []
mm_mtxs = []
mm_condvars = []

def consumer(in_header, out_header, n, color_offset, mmnode, rbc):
    b_per_px = BYTES_PER_PX(out_header)
    for b in range(0, n, b_per_px):
        color_byte = b + color_offset
        out_byte = byte_rot(rc_rot, in_header, out_header, rbc + color_byte)
        out_mmap[out_byte] = mmnode.mm[color_byte]

def consumer_wait(in_header, out_header, rsize, color_offset, mm_queue, mm_mtx, mm_condvar):
    bodybytes = BODYSIZE(out_header)
    leftbytes = bodybytes

    curr = None
    while leftbytes > 0:

        mm_mtx.acquire()
        curr = mm_queue.dequeue()
        while not curr:
            mm_condvar.wait()
            curr = mm_queue.dequeue()
        mm_mtx.release()

        n = rsize if rsize < leftbytes else leftbytes
        consumer(in_header, out_header, n, color_offset, curr, bodybytes - leftbytes)
        leftbytes -= n


def producer(in_header, filepath, rsize):
    rb = b""
    in_fd = os.open(filepath, os.O_RDONLY)

    os.lseek(in_fd, HEADERSIZE(in_header), os.SEEK_SET)
    rbc = 0
    while (rb := os.read(in_fd, rsize)) != b"":
        for i in range(len(mm_mtxs)):
            mm_mtxs[i].acquire()
            mmnode = Mem_Node(rb)
            mm_queues[i].enqueue(mmnode)
            mm_condvars[i].notify()
            mm_mtxs[i].release()

def w_mmap2file(out_filename):
    out_fd = os.open(out_filename, os.O_CREAT | os.O_RDONLY | os.O_WRONLY, stat.S_IRUSR | stat.S_IWUSR)
    wc = 0
    totalsize = FILESIZE(out_header)
    while wc < totalsize:
        wb = os.write(out_fd, out_mmap[wc:])
        wc += wb
    os.close(out_fd)

if __name__ == "__main__":
    args = parse_args(sys.argv[1:])

    if args["rotopt"] == WALSH:
        rc_rot = walsh_rc_rot
    elif args["rotopt"] == CW:
        rc_rot = cw_rc_rot
    else:
        rc_rot = ccw_rc_rot

    colorfilter = args["colorfilter"]
    doswap = args["rotopt"] != WALSH

    in_header = search_fileheader(args["filepath"])
    out_header = header_cp(in_header)

    if doswap:
        swap_rc(out_header)

    out_mmap = mmap.mmap(-1, FILESIZE(out_header))
    out_mmap.write(out_header["content"])

    rsize = PPM_ALIGN(in_header, args["rsize"])

    pool = []
    for i in range(NCHILD):
        if 1 << i & colorfilter:
            mm_queues.append(List())
            mm_mtxs.append(threading.Lock())
            mm_condvars.append(threading.Condition(mm_mtxs[-1]))

            pool.append(threading.Thread(
                target=consumer_wait,
                args=(in_header, out_header, rsize, i, mm_queues[-1], mm_mtxs[-1], mm_condvars[-1]))
            )
            pool[-1].start()

    producer(in_header, args["filepath"], rsize)
    for p in pool:
        p.join()

    if args["rotopt"] == WALSH:
        out_filename = "walsh." + args["filename"]
    elif args["rotopt"] == CW:
        out_filename = "cw." + args["filename"]
    else:
        out_filename = "ccw." + args["filename"]

    w_mmap2file(out_filename)

    out_mmap.close()
