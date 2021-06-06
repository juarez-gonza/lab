class List_Head():
    def __init__(self):
        self.next = None
        self.prev = None

class List():
    def __init__(self):
        self.head = List_Head()
        self.head.next = self.head
        self.head.prev = self.head

    def empty(self):
        return self.head == self.head.next

    def _add(self, new, prev, next):
        new.next = next
        next.prev = new
        new.prev = prev
        prev.next = new

    def add(self, new):
        self._add(new, self.head, self.head.next)

    def add_tail(self, new):
        self._add(new, self.head.prev, self.head)

    def _delete(self, prev, next):
        prev.next = next
        next.prev = prev

    def delete(self, rm):
        self._delete(rm.prev, rm.next)

    def del_tail(self):
        list_delete(self.head.prev)

    def enqueue(self, new):
        self.add_tail(new)

    def dequeue(self):
        dq = None
        if not self.empty():
            dq = self.head.next
            self.delete(self.head.next)
        return dq

    def singly_next_safe(self, curr):
        next = None
        if curr.next != self.head:
            next = curr.next
        return next

    def print_list(self):
        curr = self.head.next
        while curr != self.head:
            print(curr)
            curr = curr.next

class Mem_Node(List_Head):
    # necesita exclusion mutua externa antes de modificar ttl
    # mmap y mmunmap externos
    def __init__(self, mm):
        self.mm = mm
        super().__init__()

if __name__ == "__main__":
    l = List()
    l.enqueue(Mem_Node(0))
    curr = l.head.next
    while curr != l.head:
        print(curr)
        curr = curr.next
    print(l.dequeue())
    curr = l.head.next
    while curr != l.head:
        print(curr)
        curr = curr.next
    print(l.dequeue())
    curr = l.head.next
