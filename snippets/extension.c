//items in {} are optional
#pragma acc region \
  hetero(<cond>{,<devs>{,<sched.>{,<ratio>{,<div>}}}})\
  part_copy{in/out}(<var>{(<size>}[<cond>:<num>{:<halo>}])\
  persist(<var>)
#pragma acc depersist(<var>)
