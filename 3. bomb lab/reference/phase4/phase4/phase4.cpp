#include <stdio.h>

int logicalShift(int x, int n) {
	//to be implemented
	int mask = ~(~0 << (31 + (~n + 1)) << 1);
	int arithShift_x = x >> n;
	return arithShift_x & mask;
}

int func4(int edi, int esi, int edx) {
	int eax, ebx;
	eax = edx - esi;
	ebx = logicalShift(eax, 31); // logical
	eax = (eax + ebx) >> 1;  // arithmetic
	ebx = eax + esi;

	if (ebx - edi <= 0) {
		// <+33>
		eax = ebx;
		if (ebx - edi >= 0) return eax;
		
		else {
			esi = ebx + 1;
			eax = func4(edi, esi, edx);
			eax += ebx;
			return eax;
		}
	}
	else {
		edx = ebx - 1;
		eax = func4(edi, esi, edx);
		eax += ebx;
		return eax;
	}
}

int main() {
	int value = func4(4, 0, 14);
	printf("%d", value);
}



