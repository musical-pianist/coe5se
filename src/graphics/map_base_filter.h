// ---- Windows GLU area filter for irregular RGBA4 map textures ----
#include <stdint.h>
#include <string.h>
#include <emmintrin.h>

typedef struct {
    int count,index[3];
    float weight[3];
} BfSpan;

// Choose the nearest supported power-of-two width.
static int bfTargetWidth(int w) {
    if(w<130 || w>529)return 0;
    if(w<192)return 128;
    return w<384 ? 256 : 512;
}

// Precompute the wrapped source coverage of each output texel.
static void bfSpans(BfSpan *spans,int input,int output) {
    const float step=(float)input/output;
    const float radius=step>1.0f ? step*0.5f : 0.5f;
    for(int i=0;i<output;i++) {
        float center=((float)i+0.5f)*step,lo=center-radius,hi=center+radius;
        int j=(int)lo;
        if((float)j>lo)--j;
        BfSpan *s=&spans[i];
        s->count=0;
        while((float)j<hi) {
            int wrapped=j<0 ? j+input : j>=input ? j-input : j;
            const float left=lo>(float)j ? lo : (float)j;
            const float right=hi<(float)(j+1) ? hi : (float)(j+1);
            s->index[s->count]=wrapped;
            s->weight[s->count++]=right-left;
            ++j;
        }
    }
}

// Expand four 8-bit channels to GLU's 16-bit working range.
static __m128 bfPixel(const uint8_t *p) {
    uint32_t packed;
    memcpy(&packed,p,sizeof packed);
    __m128i bytes=_mm_cvtsi32_si128((int)packed);
    __m128i words=_mm_unpacklo_epi8(bytes,bytes); /* byte * 257 */
    return _mm_cvtepi32_ps(_mm_unpacklo_epi16(words,_mm_setzero_si128()));
}

// Filter the base level before passing it to the native mipmap builder.
static int bfScale(const uint8_t *source,int w,int h,uint16_t *out,int dw,int dh) {
    if(!source || !out || (h!=71 && h!=72) || !bfTargetWidth(w) || dw!=bfTargetWidth(w) || dh!=64)return 0;
    BfSpan xs[512],ys[64];
    bfSpans(xs,w,dw);bfSpans(ys,h,dh);
    const float stepX=(float)w/dw;
    const float area=(stepX>1.f?stepX:1.f)*((float)h/dh);
    const __m128 divisor=_mm_set1_ps(area);
    for(int y=0;y<dh;y++){
        const BfSpan *sy=&ys[y];
        for(int x=0;x<dw;x++) {
            const BfSpan *sx=&xs[x];
            __m128 sum=_mm_setzero_ps();
            for(int j=0;j<sy->count;j++){
                const uint8_t *row=source+(size_t)sy->index[j]*w*4;
                for(int i=0;i<sx->count;i++) {
                    float coverage=sy->weight[j]*sx->weight[i];
                    sum=_mm_add_ps(sum,_mm_mul_ps(bfPixel(row+4*sx->index[i]),_mm_set1_ps(coverage)));
                }
            }
            __m128 scaled=_mm_div_ps(sum,divisor);
            __m128i values=_mm_cvttps_epi32(scaled);
            /* SSE2 packs signed words. Bias around 32768 to preserve the full unsigned range. */
            __m128i biased=_mm_sub_epi32(values,_mm_set1_epi32(32768));
            __m128i words=_mm_xor_si128(_mm_packs_epi32(biased,biased),_mm_set1_epi16((short)0x8000));
            _mm_storel_epi64((__m128i *)(out+4*((size_t)y*dw+x)),words);
        }
    }
    return 1;
}
