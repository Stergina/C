# Pizza Order Program in C


**Objective**:  
Implementing a system of ordering and distributing pizza using POSIX threads. Orders are entered in this system, each order gets prepared and once ready it is distributed to the customer. In these systems we have a large number of orders which are served by a limited number of service points, so the program implements mutual blocking using mutexes and synchronization using condition variables.<br/><br/>

**Steps**:  
The pizza place has a number of makers, ovens and distributors. Each order waits until a maker becomes available. When this happens, the maker needs a pre-specified time to prepare each pizza. The maker then waits for an oven to become available. When this happens, the maker places all the pizzas of the order in the same oven and gets released for the next order (he does not wait for the baking to end). The pizzas are baked for a pre-specified time. When baking is complete, the oven closes automatically and waits for a distributor. When a distributor becomes available, he takes a ready-made order, takes it out of the oven (which gets released), packs it and delivers it to the customer..<br/><br/>

**Structure of the code**:
The original thread of the program generates one thread per order. Each thread performs the aforementioned steps until the order is completed and prints the appropriate output. Finally, the initial thread prints the final output.
