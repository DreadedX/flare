#ifndef PROXY_ALLOCATOR_H
#define PROXY_ALLOCATOR_H

#include "extra/allocator.h"

class ProxyAllocator : public Allocator {

	public:

		ProxyAllocator(Allocator& allocator, const char *name);
		~ProxyAllocator();

		void *allocate(size_t size, uint8_t alignment) override;
		void deallocate(void *p) override;

	private:
		ProxyAllocator(const ProxyAllocator&);
		ProxyAllocator& operator=(const ProxyAllocator&);

		Allocator& _allocator;
};

namespace allocator {

	inline ProxyAllocator *make_new_proxy(Allocator &allocator, const char *name) {

		void *p = allocator.allocate(sizeof(ProxyAllocator), __alignof(ProxyAllocator));

		return new (p) ProxyAllocator(allocator, name);
	}

	inline void make_delete_proxy(ProxyAllocator &proxyAllocator, Allocator &allocator) {
		
		proxyAllocator.~ProxyAllocator();

		allocator.deallocate(&proxyAllocator);
	}
}

#endif
