#include "Ref.h"

void RefIncrement(RefObject* n) {
	n->__count++;
}

bool RefDecrement(RefObject* n) {
	n->__count--;
	return (n->__count <= 0);
}