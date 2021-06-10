/*
 * yup, this is a ruthless copy of the linux kernel implementation of a doubly linked list.
 * what can i say, it's the best one i've seen...
 */

#define container_of(ptr, type, member) ({				\
		(type *)((char *)ptr - offset_of(type, member)); })

#define offset_of(type, member)				    		\
	((unsigned int)&((type *)0)->member)

struct list_head {
	struct list_head *next;
	struct list_head *prev;
};

#define LIST_HEAD_INIT(name)				    		\
	struct list_head name = { &(name), &(name) }

static inline void __list_add(struct list_head *new, struct list_head *prev, struct list_head *next)
{
	new->prev = prev;
	prev->next = new;
	new->next = next;
	next->prev = new;
}

static inline void list_add(struct list_head *new, struct list_head *headp)
{
	__list_add(new, headp, headp->next);
}

static inline void list_add_tail(struct list_head *new, struct list_head *headp)
{
	__list_add(new, headp->prev, headp);
}

/* notice it does not remove the allocated space to the "deleted" node, it just unlinks it from the list (dlmalloc pun intended) */
static inline void __list_del(struct list_head *prev, struct list_head *next)
{
	prev->next = next;
	next->prev = prev;
}

static inline void list_del(struct list_head *rm)
{
	__list_del(rm->prev, rm->next);
}

static inline void list_del_tail(struct list_head *headp)
{
	list_del(headp->prev);
}


/*
 * Macros for queue over already implemented functions/macros.
 * @new:	a pointer to list_head struct to enqueue
 * @headp:	a pointer to the head of the linked list
 */

#define list_enqueue(new, headp)				\
	list_add_tail(new, headp);				    \

#define list_dequeue(headp) ({					\
		list_empty(headp) ?		        		\
		NULL :						            \
		({						                \
		struct list_head *__dq = (headp)->next;	\
		list_del((headp)->next);			    \
		__dq;						            \
		});						                \
	})

static inline int list_empty(struct list_head *headp)
{
	return (headp->next == headp);
}

/*
 * @ptr:	a pointer used as cursor through the list
 * @headp:	a pointer to the head of the linked list
 */
#define list_for_each(ptr, headp) \
	for (ptr = (headp)->next; ptr != (headp); ptr = ptr->next)



/* this thing below is something i used to play arround and see if the list, its functions and macros work properly */
/*
#include <stdio.h>

struct ct {
	int data;
	struct list_head list;
};

LIST_HEAD_INIT(lh);

int main()
{
	struct ct c1 = {.data = 1};
	struct ct c2 = {.data = 2};
	struct ct c3 = {.data = 3};
	struct ct ctail = {.data = 666};
	struct list_head *p;
	list_enqueue(&c1.list, &lh);
	list_enqueue(&c2.list, &lh);
	list_enqueue(&c3.list, &lh);
	list_enqueue(&ctail.list, &lh);
	fputs("enqueued c1, c2, c3 and ctail in that order\n", stdout);
	list_for_each(p, &lh) {
		printf("\toffset of list member: %d,\n\tcontainer of list address: %p,\n\taddress of list member: %p, \n\tcontainer of list data: %d\n",
				offset_of(struct ct, list),
				container_of(p, struct ct, list),
				&container_of(p, struct ct, list)->list,
				container_of(p, struct ct, list)->data);
		fputs("\t=============================\n", stdout);
	}
	p = list_dequeue(&lh);
	fputs("dequeued c1, now ptr holds the address of ct struct list member\n", stdout);
	fputs("checking if the address of conatinerof(ptr) is the same as the address of c1...\n", stdout);
	printf("\tcontainer_of(ptr) == &c1 ? %s\n", container_of(p, struct ct, list) == &c1 ? "YES" : "NO");
	fputs("printing remaining nodes in queue\n", stdout);
	list_for_each(p, &lh) {
		printf("\toffset of list member: %d,\n\tcontainer of list address: %p,\n\taddress of list member: %p, \n\tcontainer of list data: %d\n",
				offset_of(struct ct, list),
				container_of(p, struct ct, list),
				&container_of(p, struct ct, list)->list,
				container_of(p, struct ct, list)->data);
		fputs("\t=============================\n", stdout);
	}

	list_dequeue(&lh);
	list_dequeue(&lh);
	list_dequeue(&lh);
	printf("DEQUEUED every node\n");
	printf("is list empty? %s\n", list_empty(&lh) == 0 ? "no" : "yes");
	printf("Trying one last dequeue anyways... return value: %p\n", list_dequeue(&lh));
	return 0;
}
*/
