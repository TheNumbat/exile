
#pragma once

u8* base_alloc(usize sz);
void base_free(void* mem);

void mem_validate();

template<const char* tname>
struct Mallocator {
    static constexpr const char* name = tname;

    template<typename T> 
    static T* make(usize n = 1) {
        return new (alloc<T>(sizeof(T) * n, alignof(T))) T[n];
    }

    template<typename T>
    static T* alloc(usize size, usize align = 1);

    template<typename T>
    static void dealloc(T* mem) {
        base_free(mem);
    }
};

struct Mvirtual {
    static constexpr const char* name = "Mvirtual";

    template<typename T>
    static T* alloc(usize size, usize align = 1);

    template<typename T>
    static void dealloc(T* mem);
};

template<const char* tname, usize N>
struct Marena {

    static constexpr const char* name = tname;
    static inline u8 mem[N] = {};
    static inline usize used = 0;

    template<typename T> 
    static T* make(usize n = 1) {
        return new (alloc<T>(sizeof(T) * n, alignof(T))) T[n];
    }

    template<typename T>
    static T* alloc(usize size, usize align = 1);

    template<typename T>
    static void dealloc(T* mem) {}

    static void reset() {
        used = 0;
        memset(mem, 0, N);
    }
};

template<const char* tname, typename T, typename Base>
struct Free_List {
    
    union norefl Free_Node {
        T value;
        Free_Node* next = null;
    };
    
    static constexpr const char* name = tname;
    static inline Free_Node* list = null;

    static T* make() {
        return new (alloc()) T;
    }

    static T* alloc() {
        if(list) {
            Free_Node* ret = list;
            list = list->next;
            memset(ret, 0, sizeof(Free_Node));
            return (T*)ret;
        }
        return (T*)Base::template make<Free_Node>();
    }

    static void dealloc(T* mem) {
        Free_Node* node = (Free_Node*)mem;
        node->next = list;
        list = node;
    }

    static void clear() {
        while(list) {
            Free_Node* next = list->next;
            Base::dealloc(list);
            list = next;
        }
    }
};

static constexpr char Mdefault_name[] = "Mdefault";
using Mdefault = Mallocator<Mdefault_name>;
