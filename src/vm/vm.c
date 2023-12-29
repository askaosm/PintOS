#define "vm/vm.h"

void supplemental_page_table_init(struct supplemental_page_table *spt UNUSED)
{
    hash_init(spt, page_hash, page_less, NULL);
}

unsigned
page_hash(const struct hash_elem *p_, void *aux UNUSED)
{
    const struct page *p = hash_entry(p_, struct page, hash_elem);
    return hash_bytes(&p->va, sizeof p->va);
}


bool page_less(const struct hash_elem *a_,
               const struct hash_elem *b_, void *aux UNUSED)
{
    const struct page *a = hash_entry(a_, struct page, hash_elem);
    const struct page *b = hash_entry(b_, struct page, hash_elem);

    return a->va < b->va;
}


struct page *
spt_find_page(struct supplemental_page_table *spt UNUSED, void *va UNUSED)
{
    struct page *page = NULL;
    
    page = malloc(sizeof(struct page));
    struct hash_elem *e;

    
    page->va = va;
    e = hash_find(&spt, &page->hash_elem);

    return e != NULL ? hash_entry(e, struct page, hash_elem) : NULL;
}




bool spt_insert_page(struct supplemental_page_table *spt UNUSED,
                     struct page *page UNUSED)
{
    /* TODO: Fill this function. */
    return hash_insert(&spt, &page->hash_elem) == NULL ? true : false; // 존재하지 않을 경우에만 삽입
}



static struct frame *
vm_get_frame(void)
{
    struct frame *frame = NULL;
 
    void *kva = palloc_get_page(PAL_USER); 

    if (kva == NULL)   // page 할당 실패 -> 나중에 swap_out 처리
        PANIC("todo"); // OS를 중지시키고, 소스 파일명, 라인 번호, 함수명 등의 정보와 함께 사용자 지정 메시지를 출력

    frame = malloc(sizeof(struct frame)); // 프레임 할당
    frame->kva = kva;                      // 프레임 멤버 초기화

    ASSERT(frame != NULL);
    ASSERT(frame->page == NULL);
    return frame;
}


static bool
vm_do_claim_page(struct page *page)
{
    struct frame *frame = vm_get_frame();

    /* Set links */
    frame->page = page;
    page->frame = frame;

    /* TODO: Insert page table entry to map page's VA to frame's PA. */
    // 가상 주소와 물리 주소를 매핑
    struct thread *current = thread_current();
    pml4_set_page(current->pml4, page->va, frame->kva, page->writable);

    return swap_in(page, frame->kva); // uninit_initialize
}


bool vm_claim_page(void *va UNUSED)
{
    struct page *page = NULL;
    /* TODO: Fill this function */
    // spt에서 va에 해당하는 page 찾기
    page = spt_find_page(&thread_current()->spt, va);
    if (page == NULL)
        return false;
    return vm_do_claim_page(page);
}
