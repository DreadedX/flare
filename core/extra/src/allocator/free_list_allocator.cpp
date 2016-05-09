#include "extra/allocator/free_list_allocator.h"

#include <stdio.h>

FreeListAllocator::FreeListAllocator(size_t size, void *start)
	: Allocator(size, start), _free_blocks((FreeBlock*)start) {

		assert(size > sizeof(FreeBlock));

		_free_blocks->size = size;
		_free_blocks->next = nullptr;
}

FreeListAllocator::~FreeListAllocator() {

	_free_blocks = nullptr;
}

void *FreeListAllocator::allocate(size_t size, uint8_t alignment) {

	assert(size != 0 && alignment != 0);

	FreeBlock *prev_free_block = nullptr;
	FreeBlock *free_block = _free_blocks;

	while(free_block != nullptr) {

		uint8_t adjustment = pointer_math::alignForwardAdjustmentWithHeader(free_block, alignment, sizeof(AllocationHeader));

		size_t total_size = size + adjustment;

		if (free_block->size < total_size) {

			prev_free_block = free_block;
			free_block = free_block->next;

			continue;
		}

		static_assert(sizeof(AllocationHeader) >= sizeof(FreeBlock), "sizeof(AllocationHeader) < sizeof(FreeBlock)");

		if (free_block->size - total_size <= sizeof(AllocationHeader)) {

			total_size = free_block->size;

			if (prev_free_block != nullptr) {

				prev_free_block->next = free_block->next;
			} else {

				_free_blocks = free_block->next;
			}
		} else {

			FreeBlock *next_block = (FreeBlock*)( pointer_math::add(free_block, total_size) );
			next_block->size = free_block->size - total_size;
			next_block->next = free_block->next;

			if (prev_free_block != nullptr) {

				prev_free_block->next = next_block;
			} else {
				
				_free_blocks = next_block;
			}
		}

		uintptr_t aligned_adress = (uintptr_t)free_block + adjustment;

		AllocationHeader* header = (AllocationHeader*)(aligned_adress - sizeof(AllocationHeader));
		header->size = total_size;
		header->adjustment = adjustment;

		_used_memory += total_size;
		_num_allocations++;

		assert(pointer_math::alignForwardAdjustment((void*)aligned_adress, alignment) == 0);

		return (void*)aligned_adress;
	}

	printf("Could not find free block large enough\n");

	return nullptr;
}

void FreeListAllocator::deallocate(void *p) {

	assert(p != nullptr);

	AllocationHeader *header = (AllocationHeader*)pointer_math::subtract(p, sizeof(AllocationHeader));

	uintptr_t block_start = reinterpret_cast<uintptr_t>(p) - header->adjustment;
	size_t block_size = header->size;
	uintptr_t block_end = block_start + block_size;

	FreeBlock *prev_free_block = nullptr;
	FreeBlock *free_block = _free_blocks;

	while(free_block != nullptr) {

		if ( (uintptr_t)free_block >= block_end) {

			break;
		}

		prev_free_block = free_block;
		free_block = free_block->next;
	}

	if(prev_free_block == nullptr) {

		prev_free_block = (FreeBlock*) block_start;
		prev_free_block->size = block_size;
		prev_free_block->next = _free_blocks;

		_free_blocks = prev_free_block;
	} else if( (uintptr_t)prev_free_block + prev_free_block->size == block_start) {

		prev_free_block->size += block_size;
	} else {

		FreeBlock *temp = (FreeBlock*) block_start;
		temp->size = block_size;
		temp->next = prev_free_block->next;
		prev_free_block->next = temp;

		prev_free_block = temp;
	}

	if(free_block != nullptr && (uintptr_t)free_block == block_end) {
		
		prev_free_block->size += free_block->size;
		prev_free_block->next = free_block->next;
	}

	_num_allocations--;
	_used_memory -= block_size;
}
