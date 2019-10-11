
#pragma once

template<typename T, u32 N>
struct array {
    T data[N] 	 = {};
    static constexpr u32 capacity = N;

    static array copy(array source) {
        array ret;
        memcpy(ret.data, source.data, sizeof(T) * N);
        return ret;
    }

    void destroy() {
        if constexpr(is_Destroy<T>()) {
            for(T& v : *this) {
                v.destroy();
            }
        }
    }
    
    T& operator[](u32 idx) {
        assert(idx >= 0 && idx < N);
        return data[idx];
    }
    T operator[](u32 idx) const {
        assert(idx >= 0 && idx < N);
        return data[idx];
    }

    const T* begin() const {
        return data;
    }
    const T* end() const {
        return data + N;
    }
    T* begin() {
        return data;
    }
    T* end() {
        return data + N;
    }
};

template<typename T, u32 N> 
struct Type_Info<array<T,N>> {
	static constexpr char name[] = "array";
	static constexpr usize size = sizeof(T [N]);
	static constexpr Type_Type type = Type_Type::array_;
	static constexpr usize len = N;
	using underlying = T;
};
