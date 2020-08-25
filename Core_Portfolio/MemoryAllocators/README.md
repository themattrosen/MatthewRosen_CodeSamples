## Memory Allocators

Memory allocators used by the Hydra project (DigiPen Course GAM 400). I've written two types of memory allocators, both inspired by _Game Engine Architecture_ by Jason Gregory. Making these thread-safe would require minimal lock protection, and would still result in very fast allocations/frees.

### Pool Allocator
The pool allocator's main use-case is for when you have a large group of objects that are all the same size that might be allocated and deallocated on the fly. The pool allocator reduces the cost of allocation/deallocation to a pop/push front operation on a singly linked list, all while staying in cache. Great for a flyweight pattern. I also used this extensively in my Engine-Done-Quick: Dandelion2D, also found on [my github page](https://github.com/themattrosen/Dandelion2D).

### Stack Allocator
The stack allocator has less use-cases than the pool allocator, but is still very valuable when the need arises. Also known as a "Frame Allocator", this allocator makes a pool of memory, and dishes out pieces of it sequentially, as if it was a stack. The drawback is that every item allocated this way must be deallocated in the reverse order that they were allocated in. If all items allocated are trivially destructable, then this lends to it's main use case of allocating a large number of objects and arrays over the course of one frame, using them, then clearing the allocator and resetting all the data in the stack. Allocating and freeing is even faster than a pool allocator, requiring only one += operation and returning a pointer. 

The main use of this was with an event manager. If the user wanted to queue events to happen at the end of a frame with variable sized event structs, I used the stack allocator to quickly allocate memory for the event to put in a queue. On frame end, I would just process all queued events and clear the stack allocator. 

Neither of these allocators are replacement for a global allocator commonly found on AAA titles, but they are good for an easy way to guarantee objects aligned in the cache and quickly created, without worry of fragmentation. 