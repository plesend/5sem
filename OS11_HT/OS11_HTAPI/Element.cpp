#include "pch.h"
#include "HT.h"

Element::Element()
	: key(nullptr), keylength(0), payload(nullptr), payloadlength(0)
{
}

Element::Element(const void* k, int klen)
	: key(nullptr), keylength(0), payload(nullptr), payloadlength(0)
{
	if (k && klen > 0) {
		void* kc = malloc((size_t)klen);
		if (kc) {
			memcpy(kc, k, (size_t)klen);
			key = kc;
			keylength = klen;
		}
	}
}

Element::Element(const void* k, int klen, const void* p, int plen)
	: key(nullptr), keylength(0), payload(nullptr), payloadlength(0)
{
	if (k && klen > 0) {
		void* kc = malloc((size_t)klen);
		if (kc) {
			memcpy(kc, k, (size_t)klen);
			key = kc;
			keylength = klen;
		}
	}
	if (p && plen > 0) {
		void* pc = malloc((size_t)plen);
		if (pc) {
			memcpy(pc, p, (size_t)plen);
			payload = pc;
			payloadlength = plen;
		}
	}
}

Element::Element(Element* oldelement, const void* newpayload, int newpayloadlength)
	: key(nullptr), keylength(0), payload(nullptr), payloadlength(0)
{
	if (oldelement && oldelement->key && oldelement->keylength > 0) {
		void* kc = malloc((size_t)oldelement->keylength);
		if (kc) {
			memcpy(kc, oldelement->key, (size_t)oldelement->keylength);
			key = kc;
			keylength = oldelement->keylength;
		}
	}
	if (newpayload && newpayloadlength > 0) {
		void* pc = malloc((size_t)newpayloadlength);
		if (pc) {
			memcpy(pc, newpayload, (size_t)newpayloadlength);
			payload = pc;
			payloadlength = newpayloadlength;
		}
	}
}

Element::~Element()
{
	if (key) { delete[]((void*)key); key = nullptr; keylength = 0; }
	if (payload) { delete[]((void*)payload); payload = nullptr; payloadlength = 0; }
}
