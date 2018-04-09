#include <iostream>
#include <memory>
#include <unistd.h>


// compile: g++ -std=c++14 -O3 unique.cpp -o unique

static int numinserts = 1000;

struct node {
	int val;
	std::unique_ptr<struct node> next;
};

static unsigned long long rdtsc()
{
	unsigned hi, lo;

	asm volatile("rdtsc" : "=a"(lo), "=d"(hi));
	return (((unsigned long long)hi) << 32) | lo;
}

int delete_node(std::unique_ptr<struct node>& root, int idx)
{
	if (root == NULL || idx < 0)
		return 0;

	if (idx == 0) {
		root = std::move(root->next);
		return 1;
	}

	std::unique_ptr<struct node>* rootp = &root;
		
	while (*rootp) {
		if (idx == 1) {
			std::unique_ptr<struct node> todel = std::move((*rootp)->next);
			if (todel)
				(*rootp)->next = std::move(todel->next);
			return 1;
		}
		rootp = &(*rootp)->next;
		idx--;
	}
	return 0;
}

int iterate_list(std::unique_ptr<struct node>& root)
{
	if (root == NULL)
		return 0;

	std::unique_ptr<struct node>* rootp = &root;
	int ret = 0;

	while (*rootp) {
		ret++;
		rootp = &(*rootp)->next;
	}
	return ret;
}

int insert_nodes(std::unique_ptr<struct node>& root)
{
	std::unique_ptr<struct node> elem;
	elem = std::unique_ptr<struct node>(new(struct node));
	if (elem == NULL)
		return 0;
	elem->val = 0;
	elem->next = std::move(root);
	root = std::move(elem);
	return 1;
}

int main(int argc, const char *argv[])
{
	std::unique_ptr<struct node> head = NULL;
	int numelems;
	unsigned long long time;

	if (argc == 2) {
		numinserts = atoi(argv[1]);
	}
	printf("numinserts : %d\n", numinserts);
	if (numinserts < 0)
		return 0;

	time = rdtsc();
	for (int i = 0; i < numinserts; i++) {
		if (!insert_nodes(head)) {
			std::cout << "failed to insert node : " << i << std::endl;
			return 0;
		}
	}
	printf("time taken during insert: %lld ms\n", (rdtsc() - time)/2530000);

	numelems = iterate_list(head);
	if (numelems != numinserts) {
		printf("test failed! %d %d\n", numelems, numinserts);
	}

	time = rdtsc();
	for (int i = 0; i < numinserts/20; i++) {
		int idx = rand() % ((numinserts/20) - i);
		if (!delete_node(head, idx)) {
			std::cout << "failed to delete node : " << idx << std::endl;
			return 0;
		}
	}
	printf("time taken during delete: %lld ms\n", (rdtsc() - time)/2530000);

	numelems = iterate_list(head);
	if (numelems != numinserts - (numinserts/20)) {
		printf("test failed! %d %d\n", numelems, numinserts);
	}

	time = rdtsc();
	head = NULL;
	printf("time taken during cleanup: %lld ms\n", (rdtsc() - time)/2530000);

	return 0;
}
