// RUN: %clang_cc1 -std=c++20 -triple x86_64-unknown-linux-gnu -fclangir -emit-cir %s -o %t.cir
// RUN: FileCheck --input-file=%t.cir %s

// Test that __builtin_ia32_pshufd generates correct CIR vec.shuffle operations
// This verifies the fix for SIMD intrinsic support that was previously NYI

typedef int __v4si __attribute__((__vector_size__(16)));
typedef __v4si __m128i;

// CHECK-LABEL: @_Z11test_pshufdv
void test_pshufd() {
    __m128i vec = {1, 2, 3, 4};
    // CHECK: cir.vec.shuffle(%{{.*}}, %{{.*}} : !cir.vector<!s32i x 4>) [#cir.int<2> : !s32i, #cir.int<3> : !s32i, #cir.int<0> : !s32i, #cir.int<1> : !s32i] : !cir.vector<!s32i x 4>
    __m128i result = __builtin_ia32_pshufd(vec, 0x4E);
}

// CHECK-LABEL: @_Z19test_different_maskv  
void test_different_mask() {
    __m128i vec = {10, 20, 30, 40};
    // Test different immediate value: 0x1B = 00011011 = [3,2,1,0] reversed
    // CHECK: cir.vec.shuffle(%{{.*}}, %{{.*}} : !cir.vector<!s32i x 4>) [#cir.int<3> : !s32i, #cir.int<2> : !s32i, #cir.int<1> : !s32i, #cir.int<0> : !s32i] : !cir.vector<!s32i x 4>
    __m128i result = __builtin_ia32_pshufd(vec, 0x1B);
}

// CHECK-LABEL: @_Z19test_stb_image_casev
void test_stb_image_case() {
    __m128i p0 = {1, 2, 3, 4};
    
    // This reproduces the exact pattern from stb_image.h:2685 that was failing:
    // _mm_storel_epi64((__m128i *) out, _mm_shuffle_epi32(p0, 0x4e));
    // Which expands to: __builtin_ia32_pshufd(p0, 0x4e)
    // CHECK: cir.vec.shuffle(%{{.*}}, %{{.*}} : !cir.vector<!s32i x 4>) [#cir.int<2> : !s32i, #cir.int<3> : !s32i, #cir.int<0> : !s32i, #cir.int<1> : !s32i] : !cir.vector<!s32i x 4>
    __m128i out_vec = __builtin_ia32_pshufd(p0, 0x4e);
}
