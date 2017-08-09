//Functions
ctsar * ctsar_init(ctsar_type sched, ctsar_dev_type devs,
                      double *rat, int *div);
////Scheduling
ctsar * ctsar_next(ctsar * c, int size);
int     ctsar_loop(ctsar * c);
////Memory management
void  * ctsar_reg_mem_2d(ctsar * c, void * ptr,
                         size_t item_size, size_t rows, 
                         size_t cols, ctsar_mem flags, 
                         size_t pad_x, size_t pad_y);
void  * ctsar_reg_mem(ctsar * c, void * ptr,
                     size_t item_size, size_t count
                     ctsar_mem flags);
void ctsar_unreg_mem(ctsar * c, void * ptr);
void ctsar_retarget_mem(ctsar * c, void * old, void * new);
typedef void(* ctsar_reduc_function)(void*,void*);
void  * ctsar_reg_reduc(ctsar * c, void * ptr, 
                        void * identity, size_t item_size, 
                        ctsar_reduc_function reduc); 

//Macros
CSTART(ctsar *c);
CEND(ctsar *c);

