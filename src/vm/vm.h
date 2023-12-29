
struct supplemental_pageTable{

   struct hash spt_hash;

}


struct page
{
   const struct page_operations *operations;
   void *va;
   struct frame *frame;

   struct hash_elem hash_elem;
   
   union
   {
      struct uninit_page uninit;
      struct anon_page anon;
      struct file_page file;
   #ifdef EFILESYS
        struct page_cache page_cache;
   #endif

    };
};

